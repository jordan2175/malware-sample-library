#pragma once
#include "manager.h"

namespace WndRec
{

const int STREAM_FILE = 0; //�������� �����
const int STREAM_VIDEO = 1; //����� �����
const int STREAM_LOG = 3; //����
const int STREAM_PIPE = 8; //���� �����

bool InitStreams();
//������� ����� � ���������� ��� ��
uint CreateStream( ServerData* server, uint typeId, const char* typeName, const char* fileName, const char* ext, int wait );
//������� ���������� ��� ���������� �������� ������ (������ ��� ����������� � ��������� ������������ ������ ������)
uint RecreateStream( ServerData* server, uint idOld, uint idNew );
//����� ������ � �����
void WriteStream( ServerData* server, uint id, const void* data, int c_data );
//��������� �����
void CloseStream( ServerData* server, uint id );

}
