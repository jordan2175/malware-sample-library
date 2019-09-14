#pragma once

#include "core\core.h"
#include "WndRec\sender.h"

namespace WndRec
{

const int MinSizePackage = 4096; //���������� �������� ���������� ������ ��� ������

const int MaxIPOnServer = 4; //���������� IP ������� �������� �� ���� �������� (�������� � �����������)
const int MaxServers = 10; //������������ ���������� ���������� ��������

//����� ������������ ������
struct Package
{
	uint id;
	int cmd;
	uint time; //����� �������� ������, � ������������
	Mem::Data data; //������ � ������
	int flags; //1 - ��� ������ ����� ������� ����� �� ������
	Package* next;
};

struct ListPackage
{
	CRITICAL_SECTION lock;
	Package* last;
	int count;
};

struct IPServer
{
	char ip[128];
	int port;
};

struct ServerData
{
//	int scServer; //����� ����� � ��������
	IOServer io;
	IPServer ip[MaxIPOnServer]; //������ ��������
	int c_ip;
	int currIP; //����� ������� � �������� ������������
	int stopManager; //��� ��������� ������ ���������, 0 - �������� ��������, 1 - ����� ��������� ����� ���������
					 //2 - ����� ��������

	ListPackage packageIn; //������ ������� ������� (���������� ��������)
	ListPackage packageOut; //������ ��������� ������� (���������� �������)
	ListPackage packageFree; //������ ��������� �������

	uint downtime; //������������ ����� �������
	SenderBuf* buf; //������ ��� �������� ������ � ������
	int flags; // ������ �������� flags � ������� Init

