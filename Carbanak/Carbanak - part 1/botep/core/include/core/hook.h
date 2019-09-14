#pragma once

#include "core.h"

namespace Hook
{

const int MaxHooks = 128;
const int MaxHooksEx = 128; //������������ ���������� �������� ������� (�����������)
const int MaxJoinHooks = 8; //������������ ���������� ����������� � ������ ����

//��� �����, ������ ���� �� ������ MaxHooks
const int ID_ZwResumeThread = 0;
const int ID_DispatchMessageA = 1;
const int ID_DispatchMessageW = 2;
const int ID_GetMessageA = 3;
const int ID_GetMessageW = 4;
const int ID_PeekMessageA = 5;
const int ID_PeekMessageW = 6;
const int ID_DestroyWindow = 7;

typedef NTSTATUS ( WINAPI *typeZwResumeThread )( HANDLE hThread, PULONG PreviousSuspendCount );
struct ParamsZwResumeThread
{
	HANDLE hThread;
	PULONG PreviousSuspendCount;
	NTSTATUS _ret; //������������ ��������
	void* tag;
};

typedef LRESULT ( WINAPI *typeDispatchMessageA )( const MSG* lpmsg );
typedef LRESULT ( WINAPI *typeDispatchMessageW )( const MSG* lpmsg );
struct ParamsDispatchMessage
{
	MSG* lpmsg;
	bool _ansi; //true - ���� DispatchMessageA, ����� DispatchMessageW
	LRESULT _ret;
	void* tag;
};

typedef BOOL ( WINAPI *typeGetMessageA )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax );
typedef BOOL ( WINAPI *typeGetMessageW )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax );
struct ParamsGetMessage
{
	LPMSG lpMsg;
	HWND hWnd;
	UINT wMsgFilterMin;
	UINT wMsgFilterMax;
	bool _ansi; //true - ���� GetMessageA, ����� GetMessageW
	BOOL _ret;
	void* tag;
};

typedef BOOL ( WINAPI *typePeekMessageA )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg );
typedef BOOL ( WINAPI *typePeekMessageW )( LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg );
struct ParamsPeekMessage
{
	LPMSG lpMsg;
	HWND hWnd;
	UINT wMsgFilterMin;
	UINT wMsgFilterMax;
	UINT wRemoveMsg;
	bool _ansi; //true - ���� GetMessageA, ����� GetMessageW
	BOOL _ret;
	void* tag;
};

typedef BOOL (WINAPI *typeDestroyWindow )( HWND hWnd );
struct ParamsDestroyWindow
{
	HWND hWnd;
	BOOL _ret;
	void* tag;
};

typedef BOOL (WINAPI *typeWriteProcessMemory)( HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T *lpNumberOfBytesWritten );

//���� ��� ������� ����� ���������� � ����
//���� ��� ���������� true, ���������� ������� ������� �� ����� ����������
typedef bool (*typeStruZwResumeThread)( ParamsZwResumeThread& ); 
typedef bool (*typeStruDispatchMessage)( ParamsDispatchMessage& );
typedef bool (*typeStruGetMessage)( ParamsGetMessage& );
typedef bool (*typeStruPeekMessage)( ParamsPeekMessage& );
typedef bool (*typeStruDestroyWindow)( ParamsDestroyWindow& );

struct HookInfo
{
	bool init; //true ���� ��� ��������� (��� ����������)
	ADDR real; //����� ������� �� ������� ��������� ���
	ADDR proxy; //����� ������� ����� �������� ����� ��������� ���� ��� ������ �������� ������� (��������� �� code)
	byte code[16]; //��������� ����� ���� �������� �������, ������ ���� ����� ����������� � c_code
	int c_code; //������ ������������ ���� �� ������ �������� �������, ������������ ��� ������ ����. ��� ������ �� �������� ������ �������� �� �������� ����� �������
};

struct HookInfoEx : public HookInfo
{
	ADDR joinBefore[MaxJoinHooks]; //������������ � ���� ������� ������� ���������� �� ������ ������������
	void* tagBefore[MaxJoinHooks]; //�������������� �������� ��� �������������� �������
	ADDR joinAfter[MaxJoinHooks]; //������������ � ���� ������� ������� ���������� ����� ������ ������������
	void* tagAfter[MaxJoinHooks]; //�������������� �������� ��� �������������� �������
};

bool Init();
void Release();

bool GetAsmLen( void* code, int* len );

//��������� ������������ ���� �� WinApi ������� realFunc, �� ������� idFunc, ��������� ����� hookFunc
//�� ��� ������� ������������� ���������� ������� �������
bool Set( int idFunc, void* realFunc, void* hookFunc );
//������������� ������� ��� �� �������� realFunc, ����� ���������� hookFunc,
//���������� ����� ��� ������ �������� �������, ���� 0, �� ��� �� �����������
void* Set( void* realFunc, void* hookFunc );
//������������� ��� �� ������� �� ��������� ���
void* Set( HMODULE dll, uint hashFunc, void* hookFunc );
//���������� ����� ���� ��� ������� realFunc
//���� ���������� 0, �� �� ��������� ������� �� ���������� ���
void* GetHook( void* realFunc );
//��������� ������ ���������� ��� �������� �������, ������ ���� before = true, �� ����� ����� ��
//�������� ������� ��� ���� before = false, ����� ������ �������� �������
bool Join( int idFunc, void* hookFunc, void* tag, bool before = false );
//���������� ����� �������������� �������
ADDR GetJoinFunc( int idFunc, int& n, void*& tag, bool before );
//���������� ����� �������� �������
ADDR GetRealFunc( int idFunc );
//���������� ����� �� �������� ����� ������� �������� �������, ����� ��� ������� ����� �������� ��������
ADDR GetProxyFunc( int idFunc );
//������������ ��������� ������� � ����, ������������ ���� ���������� ���������
bool Join_Func( int idFunc, void* realFunc, void* hookFunc, void* myFunc, void* tag = 0, bool before = false );
//������� ����������� � ��������� �����
bool Join_ZwResumeThread( typeStruZwResumeThread myFunc, void* tag = 0, bool before = false );
bool Join_DispatchMessage( typeStruDispatchMessage myFunc, void* tag = 0, bool before = false );
bool Join_GetMessage( typeStruGetMessage myFunc, void* tag = 0, bool before = false );
bool Join_PeekMessage( typeStruPeekMessage myFunc, void* tag = 0, bool before = false );
bool Join_DestroyWindow( typeStruDestroyWindow myFunc, void* tag = 0, bool before = false );

}
