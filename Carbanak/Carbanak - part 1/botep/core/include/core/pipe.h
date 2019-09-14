//������ ��� ���������������� ������

#pragma once

#include "core.h"
#include "misc.h"

class PipeResponse;

namespace Pipe
{

struct Msg;
typedef MovedPtr<Msg> AutoMsg;

//��� ������� ������� �������� ����� �� ������ �� pipe ������
typedef void (*typeReceiverPipeAnswer)( AutoMsg msg, DWORD tag );

//��������� �������������� �� ������ �������� �������
#pragma warning ( disable : 4200 )
#pragma pack(1)
struct Msg
{
	int cmd; //�� ���������
	char answer[32]; //������ ������ ���������� �����
	typeReceiverPipeAnswer func; //������� ����� �������� �����
	DWORD tag; //�������� ������� ������������ ��� func, ��� �������� � �������� ������ ����� �������� �������������� �������� ����� ��� � ��������
	int sz_data; //����� ������, ������ � ����������
	byte data[0];
};
#pragma pack()

//������� ������ ������ ���������� � msg
bool SendAnswer( const Msg* msg, int cmd, const void* data, int sz_data );

extern PipeResponse* serverPipeResponse;
//������������� ������� ������ ��������� �� ������, ������� �������� �� ��������� ��������
bool InitServerPipeResponse();

}

class PipePoint
{
	protected:

		HANDLE pipe;
		StringBuilderStack<32> remote;
		StringBuilderStack<32> name;
		bool onlyReadOrWrite; //true - ����� ��������������, ��� ������� ������ ������, ��� ������� ������ ������

		bool Read( Mem::Data& data );
		//�������� ������ � �����, ������ ��������� ������� ������ data ����� �������
		bool Write( const void* data, int c_data );
		StringBuilder& GetFullName( StringBuilder& fullName ); //������� � ���������� ������ ��� ������

	public:

		PipePoint();
		~PipePoint();
		const StringBuilder& GetName() const
		{
			return name;
		}
		void SetName( const char* _name ); //������������� ��� ������
		static void GenName( StringBuilder& newName ); //���������� ��� ������
		void Close();
		bool IsValid()
		{
			return pipe != nullptr;
		}
};

class PipeServer : public PipePoint
{
	protected:
		//������� ������� ��� ������ � ������ ������
		static const int SizeInBuffer = 1024;
		static const int SizeOutBuffer = 1024;

		//���������� �������� ���������, ���� ���������� ������ ������, �� �������� ����� ������ �� msgOut, ���� 0 - �� ������ ���, ���� -1, �� ������ ������ ��������� ������
		virtual int Handler( Pipe::Msg* msgIn, void** msgOut );
		//���������� ��� ��������� ������� CmdDisconnect, ����� ���� ������ ��������� ������
		virtual void Disconnect();
		//���� ������ ���������
		void Loop();
		//�������� ��������� ����������� ������� ����������� � ��������� ������, ��� ���� ��������� �����������
		//��� ������������ ���� ��������� ��������� �������� ������ �����, � ����� ����� ����������� ��� ����� �������
		static bool HandlerAsync( Pipe::typeReceiverPipeAnswer func, Pipe::Msg* msg, DWORD tag = 0 );

	public:

		enum
		{
			CmdDisconnect = 1000000 // ������� �� ���������� (����������) �������, �� ���� ������� ���������� ������� Disconncet()
		};

		PipeServer();
		~PipeServer();
		bool Start( bool onlyRead = true ); //��������� ����� �� ����� � ��������� ������, ������� ������ ����� �������� ������
		bool StartAsync( bool onlyRead = true ); //��������� ����� � ��������� ������
		void StartAnotherThread() //������ ������� ����� ����� � ������ ����� (����� ������ StartAsync � ������ ��������� onlyRead)
		{
			Start(onlyReadOrWrite);
		}
		void Stop(); //��������� �������, ������ �������� � ��� �� ������ ��� � ��� ������ (��� ��������� ��������� � Handler())
};

//�������� ������ � ���������� �� ��������� �������� (typereceiverPipeAnswer). ������� ����������� � ��������� �������
//����� �� ����������� ���� ������
class PipeResponse : public PipeServer
{

		virtual int Handler( Pipe::Msg* msgIn, void** msgOut );

	public:

		PipeResponse();
		~PipeResponse();
};

class PipeClient : public PipePoint
{

	protected:

	public:

		PipeClient( const char* _name ); //name - ��� ������ �������� ����� �������� ���������
		~PipeClient();
		bool Connect( bool onlyWrite = true ); //����������� � �������
		//����� � ����� ������
		bool Write( int cmd, const void* data, int sz_data, const char* nameReceiver = 0, Pipe::typeReceiverPipeAnswer funcReceiver = 0, DWORD tag = 0 );
		//�������� ������ � �������� ����� ����� � result
		//��� ���������� ������� ����� ����������� � ����� �����������
		bool PipeClient::Request( int cmd, const void* data, int sz_data, Mem::Data& result );
		//�������� ������, ���������� Write, ������ ��� ������ Connect() � � ����� Close()
		bool Send( int cmd, const void* data, int sz_data, const char* nameReceiver = 0, Pipe::typeReceiverPipeAnswer funcReceiver = 0, DWORD tag = 0 );
		bool Send( const void* data, int sz_data );
		//������� ��������� ���������� ������
		static bool Send( const char* namePipe, int cmd, const void* data, int sz_data, const char* nameReceiver = 0, Pipe::typeReceiverPipeAnswer funcReceiver = 0, DWORD tag = 0 );
		static bool Send( const char* namePipe, const void* data, int sz_data );
};
