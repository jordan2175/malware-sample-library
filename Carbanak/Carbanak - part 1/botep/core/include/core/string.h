#pragma once

#include <stdarg.h>
#include "memory.h"
#include "type.h"
#include "misc.h"
#include "vector.h"

#define RN(x) x[0] = '\r', x[1] = '\n', x[2] = 0

namespace Str
{

bool Init();
void Release();

int Len( const char* s );
//����������� ������ ����� ������, ��� � dst ���������� �����
char* Copy( char* dst, const char* src, int len = -1 );
//����������� ������ � ��������� ������� ������������ ���������� ������, ���� �� �������, �� ������ ����������
char* Copy( char* dst, int sz_dst, const char* src, int len = -1 );
char* Duplication( const char* s, int len = -1 );
//�������� ������ ��� ������ ��������� ����� � ������ ������ ������� �����
char* Alloc( int len );
inline void Free( char* s ) //����������� ������ ������
{
	Mem::Free(s);
}
//��������� ��������� ����� � ����, ��������� ���������� ������ ���� 0
//������� ��������� �������� �� ����������� 16 �����
char* Join( const char* s, ... );
//�������� ��������� maxSize ������ � ��������� ������� wsprintf ������� ���� ��������������� �����
char* Format( int maxSize, const char* format, ... );
//������� ��������������� ����� � buf, ��������� ����� ���������� ������
int Format( char* buf, const char* format, ... );
int FormatVA( char* buf, const char* format, va_list va );
//��������� ��� ������
uint Hash( const char* buf, int c_buf = - 1 );
char* Upper( char* s ); //��� ������� ���� � ��������� 1251
char Upper( char c );
char* Lower( char* s ); 
char Lower( char c );
//���������� ������, max - ���������� ������������ ��������
int Cmp( const char* s1, const char* s2, int max = INT_MAX );
//����� � ������ s ��������� sub, ���������� ������ ��������� ���������,
//���� ���������� -1, �� ��������� �� �������
int IndexOf( const char* s, const char* sub, int c_s = -1, int c_sub = -1 );
//���� � ������ ��������� ������, ���������� ��� ������� � ������, ���� -1, �� ������ �� ������
int IndexOf( const char* s, char c );
//������ �������� �� olds �� �������������� �� �������� ������� �� news, ���� � news �� ����� ������� ��������, �� ����� ������� ���������
//���������� ����� ������, ���� news == 0, �� ������� �� olds ���������, �. �. ������ ������ ������
int ReplaceChars( char* s, const char* olds, const char* news );
//����������� ������ � �����, ���� hex = true, �� ��������� ������ � 16-������ ���� (16-������ ������ ����� ���� � ������� 0xabd ��� abd)
//������� � ������ ������ ������������
int ToInt( const char* s, bool hex = false );
//������������ ����� �� 0 �� 15 � 16-������ �����
char DecToHex( int v );
//��������� �������� ����� � �����, ���������� ����� ���������� ������
int ToString( int v, char* s, int radix = 10 );
//��������� ���������� ����� � �����, ���������� ����� ���������� ������
int ToString( uint v, char* s, int radix = 10 );
//������� ��������� ������� � ������ ������, ���������� ���������� �������� ��������
int LTrim( char* s, char c = ' ' );
//������� ��������� ������� � ����� ������, ���������� ���������� �������� ��������
int RTrim( char* s, int c = ' ', int c_s = -1 );
//������� ��������� ������� � ������ � ����� ������, ���������� ���������� �������� ��������
int Trim( char* s, int c = ' ', int c_s = -1 );
//������������� ������ char �  wchar_t
//���� c_src = -1, �� ������ src ��������� ��������
//���� to = 0 � size = 0, �� ���������� ����������� ���������� ������ � �������� ��� to
//������� � ����� to �� ������ ��� 0, ������� ���� ����������� c_src, �� ��������� � ������
//������������ ����, � size ���� ��������� � ������ ������������ ����
int ToWideChar( const char* src, int c_src, wchar_t* to, int size );
//������������� ������ char �  wchar_t
//���� c_src = -1, �� ������ src ��������� ��������
//���������� ���������������� ������, ������� ����� ����� �������
wchar_t* ToWideChar( const char* src, int c_src = -1 );
//���������� � ������ s ��������� ������, �. �. ������� � ������� p ���� ������ ������ ������� �� ����� c
int Ignore( const char* s, int p, char c );

}

namespace WStr
{

int Len( const wchar_t* s );
//������������� �� wchar � char, size - ������ ������ � to (������� ����� ��� ������������ ����)
//c_src - ����� src � ��������, ���� -1, �� ������� ������� ����
//���� to = 0 � size = 0, �� ��������� ����������� ���������� ������ ��� �������� ������
//���������� ���������� ���������� ���� � to (������� ����������� 0)
int ToWin1251( const wchar_t* src, int c_src, char* to, int size );
wchar_t* Alloc( int len );
inline void Free( wchar_t* s )
{
	Mem::Free(s);
}
//���� � ������ ��������� ������, ���������� ��� ������� � ������, ���� -1, �� ������ �� ������
int IndexOf( const wchar_t* s, wchar_t c );
wchar_t* Chr( const wchar_t* s, wchar_t c );

}

