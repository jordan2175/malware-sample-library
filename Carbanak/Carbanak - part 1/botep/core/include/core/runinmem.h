#pragma once

//������ ���������� ��� dll � ������

#include "core.h"

namespace RunInMem
{

//������ exe ����� � ������
bool RunExe( const void* module, int szModule );
//������ dll � ������, ���������� ����� ���� ��� ��� ��������� ������� �������
HMODULE RunDll( const void* module, int szModule, void* thirdParam = 0 );
void FreeDll( HMODULE module );

}
