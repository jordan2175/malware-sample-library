#pragma once

#include "socket.h"
#include "core.h"
#include "proxy.h"

namespace HTTP
{

class Request
{
		StringBuilderStack<128> host; //��� ����� ��� http
		StringBuilder file; //��� ��� ���� ����� ����� ����� � ����
		int port; //����� �����, �� ��������� 80
		Mem::Data data; //���������� � ���������� ������� ������
		Mem::Data postData; //������������ ������ ��� POST �������
		StringBuilder contentType; //��� ������������ ������

		//���������� ����������� ���������
		int answerCode; //��� ������
		int contentLen; //����� ���������� ������, -1 - ����� ���������� (�� �������)
		StringBuilder location; //��������
		int transferEncoding; //���� 1 (chunked), �� ����� �������� �� ���������� ������
		Proxy::Connector* cn; //��������� ����� ������� ���� ����������

	public:

		enum Protocol
		{
			HTTP, HTTPS
		};

	private:

		Protocol protocol;

		//������������ ��������� �������
		bool CreateHeader( StringBuilder& header, bool post, bool proxy, const StringBuilder& authentication );
		//������� � ��������, ������� ������� � ��������� ������
		bool SendRecv( const char* type, int wait, bool post );
		//���������� ������� ���������� ����
		bool Exec( const char* type, int wait, bool post );
		//����������� ���������� ��������� � ��������� ������ ���� ������
		bool AnalysisHeader( const StringBuilder& header );
		void Init( Protocol _protocol, Proxy::Connector* _cn = 0 );

	public:

		Request( Protocol _protocol = HTTP, Proxy::Connector* cn = 0 );
		Request( Proxy::Connector* cn );
		~Request();

		void SetHost( const char* _host ); //��������� �����
		//���-�� ����� ������ SetPort, ������� ��� ����� ��� ������� �� �����������
		#undef SetPort
		void SetPort( int _port )
		{
			port = _port;
		}
		void SetFile( const char* _file ); //��������� ����� ���� ������� ����� �����
		bool SetUrl( const char* url ); //����������� ��� � ��������� ��� �� ��� ����� � �����, ���� ��� �����, �� ���������� true
		//��������� ������ GET, wait - ����� �������� ������ � �������������
		bool Get( int wait = 0 ); 
		//��������� ������ POST, wait - ����� �������� ������ � �������������
		bool Post( int wait = 0 ); 
		StringBuilder& GetUrl( StringBuilder& url ) const;
		void NameProtocol( StringBuilder& name ) const; //��� ��������� ��� ������������ ����
		void VerProtocol( StringBuilder& name ) const; //��� � ������ ��������� ��� ������������ ��������� �������
		bool AddPostData( const void* ptr, int c_ptr ) //��������� ������ ��� POST �������
		{
			return postData.Append( ptr, c_ptr );
		}
		bool AddPostData( const StringBuilder& s )
		{
			return postData.Append( s.c_str(), s.Len() );
		}
		bool AddPostData( const Mem::Data& data )
		{
			return postData.Append( data.Ptr(), data.Len() );
		}

		Mem::Data& Response() //���������� ������ (�����)
		{
			return data;
		}
		void SetContentType( const char* s );
		void SetContentWebForm();
		void SetContentMultipart( const char* boundary );
		int AnswerCode() const
		{
			return answerCode;
		}
};

//��������� � ������ ���� ������ � ������� Multipart
class PostDataMultipart
{
		Request& request;
		StringBuilderStack<64> boundary;
		StringBuilderStack<8> end; //������ ����� ������

		//��������� ��� ����
		void AddName( StringBuilder& form, const char* name );

	public:

		PostDataMultipart( Request& _request );
		//��������� ���� name = value
		void AddValue( const char* name, const char* value );
		//��������� ����, ���������� ����� ������ ���� � data
		void AddFile( const char* name, const char* fileName, const void* data, int c_data );
		void AddFile( const char* name, const char* fileName, Mem::Data& data )
		{
			AddFile( name, fileName, data.Ptr(), data.Len() );
		}
		//���������� ���������� ������
		void End();
};

//�������� ������ ����� �������, ����� ���� ������ ���������� ������� ��� ����
StringBuilder& UrlEncode( const char* src, StringBuilder& dst );

}
