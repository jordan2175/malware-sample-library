#pragma once

#include "core\core.h"

//������� ������� ����� ������������� � �������� ��������,
//��� ������ �������� ������� � �������� �� ����������

namespace Abstract
{

//���������� uid ���������� ��� �����
bool GetUid( StringBuilder& uid ); //��� ������� ����� �����������
bool _GetUid1( StringBuilder& uid ); //������� ���������� (�� ����������� �� ������ mac ������), � uid ������ ������� �� ����� 10 ��������

//������ ���� ������������ ��� ���������� � �������� ������ ������� (�������� � pipe)
extern const byte* XorVector;
extern int SizeXorVector;

}