	DWORD timeConnect; //����� ��������
	int attemptsConnect; //������� ��������
	int sleep; //����� ������ � �������� �� ����� ����������
};

//������������� ���������, ���������� TRUE - �������� �������, FALSE - ������ �������������
//flags: 1 - ������ � ������ ������, 2 - �� ��������� �� ���������� ip � port (������������ ��� �������� ������������ ����������)
//		4 - ��������� ������ ������� ��� ������ �����, 8 - ���� ����� �� ���������� ����������� � ��������, �� ��������� ������ ���� 
//downtime - ����� ������� � �������, �� ��������� �������� �������� ����������� �� �������
//			���� 0, �� ����� ����� MaxTimeNoAction
//���������� �� ���������, �� �������� ����� ���������� � ��������� ��������
ServerData* Init( int flags, const char* ip, int port, int downtime );
//��������� ��������
void Release( ServerData* );
//�������� ����������� � ��������� ������� �������, ������ ���� �� ���� � ������ ��������
//������ ���������� �����������
bool Reconnect( ServerData* server, const char* ip, int port );
//���������� �������� 
//setFlags - ����� ������� ����� ����������, resetFlags - ����� ������� ����� ��������
//downtime - ����� ����� ���������� � �������, ���� - -1, �� �� ����� ������
void UpdateSettings( ServerData*, int setFlags, int resetFlags, int downtime );
//���������� ������� � ������� �� ������, ���������� ������������� ���������� ������, ���� ��������� 0,
//�� ������ ��������.
//���� ����� ����� ������ ����� 4096 ����, �� �� ������� �� ��������� �������, ������ �� ����� 4096 ����,
//��� ���� ��� ������ ����� ����� ���� � ���� �������
//flags - 1 - ��� ������� ����� ������� ����� �� �������, 2 - ��� ������ ����� �� ����� ������������� �����
//� ���������� 0, ����� ����� ��� ��� ����� �����, 4 - �������� ����� � ������ ������� (����� ������� ���������),
DWORD WriteData( ServerData* server, int cmd, const void* data, int c_data, int flags = 0, uint id = 0 );
//���� ����� ������������ ������� ������ ������ min �� �� ����� wait ����������� 
void WaitSendedPackages( ServerData* server, int min, int wait );
//������ ������ �� �������������� �������������� �������� WriteData. ���������� ����� ���������� ������.
//data - ��������� �� ����� ���� ����� ������� ������, szData - ������ ������, ���� szData < 0, ��
//������� ���� �������� ������ ���������� ������ � ���������� � data, � ����� ������� ����� ����� �����������
//������.
//wait - ���������� ������ �������� ������
//���� ������������ 0, �� ������ ������, ���� -1, �� ������ ��� ���, ����� ��� �����
int ReadData( ServerData* server, DWORD id, int cmd, void** data, int szData, int wait );
//���� ��� � ReadData ������ � data ��� ��������� ���������� ������
int ReadData2( ServerData* server, DWORD id, int cmd, void* data, int szData, int wait );
//�������� ������� � ������� � �������� ���������. �������� ����� WriteData() � ReadData()
int GetData( ServerData* server, int cmd, void* sendData, int c_sendData, void** recvData, int szRecvData, int flags, int wait );
//���� ��� � GetData ������ � recvData ��� ��������� ���������� ������
int GetData2( ServerData* server, int cmd, void* sendData, int c_sendData, void* recvData, int szRecvData, int flags, int wait );
//����� �� ������. ���� ��� ����� �� �������� ��������, �� �� ������ � ����� ������ (����������� �� �������)
//��� ������� ���������� ����� � ���� ���� ���������� ����������, ���� ��� ������, �� ���������� 0
//���� notWait = true, �� �� ���� ���� ������ �� ������ ������
int OutOfHibernation( ServerData* server, bool notWait = false );
//���������� IP ������������ �������, �� ������ ���� � ��������� �� ����� �������
//���������� TRUE, ���� IP ��������
DWORD AddIPServer( ServerData* server, const char* ip, int port );

//�������� ����� ������� �������
Package* AllocPackage( ServerData* server, int sz );
//���������� ����� ���������� AllocPackage(), ���������� id ������. � ������ ������ ���� ��������
//������� � ���� data � ������ �� ������ � c_data
//�������� flags ��. � ������� WriteData
DWORD WritePackage( ServerData* server, int cmd, Package* p, int flags = 0 );

const char* GetIPServer(ServerData* server);
int GetPortServer(ServerData* server);

//��������� ������� �� �������, ��������� ������ �������� � ���������� ������� cmds: cmds[i][0]-cmds[i][1] ������������,
//��������� ���� ������� ������ ���� ������, ��� ������������� �� ��������� ����������. 
//��������� ������� ������� � cmd, ������������ ���������� ���� ��������� � ��������, � data - ���� ��� ������,
//szData - ������ ������ data. wait - ����� �������� ������� � �������������
//���� ������� ���������� -1, �� ������� ����� ��������
int ReadCmd( ServerData* server, int& cmd, uint& id, const int cmds[][2], Mem::Data& data, int wait );
//����� � ����� ������, time ����� � �������� �� ������� ����, ���� 0, �� ���� �����
void RunHibernation( ServerData* server, int time );
//���� ���� ���������� ����� id, �. �. ���� �� �� �������� �� ��������� �������, wait - ����� ��������
//� ������������
//����������: true - ����� ����������, false - ����� �� ���������� (����������� ����� ��������)
bool WaitOutPackage( ServerData* server, DWORD id, int wait = 24 * 3600 * 1000 ); 

//��������� ������� ������, �. �. ������� ����� ���������� � ����������� �������� �������
//���������� ��� ������ ��������� �������� ����������� �������, �. �. ��� ������ ������ �� ���� 
//����� �������� �������� ��������� ���������� ��� ����������. ��� �������� �������� ������ �������
//���� ��������� ������ ����������� ������� (������ ��������� � ���� �����)
//���������� ��������� ����� ������, ���� 0, �� ������
ServerData* DuplicationServer( ServerData* server, int id );

//���������� ������, ��� id - ��� ���������� � ������, subId - ���������� ��� ������
DWORD SendStr( ServerData* server, int id, int subId, const char* s, int c_s );

}
