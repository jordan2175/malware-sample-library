#pragma once

#include "core.h"
#include "shlobj.h"

namespace Path
{

const char DirectorySeparatorChar = '\\';

bool GetSystemDirectore( StringBuilder& path );
bool GetStartupExe( StringBuilder& path ); //��� ����������� �����
//���������� ���� �� �� CSIDL (������� ��� ������� SHGetFolderPath)
//� path ������ ���� ���������� ������, �� ������ MAX_PATH
bool GetCSIDLPath( int csidl, StringBuilder& path );
//� ����� csidl ��������� add
bool GetCSIDLPath( int csidl, StringBuilder& path, const char* add );
//���������� ��� ��������� �����, ���� ������ add, �� ���� ���� ����������� 
bool GetTempPath( StringBuilder& path, const char* add = 0 );
//����� �������� ������������ 
inline bool GetStartupUser( StringBuilder& path ) 
{
	return GetCSIDLPath( CSIDL_STARTUP, path );
}
//���������� ��� ����� � ����������� (���������� path ����� ��������)
StringBuilder& GetFileName( StringBuilder& path );
//���������� ��������� � �������� ���������� ��� �����
char* GetFileName( const char* path );
//���������� ��������� � �������� ���������� ���������� (��������� �� �����)
char* GetFileExt( const char* path );
//�������� � fileName ����������
StringBuilder& ChangeExt( StringBuilder& fileName, const char* newExt );
//���������� ���� (������� ��� �����)
StringBuilder& GetPathName( StringBuilder& path );

StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2 );
StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2, const StringBuilder& path3 );
StringBuilder& Combine( StringBuilder& res, const StringBuilder& path1, const StringBuilder& path2, const StringBuilder& path3, const StringBuilder& path4 );

StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2 );
StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2, const char* path3 );
StringBuilder& Combine( StringBuilder& res, const char* path1, const char* path2, const char* path3, const char* path4 );

//��������� � ���� ����
StringBuilder& AppendFile( StringBuilder& path, const char* file, int c_file = -1 );
inline StringBuilder& AppendFile( StringBuilder& path, const StringBuilder& fileName )
{
	return AppendFile( path, fileName.c_str(), fileName.Len() );
}
//������� ��������� ����� ���� ���� ��� �����-�� �������������
bool CreateDirectory( const char* path );
//������� ����� �� ����� ����������
bool DeleteDirectory( const char* path );
//���� � ����� path ��������� ����������� \ ��� /, �� �� ���������
void RemoveEndSeparator( StringBuilder& path );

}
