#pragma once

#include "core.h"
#include "TlHelp32.h"

namespace Process
{

#define MAIN_USER ((char*)0xffffffff)

//���� ����� � options ��� ������� Exec, �� ��������� ������� � ������� �������� �����
//const DWORD EXEC_USER = 0x8000000;
//��� �������� �������� �� ���������� ����
const DWORD EXEC_NOWINDOW = 0x40000000;

//���������� � �������� ��� ������������ ���������
struct ProcessInfo
{
	StringBuilderStack<MAX_PATH> fullPath; //������ ���� � �������� � ������ ��������
	StringBuilderStack<64> fileName; //��� ����� �������� � ������ ��������
	DWORD pid; 
	DWORD parentPid;
	uint hash; //��� ����� ����� ��������
};

//���������� �������� ListProcess, ���� ���������� true, �� ������������� ���������� ������������
//tag - ������ ������������ �������� ����������� ListProcess()
typedef bool (*typeListProcessCallback)( ProcessInfo&, void* tag );

//������ ��������, options - ��� ����� ��� ������� CreateProcess, cmd - ����������� ������� � �����������
//���������� pid �������� ��� 0, ���� ��������� �� �������
//����� ����� ��������� ���������� ����� ��������, ����� ������ hthread, ��� ������� ������ ������� exitCode, wait - ������� � ������������� �����
//���������� ��������, ���� 0, �� ����� �� �����
//nameUser - ��� ����� ������ ����� ���������, ���� 0, �� ��� ��� ��� ����� ������� ���, ���� = DefUser, �� ��������� ��� ������ � �������� ������� ���������
DWORD Exec( DWORD options, const char* nameUser, HANDLE* hprocess, HANDLE* hthread, DWORD* exitCode, int wait, const char* cmd, ... );
//������ �������� � ���������
DWORD Exec( DWORD* exitCode, int wait, const char* cmd, ... );
DWORD Exec( DWORD options, DWORD* exitCode, int wait, const char* cmd, ... );
//������ �������� ��� ��������
DWORD Exec( DWORD options, const char* nameUser, const char* cmd, ... );
DWORD Exec( const char* cmd, ... );
//PID �������� ��������
DWORD CurrentPID();
//��� ����� �������� �������� (��� � ������ ��������)
uint CurrentHash();
//��� �������� ��������
StringBuilder& Name( StringBuilder& name );
//��� �������� �� ��� pid, ���� full = true, �� ���������� ������ ����
StringBuilder& Name( DWORD pid, StringBuilder& name, bool full = false );
//���������� PID ����������� ��������, ���� ��� �������� ������ (������ ��� �����), �� full ������ ���� ����� true
DWORD GetPID( const char* nameProcess, bool full = false );
//���������� PID �������� �������� ����������� ����
DWORD GetPID( HWND hwnd );
//���������� PID �������� �� ������ ��� ������
DWORD GetPID( HANDLE hthread );
//������������ ���� ���������� ���������, ���������� ���������� ������������� ���������
int ListProcess( typeListProcessCallback func, void* tag, DWORD typeSnap = TH32CS_SNAPPROCESS );
//���������� PID �������� explorer
DWORD GetExplorerPID();
//��������� ������� �� ��� pid
bool Kill( DWORD pid, int wait );
//��������� ������� �� ��� �����
bool Kill( const char* nameProcess, int wait );
//������ �������� �� ���������� �������� ��� ������� �������������
bool ExecAsCurrUser( const char* cmd );
//�������� ��������� �������� ���� hwnd ������� ����������� ������� ���������
bool SendMsg( HWND hwnd, DWORD msg, WPARAM wparam, LPARAM lparam );
bool SendMsg( HWND hwnd, MSG* msgs, int c_msgs );
//������� �������� ������� ������ �������� ��������� ����
void KillBlocking( const char* fileName );
//������� �������� ������� ��������� ��������� dll
void KillLoadedModule( const char* nameModule );
//���������� true ���� ������� ��� ��������
bool IsAlive( HANDLE hprocess );
//���������� ��������� ������ � ����������� ���������� ��������
bool GetCommandLine( uint pid, StringBuilder& s );

};

