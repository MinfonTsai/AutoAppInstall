// AutoAppInstall.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CAutoAppInstallApp:
// �аѾ\��@�����O�� AutoAppInstall.cpp
//

class CAutoAppInstallApp : public CWinApp
{
public:
	CAutoAppInstallApp();

// �мg
	public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CAutoAppInstallApp theApp;