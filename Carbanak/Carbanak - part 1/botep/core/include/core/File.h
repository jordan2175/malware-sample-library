#pragma once

#include "core.h"

namespace File
{

//��������� � ����� ��� ������������ ������ � �����
struct FileInfo
{
	WIN32_FIND_DATAA fd;
	const StringBuilder* path; //����� � ������� ��������� ����
	StringBuilder* fullName; //������ ��� �����
};

//���������� �������� ListFiles, ���� ���������� false, �� ������������� ���������� ������������
//tag - ������ ������������ �������� ����������� ListFiles()
typedef bool (*typeListFilesCallback)( FileInfo&, void* tag );

//��������� ����, ������������ ���� ����� �������� � ������� �����
HANDLE Open( const char* fileName, DWORD access, DWORD creation, DWORD flags = 0 );
bool Write( HANDLE file, const void* data, int c_data );
uint Read( HANDLE file, void* data, int c_data );
//������ ����� ����� 
bool ReadAll( const char* fileName, Mem::Data& data );
//������ ������ � ����
bool WriteAll( const char* fileName, const void* data, int c_data );
inline bool Write( const char* fileName, const StringBuilder& s )
{
	return WriteAll( fileName, s.c_str(), s.Len() );
}
inline bool Write( const char* fileName, const Mem::Data& data )
{
	return WriteAll( fileName, data.Ptr(), data.Len() );
}
//������ ������ � ����� �����
bool Append( const char* fileName, const void* data, int c_data );
inline bool Append( const char* fileName, const Mem::Data& data )
{
	return Append( fileName, data.Ptr(), data.Len() );
}
inline bool Append( const char* fileName, const StringBuilder& s )
{
	return Append( fileName, s.c_str(), s.Len() );
}
bool Append( const char* fileName, const char* s, int c_s = - 1 );

//���������� ������ �����, ���� ���������� -1, �� ������
int Size( const char* fileName );
//��������� ������� �����
bool IsExists( const char* fileName );
bool Delete( const char* fileName );
void Close( HANDLE file );
//�������� ���� �� src � dst
bool Copy( const char* src, const char* dst );
//������������� ��������� �����
bool SetAttributes( const char* fileName, DWORD attr );
//���������� ��� ���������� �����, ������ fileName ������ ���� �� ������ MAX_PATH
bool GetTempFile( char* fileName, const char* prefix = 0 );
//����������� ��� ����� � ����� path �� ����� mask
//���� subdirs = true, �� � ������������� ����� � ���������
//���������� 0, ���� ��� ����� �����������, 1 - ���� ����� ��� ���������� callback ��������, 2 - ���� ��� ������� � �����
int ListFiles( const StringBuilder& path, const char* mask, typeListFilesCallback func, int subdirs, void* tag = 0 );
//���������� � ���������� ����� ���� 0xff � ����� ������� ����
bool DeleteHard( const char* fileName );

}

class MemFile
{
	public:

		Mem::Data data;
		int pos; //������� � �����

	private:
	
		bool linked;

	public:

		MemFile( int size = 0 );
		MemFile( const void* ptr, int c_ptr );
		MemFile( const char* fileName );
		MemFile( Mem::Data& _data );
		~MemFile();

		int Read( void* mem, int sz );
		int Write( const void* mem, int sz );
		int Seek( int p, int moveMethod );
		void Clear();
};
