#pragma once

#include "core\core.h"

//��������� ��������� ����� ���������� ������� ��������� � �������������, ��� ��������� (17,5)
namespace Rand
{

void Init();
uint Gen( int min, int max );
inline uint Gen( int max )
{
	return Gen( 0, max );
}
StringBuilder& Gen( StringBuilder& s, int min, int max );
inline StringBuilder& Gen( StringBuilder& s, int len )
{
	return Gen( s, len, len );
}
//���������� size ��������� ����� � ��������� 0 - 255
void Gen( byte* buf, int size );

//������� ����������� ����� �� ������ ��������� �����������
//���������� ����� �� 0 �� 100 � ���� ��� ������ v, �� ���������� true, ����� false
bool Condition( int v );

}
