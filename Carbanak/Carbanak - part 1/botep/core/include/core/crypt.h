#pragma once

#include "core.h"

namespace Crypt
{

bool Init();
void Release();

//��������������� � base64
bool ToBase64( const void* data, int c_data, StringBuilder& s );
inline bool ToBase64( const Mem::Data& data, StringBuilder& s )
{
	return ToBase64( data.Ptr(), data.Len(), s );
}
//��������������� �� base64
bool FromBase64( const StringBuilder& s, Mem::Data& data );
//������������ ��� � ��������� ����� �������� (�������), �. �. �� ��� ������������� xor ����� � ��������� �������������� � base64
//����� ������� ��������� ������� ��� ���������� ��� ������� �����
StringBuilder& Name( const char* name, const char* xor, StringBuilder& s );
//������� ��� �����, ��� ���� ���� � ���������� �������� ��������
StringBuilder& FileName( const char* xor, StringBuilder& fileName );
//���������� ���������� RC2 ������ �� data
bool EncodeRC2( const char* psw, const char* IV, Mem::Data& data );
//���������� ���������� RC2 ������ �� data
bool DecodeRC2( const char* psw, const char* IV, Mem::Data& data );

//�������� ���������� ������ ��������� Abstract::XorVector
void EncodeDecodeXorVector( void* data, int c_data );
inline void EncodeDecodeXorVector( Mem::Data& data )
{
	EncodeDecodeXorVector( data.Ptr(), data.Len() );
}

}
