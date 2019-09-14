#pragma once

namespace Mem
{
void Free( void* ptr );
}

//������ ������������ ����������� �����
#ifdef ON_CODE_STRING
	//��� ���������� � ���� �������
	#define _CS_(X) DECODE_STRING("BS" X "ES")
	#define _WCS_(X) DECODE_STRINGW("BS" X "ES")
	//��� ���������� �������� �����������, � ����� ������ ����������� ����� ����� ������������ DECODE_STRING
	#define _CT_(X) ("BS" X "ES")
//	#define _CS_(X) X
//	#define _CT_(X) X
#else
	#define _CS_(X) X
	#define _CT_(X) X
	#define _WCS_(X) L##X
#endif

//������������ ������ ��� ������������ ������
class StringDecoded
{
		char* ptr;

		StringDecoded( const StringDecoded& );

	public:

		StringDecoded( char* s ) : ptr(s)
		{
		}
		StringDecoded( StringDecoded&& s )
		{
			ptr = s.ptr;
			s.ptr = 0;
		}
		~StringDecoded()
		{
			//Str::Free(ptr);
			Mem::Free(ptr);
		}
		operator const char*() const
		{
			return ptr;
		}
		operator char*() const
		{
			return ptr;
		}
};

//������������ ������ ��� ������������ ������ ���� wchar_t
class StringDecodedW
{
		wchar_t* ptr;

		StringDecodedW( const StringDecoded& );

	public:

		StringDecodedW( wchar_t* s ) : ptr(s)
		{
		}
		StringDecodedW( StringDecodedW&& s )
		{
			ptr = s.ptr;
			s.ptr = 0;
		}
		~StringDecodedW()
		{
//			WStr::Free(ptr);
			Mem::Free(ptr);
		}
		operator const wchar_t*() const
		{
			return ptr;
		}
		operator wchar_t*() const
		{
			return ptr;
		}
};


//������� ������ ���� ���������� � �������� �������, �� ���� �������� ������������ ������
//� �� ������ �������� ������ StringDecoded, ���������� �������������� ������, ������ ��� ������� �������������� ������
//����� ���� ��� ��� �� ����� �����
#ifdef ON_CODE_STRING
StringDecoded DECODE_STRING( const char* );
char* DECODE_STRING2( const char* ); //������������ ������ ����� ������ �������
StringDecodedW DECODE_STRINGW( const char* );
wchar_t* DECODE_STRINGW2( const char* ); //������������ ������ ����� ������ �������
#else
#define DECODE_STRING(X) X
char* DECODE_STRING2( const char* ); //������������ ������ ����� ������ �������
StringDecodedW DECODE_STRINGW( const char* );
wchar_t* DECODE_STRINGW2( const char* ); //������������ ������ ����� ������ �������
#endif
