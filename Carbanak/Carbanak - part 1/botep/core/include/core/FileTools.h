#pragma once

#include "File.h"

namespace File
{

struct PatchData
{
	byte* find; //������������������ ������� ����� �����
	int c_find; //���������� ���� � find
	byte* replace; //��� ����� ��������
	int c_replace; //���������� ���� � replcace
	int offset; //�������� �� ������� � ������� ������ find, � ����� �������� � ����������� replace
};

//���� ���������� true, �� ���� ����������, ���� false, �� ������ � PatchExe() ������ -2
typedef bool (*typeCallbackAfterPatch)( Mem::Data& data );

//������ ��������� ����, ���������� ���������� ��������� ������ �� patch
//����� ������� ������������ �� patch->find = 0
//���� ���������� 0, �� ������ �� �������, -1 - ���� ������ ����� ��� ��� ��� ������ ��������, -2 - cb ������� false
int Patch( const char* fileNameFrom, const char* fileNameTo, PatchData* patch, typeCallbackAfterPatch cb = 0 );
//������ exe ��� dll ���� ��� ���� ������������� ����� ����������� ����� � PE ���������
int PatchExe( const char* fileNameFrom, const char* fileNameTo, PatchData* patch );

struct MonitoringStru
{
	const char* fileName; //���� ������� �����������
	StringBuilder* fullName; //������ ��� �����, ��������������� �������� Monitoring()
	DWORD tag; //�����-�� �������������� ����������
	DWORD action; //FILE_NOTIFY_INFORMATION.Action
};

//���� ���������� false, �� ���������� ���������������
typedef bool (*typeCallbackMonitoring)( MonitoringStru* );

//���������� 0 - ������� cb ���������� false (��������� �����������), <0 - �����-�� ������
int Monitoring( const char* path, MonitoringStru* files, int c_files, typeCallbackMonitoring cb, int wait = INFINITE );

}
