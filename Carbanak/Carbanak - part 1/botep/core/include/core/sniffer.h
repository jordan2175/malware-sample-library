#pragma once

#include "core.h"

#define SIO_RCVALL         0x98000001

typedef struct IPHeader
{
	UCHAR   iph_verlen;   // ������ � ����� ���������
	UCHAR   iph_tos;      // ��� �������
	USHORT  iph_length;   // ����� ����� ������
	USHORT  iph_id;       // �������������
	USHORT  iph_offset;   // ����� � ��������
	UCHAR   iph_ttl;      // ����� ����� ������
	UCHAR   iph_protocol; // ��������
	USHORT  iph_xsum;     // ����������� �����
	ULONG   iph_src;      // IP-����� �����������
	ULONG   iph_dest;     // IP-����� ����������
} IPHeader;


namespace Sniffer
{

//���� ���������� true �� ���������� �������������
typedef bool (*typeIPPacket)( const IPHeader* header, const void* data, int c_data, void* tag );

//��������� ������ �� ��������� ipSrc � �������� ipDst ���� ������� ���������� ��������� protocol
//���� c_ipSrc = 0, �� ��������� IP ������ �� ����������� (���������� ���)
//���� c_ipDst = 0, �� �������� IP ������ �� ����������� (���������� ���)
//���� protocol = 0, �� ���������� ��� ���������
//wait - ����� �������� ������, ���� ����� �������� �������, �� � func ���������� ������ ������ � ������ ������ 0
//���������� ������������� ���� func ���������� true
void Filter( ULONG* ipSrc, int c_ipSrc, ULONG* ipDst, int c_ipDst, UCHAR protocol, typeIPPacket func, int wait, void* tag );

}
