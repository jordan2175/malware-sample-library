#pragma once

#include "core.h"
#include "misc.h"

struct AddressIpPort
{
	char ip[128]; //��� ����� ���� ��� ���� ����� ��� � ��� ������
	int port;

	bool Parse( const char* s )
	{
		return ExtractIpPort( s, ip, port );
	}
};

namespace Proxy
{

enum Type
{
	HTTP, HTTPS, SOCKS5 //� ������� ��������
};

struct Info
{
	Type type;
	AddressIpPort ipPort;
	char authentication[128]; //������ �������������� ��� HTTP ���������
};

//������� ���������� � ��������� ������, � ���� �� �������
//�� ����������� � ���� �� ������, ��� ���� ����� ��������� ������ ������������
//��� ������������� � ��������� ���
class Connector
{
		static const int MAX_PROXY = 10;

	public:
		Info addr[MAX_PROXY];
		int c_addr;
	
	private:
		int curr; //����� ������ ����� ������� � ��������� ��� ��� �������
		CRITICAL_SECTION lock;

	public:

		Connector();
		~Connector();
		void Add( const Info* _addr, int count );
		void Del( const Info* _addr, int count );
		//����������� � ��������� ������ ��� � ������, � authentication ����� ������ �������������� ��� ������, ���� ����� ����
		int Connect( const char* host, int port, bool& proxy, StringBuilder& authentication );
};

}
