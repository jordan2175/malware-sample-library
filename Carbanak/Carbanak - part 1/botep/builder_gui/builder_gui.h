
// builder_gui.h : ������� ���� ��������� ��� ���������� PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"		// �������� �������


// Cbuilder_guiApp:
// � ���������� ������� ������ ��. builder_gui.cpp
//

class Cbuilder_guiApp : public CWinApp
{
public:
	Cbuilder_guiApp();

// ���������������
public:
	virtual BOOL InitInstance();

// ����������

	DECLARE_MESSAGE_MAP()
};

extern Cbuilder_guiApp theApp;