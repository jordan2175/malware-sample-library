#pragma once

#include "core.h"

class HttpProxy
{
		int port; //���� ������� �������
		//�������� ���� ������
		void Loop();

	public:

		HttpProxy( int _port );
		~HttpProxy();

		bool Start();
		bool StartAsync();
};
