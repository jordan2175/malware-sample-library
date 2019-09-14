#pragma once

#include "winapi.h"

//���������� ����
uint CalcHash( const byte* ptr, int c_ptr );

//����� ���������, ��������� ������� ���������
template< class T >
class AutoPtr
{
		mutable T* ptr;

	public:

		AutoPtr( const T& val );
		AutoPtr( T* val ) : ptr(val)
		{
		}
		AutoPtr( const AutoPtr& p )
		{
			ptr = p.ptr;
		}
		~AutoPtr();
		operator T&()
		{
			return *ptr;
		}
		operator T*()
		{
			return ptr;
		}
		T* operator->()
		{
			return ptr;
		}
};

//���� �����, ��� � AutoPtr, ������ ������������� ��� ����������� �������, ��������� ������� ���������� ��������� (������������� 0)
//���� ����������� ����� � ������������ � ����������� ��� ������ �� �������� ���������
template< class T >
class MovedPtr
{
		mutable T* ptr;

	public:

		MovedPtr( const T& val );
		MovedPtr( T* val )
		{
			ptr = val;
		}
		MovedPtr( const MovedPtr& p )
		{
			ptr = p.ptr;
			p.ptr = 0;
		}
		~MovedPtr();
		operator T&()
		{
			return *ptr;
		}
		operator const T&() const
		{
			return *ptr;
		}
		operator T*()
		{
			return ptr;
		}
		T* operator->()
		{
			return ptr;
		}
		const T* operator->() const
		{
			return ptr;
		}
		T& operator=( const MovedPtr& p )
		{
			ptr = p.ptr;
			p.ptr = 0;
			return *this;
		}
		bool IsValid() const
		{
			return ptr != nullptr;
		}
};


template< class T >
AutoPtr<T>::AutoPtr( const T& val )
{
	ptr = new T();
	*ptr = val;
}

template< class T >
AutoPtr<T>::~AutoPtr()
{
	if( ptr ) delete ptr;
}

template< class T >
MovedPtr<T>::MovedPtr( const T& val )
{
	ptr = new T();
	*ptr = val;
}

template< class T >
MovedPtr<T>::~MovedPtr()
{
	delete ptr;
}

//���������� ��� �����. ����� ����������� � mac (����� ������ ���� ����������, �� ������ 8-�� ����)
//������� ��������� ���� ���������� � mac, ���� 0, �� ������
int GetMacAddress( byte* mac );
//������ ������
bool RunThread( typeFuncThread func, void* data );
//������ ������ � ������� ��� ������
HANDLE RunThread( typeFuncThread func, void* data, DWORD* threadId );
//�������� � ������������� (������� ��� Sleep)
void Delay( uint v );

//������ � ������������ ��������
class CriticalSection
{
		CRITICAL_SECTION& cs;
	
	public:

		static void Init( CRITICAL_SECTION& cs );
		static void Enter( CRITICAL_SECTION& cs );
		static void Leave( CRITICAL_SECTION& cs );

		CriticalSection( CRITICAL_SECTION& _cs ) : cs(_cs)
		{
			Enter(cs);
		}
		~CriticalSection()
		{
			Leave(cs);
		}
};

//�� ������ ip:port �������� ip � ����
bool ExtractIpPort( const char* s, char* ip, int& port );
