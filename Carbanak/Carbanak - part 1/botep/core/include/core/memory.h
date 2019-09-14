#pragma once

//������ � �������

#include "misc.h"
#include "type.h"

class StringBuilder;
typedef MovedPtr<byte> AutoMem; //������ � � ������������� ������������� ������

namespace Mem
{

bool Init();
void Release();

void* Alloc( int size );
void Free( void* ptr );
void* Realloc( void* ptr, int newSize );

void* Copy( void* dst, const void* src, int size );
void* Duplication( const void* src, int size );
void* Set( void* ptr, int c, int count );
//����� � ������ ��������� ������������������, ���������� ������� ��� ������� ��� -1, ���� ������ ���������
int IndexOf( const void* ptr, int c_ptr, const void* what, int c_what );
int IndexOf( const void* ptr, char c, int c_ptr );
//����� ���������� �������, ���������� ��������� �� ����, ��� 0 ���� �� ������
void* Find( const void* ptr, char c, int c_ptr );
int Cmp( const void* mem1, const void* mem2, size_t n ); 

//������� ��������
template<class T> void Zero( T& ptr )
{
	Set( &ptr, 0, sizeof(T) );
}

//���������� ������ � �������
class Data
{
		void* data; //���������� ������
		int c_data, sz_data; //����� ���������� ������ � ������ ���������� ������

		Data( const Data& );

	public:

		Data( int sz = 0 );
		~Data();
		bool Realloc( int sz );
		//��������� ���������� �� ������ ��� ���������� sz ������, ���� ���, �� ������������ ����� �������
		bool MakeEnough( int sz );
		void* Ptr() const
		{
			return data;
		}
		byte* p_byte() const
		{
			return (byte*)data;
		}
		char* p_char() const
		{
			return (char*)data;
		}
		int Len() const
		{
			return c_data;
		}
		int SetLen( int sz );
		int AddLen( int add );

		int Size() const
		{
			return sz_data;
		}
		bool Append( const void* ptr, int c_ptr ); //��������� ����� ������, ���� ���������� ������ �� ����� �������, �� ������������� ������� �����
		bool AppendStr( const char* s, int c_s = - 1 ); //��������� ������, ���� s = 0, �� ��������� 0 (������ ������)
		bool IsEnough( int sz ) //�������� �� ������ ��� ������ sz ����
		{
			if( sz < sz_data - c_data )
				return true;
			return false;
		}
		void* PtrWrite() //������ ��������� ������  ���� ����� ������ ������
		{
			return (byte*)data + c_data;
		}
		void Clear()
		{
			c_data = 0;
		}
		//���������� ���������� ������ �������� ������
		void SetFull( char c )
		{
			Mem::Set( data, c, sz_data );
		}
		int IndexOf( int start, const void* what, int c_what );
		int IndexOf( int start, char c );
		int IndexOf( const void* what, int c_what )
		{
			return Mem::IndexOf( data, c_data, what, c_what );
		}
		//�������� ������ ���� � ������ ������ ������� � index � ������ len, �. �. ���� � 0 �� index - 1 ���������
		void Submem( int index, int len = INT_MAX );
		//�������� ���� ������� � index � mem
		void Submem( Data& mem, int index, int len = INT_MAX );
		//�������� � ��������� ������� ������ ptr ������ c_ptr, ���������� true ���� ������� ��������
		bool Insert( int index, const void* ptr, int c_ptr );
		bool Insert( int index, const Data& data )
		{
			return Insert( index, data.Ptr(), data.Len() );
		}
		//������� �� ������� ����� ������� � ������� i � ������ len
		void Remove( int index, int len );
		void Left( int n ) //��������� ��������� n ����
		{
			Submem( 0, n );
		}
		void Right( int n ) //��������� �������� n ����
		{
			Submem( c_data - n, n );
		}
		void DelLeft( int n ) //������� ��������� n ����
		{
			return Right( c_data - n );
		}
		void DelRight( int n ) //������� �������� n ����
		{
			return Left( c_data - n );
		}
		//�������� ���� ������, ���������� ������� ���� �����������
		int Copy( const void* src, int c_src ); //��� ����� ����������� ������ ����� ����� c_src
		int Copy( const Data& src )
		{
			return Copy( src.data, src.c_data );
		}
		int Copy( int dst_start, int src_start, int count, const void* src, int c_src ); //��� ����� ����������� ������ ��������� ������� ���� ���������� ���� dst_start + c_src ����� ������
		int Copy( int start, const void* src, int c_src )
		{
			return Copy( start, 0, c_src, src, c_src );
		}
		int Copy( int dst_start, int src_start, int count, const Data& src )
		{
			return Copy( dst_start, src_start, count, src.data, src.c_data );
		}
		int Copy( int dst_start, int src_start, const Data& src )
		{
			return Copy( dst_start, src_start, src.c_data - src_start, src );
		}
		StringBuilder& ToString( StringBuilder& s );
		//���������� �� ����� ������� ���������� ������, ����� � ����������� ��� �� ���� �������. ���������� ��������� �� ���������� ������
		void* Unlink();
		//���������� ������ �� ������ Data, ����������� ����� ��� ����� ������� Unlink, ����� ����� ������� �������� ������� data
		//������� Data ������ ���� ������ (����� ����� ������������ Data() ��� ����� Unlink), ����� �������� ������ ������
		//����� �� � ���� ������ �� ������ ���� ������������� ������.
		//Link ����� ������������ ������ ���� ������������� ������ �� ����������
		void Link( Data& src );
		void Link( void* _data, int _c_data );
		//�������� oldv �� newv, ���������� ���������� �����
		int Replace( void* oldv, int c_oldv, void* newv, int c_newv );
};

}

//�������������� ����������� �������� C++ ��� ��������� � �������� ������
inline void* operator new( size_t size )
{
	return Mem::Alloc(size);
}

inline void* operator new[]( size_t size)
{
	return Mem::Alloc(size);
}

inline void operator delete( void* ptr )
{
	Mem::Free(ptr);
}

inline void operator delete[]( void* ptr )
{
	Mem::Free(ptr);
}