class StringBuilder;

//������ �����
typedef Vector< MovedPtr<StringBuilder> > StringArray;

class StringBuilder
{
	protected:

		char* ptr; //���������� ������ ��� ������
		int len; //����� ������ (��� ������������ ����)
		int size; //������ ���������� ������
		bool alloc; //true - ���� � ptr ���������� ���� ������� ������, ����� �������� � �����������, ���� false, �� ������ ���� �������� �� ��� � ������� ��������� �� �����

		StringBuilder( const StringBuilder& );

	public:

		//������� ������ ������ � �������� ��������� ���������� ������, �������� ��������� ������
		explicit StringBuilder( int _size = 128, const char* s = 0, int len_s = -1 ); 
		//������� ������ �������� � ���������� ������, � ����������� ��� ������ �� ����� ��������
		//���� _ptr = 0, �� ����� ������� s, �. �. ��������� ������ ��� ����������� ������, 
		//� ���� _size < 0, �� ��� ����� ����� ����� ������ + 1 (����������� ����)
		explicit StringBuilder( char* _ptr, int _size, const char* s = 0, int len_s = -1 );
		//������� ������ �� ������ ����������� ����� ������, ������ ������ � start, ���� len = -1, �� ����� ������ ����� ����� ����� ����������� ������
		//� ����� ������ ����������� ������ ���� ����� ��� ������������ ����, ��� ��� ����������� ��� ����� �������������.
		//���������� ���������� ������ ����� ����� ���������� ���������� ������ � data. 
		//��������! data �� ������ ������������ ������ ��� ������ ������
		StringBuilder( Mem::Data& data, int start = 0, int _len = -1 );
		~StringBuilder();

		//����������� ������ ���������� ������ ��� ������, ���� �����
		bool Grow( int addSize );
		int Len() const
		{
			return len;
		}

		int Size() const
		{
			return size;
		}

		char* c_str() const
		{
			return ptr;
		}

		operator char*() const
		{
			return c_str();
		}

		char& operator[]( int index )
		{
			return ptr[index];
		}

		StringBuilder& Cat( const char* s, int len_s = -1 );
		StringBuilder& Cat( char c );
		StringBuilder& Cat( int v, int radix = 10 );
		StringBuilder& Cat( uint v, int radix = 10 );
		StringBuilder& Copy( const char* s, int len_s = -1 );

		StringBuilder& operator+=( const char* s )
		{
			return Cat(s);
		}
		StringBuilder& operator+=( const StringBuilder& sb )
		{
			return Cat( sb.ptr, sb.len );
		}
		StringBuilder& operator+=( char c )
		{
			return Cat(c);
		}
		StringBuilder& operator+=( int v )
		{
			return Cat(v);
		}
		StringBuilder& operator+=( uint v )
		{
			return Cat(v);
		}

		StringBuilder& operator=( const char* s )
		{
			return Copy(s);
		}
		StringBuilder& operator=( const StringBuilder& sb )
		{
			return Copy( sb.ptr, sb. len );
		}

		//��������� ����� ������, ������������ ����� ������������� WINAPI �������
		int UpdateLen();
		//������������� ����� ����� ������ � ���������� ������
		int SetLen( int newLen );
		//������ ������ ������
		void SetEmpty()
		{
			SetLen(0);
		}
		StringBuilder& Upper()
		{
			Str::Upper(ptr);
			return *this;
		}
		StringBuilder& Lower()
		{
			Str::Lower(ptr);
			return *this;
		}
		uint Hash() const
		{
			return Str::Hash( ptr, len );
		}

		StringBuilder& Format( const char* format, ... )
		{
			va_list va;
			va_start( va, format );
			len = Str::FormatVA( ptr, format, va );
			return *this;
		}
		int Cmp( const char* s, int max = INT_MAX ) const
		{
			return Str::Cmp( ptr, s, max );
		}
		bool operator==( const char* s ) const
		{
			return Cmp(s) == 0;
		}
		bool operator==( const StringBuilder& s ) const
		{
			return Cmp( s.c_str() ) == 0;
		}
		bool operator!=( const char* s ) const
		{
			return Cmp(s) != 0;
		}
		bool operator!=( const StringBuilder& s ) const
		{
			return Cmp( s.c_str() ) != 0;
		}
		int IndexOf( int start, const char* sub, int c_sub = -1 ) const;
		int IndexOf( int start, const StringBuilder& sub ) const
		{
			return IndexOf( start, sub.ptr, sub.len );
		}
		int IndexOf( const char* sub, int c_sub = -1 ) const
		{
			return IndexOf( 0, sub, c_sub );
		}
		int IndexOf( const StringBuilder& sub ) const
		{
			return IndexOf( 0, sub );
		}
		int IndexOf( int start, char c ) const;
		int IndexOf( char c ) const
		{
			return IndexOf( 0, c );
		}

