#pragma once

#include "manager.h"
#include "stream.h"

namespace WndRec
{

bool SendFile( ServerData* server, const char* typeName, const char* fileName, const char* ext, const void* data, int c_data );
//���������� ������ �� ������, ����� �� ������ ��������� ������, ���������� �� �� �������� ����� CmdExec ����� ���������� ���������� ������
uint LoadPluginAsync( ServerData* server, const char* namePlugin);

}
