#pragma once

#include "core.h"
#include "misc.h"

//�������� ������� �� �������
//���������� ������ ����� ����

class ThroughTunnel
{
		int portIn; //���� �����������

	protected:

		char ipOut[16]; //����� ���� ������������
		int portOut; //���� ���� ������������

		//���������� ����� �������� �������� ���������� � �� ���� ��� ����� ������� ���������� � ipPort
		//���������� ��������� �����, ���� 0, �� �������������� ���������� � ipPort
		virtual int Connected( int sc );

	private:

		void Loop();

	public:

		ThroughTunnel( int _portIn, const char* _ipOut, int _portOut );
		~ThroughTunnel();

		bool Start();
		bool StartAsync();

};