		//�������� ��������� ������� � ������� index � ������ len
		StringBuilder& Substring( int index, int _len = INT_MAX );
		//�������� ��������� ������� � ������� index � ������ len � ����� � s
		//���������� s (���������� ������) 
		StringBuilder& Substring( StringBuilder& s, int index, int _len = INT_MAX ) const;
		//��������� ��������� ������� ������, ���� ������ ������ ����� ������, �� ������� �� ����� ����������
		StringBuilder& Insert( int index, const char* s, int c_s = -1 );
		StringBuilder& Insert( int index, const StringBuilder& s )
		{
			return Insert( index, s.c_str(), s.Len() );
		}
		StringBuilder& Insert( int index, char c );
		//�������� � ������ � ������� index � sz �������� �� ������ s
		StringBuilder& Replace( int index, int sz, const char* s, int c_s = -1 );
		StringBuilder& Replace( int index, int sz, const StringBuilder& s )
		{
			return Replace( index, sz, s.c_str(), s.Len() );
		}
		//�������� ������ oldStr �� newStr ������� � ������� start � n ���
		//���� �� ��������� newStr, �. �. ���������� 0, �� ����� ��������� ������ oldStr, �� ������ ������ 0, ����� ������� nullptr, ��� ��� ��-��
		//������������� ������� ����� ��������� ����
		StringBuilder& Replace( int start, int n, const char* oldStr, const char* newStr, int c_oldStr = -1, int c_newStr = -1 );
		StringBuilder& Replace( int start, const char* oldStr, const char* newStr, int c_oldStr = -1, int c_newStr = -1 )
		{
			return Replace( start, INT_MAX, oldStr, newStr, c_oldStr, c_newStr );
		}
		StringBuilder& Replace( const char* oldStr, const char* newStr, int c_oldStr = -1, int c_newStr = -1 )
		{
			return Replace( 0, INT_MAX, oldStr, newStr, c_oldStr, c_newStr );
		}
		StringBuilder& Replace( int start, int n, const StringBuilder& oldStr, const StringBuilder& newStr )
		{
			return Replace( start, n, oldStr.c_str(), newStr.c_str(), oldStr.Len(), newStr.Len() );
		}
		StringBuilder& Replace( int start, const StringBuilder& oldStr, const StringBuilder& newStr )
		{
			return Replace( start, INT_MAX, oldStr.c_str(), newStr.c_str(), oldStr.Len(), newStr.Len() );
		}
		StringBuilder& Replace( const StringBuilder& oldStr, const StringBuilder& newStr )
		{
			return Replace( 0, INT_MAX, oldStr.c_str(), newStr.c_str(), oldStr.Len(), newStr.Len() );
		}

		int ReplaceChars( const char* olds, const char* news )
		{
			return len = Str::ReplaceChars( ptr, olds, news );
		}

		int ReplaceChar( char oldc, char newc );

		bool IsEmpty() const
		{
			if( len == 0 ) return true;
			return false;
		}
		StringArray Split( char c ) const;
		StringArray Split( const char* s, int c_s = -1 ) const;
		int ToInt( bool hex = false )
		{
			return Str::ToInt( ptr, hex );
		}
		int ToInt( int start, bool hex = false );
		StringBuilder& LTrim( char c = ' ' )
		{
			len -= Str::LTrim( ptr, c );
			return *this;
		}
		StringBuilder& RTrim( char c = ' ' )
		{
			len -= Str::RTrim( ptr, c, len );
			return *this;
		}
		StringBuilder& Trim( char c = ' ' )
		{
			len -= Str::Trim( ptr, c, len );
			return *this;
		}
		//������������ ����� ������ (\r\n)
		StringBuilder& FillEndStr();
		//��������� ������ ��������� c, ������ ������ ������ count
		StringBuilder& Set( char c, int count );
		//��������� count �������� �����
		StringBuilder& Left( int count )
		{
			SetLen(len);
			return *this;
		}
		//��������� count �������� ������
		StringBuilder& Right( int count );
		//������������ ������ unicode � win1251
		StringBuilder& ToWin1251( const wchar_t* from, int c_from = -1 );

		int Ignore( int p, char c )
		{
			return Str::Ignore( ptr, p, c );
		}
};

//�������� ������ �� �����, ��� �������� ���������� ��������� ��������
template<int size_on_stack>
class StringBuilderStack : public StringBuilder
{
		char buf[size_on_stack];

		StringBuilderStack( const StringBuilderStack& );

	public:

		StringBuilderStack( const char* s = 0, int len = - 1 ) : StringBuilder( buf, size_on_stack, s, len )
		{
		}
		StringBuilder& operator=( const char* s )
		{
			return Copy(s);
		}
		StringBuilder& operator=( const StringBuilder& sb )
		{
			return Copy( sb.c_str(), sb.Len() );
		}
		int Size() const
		{
			return size_on_stack;
		}

};

//������ �����
typedef Vector< MovedPtr<StringBuilder> > StringArray;

#include "string_crypt.h"
