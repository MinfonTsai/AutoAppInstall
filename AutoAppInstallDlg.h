// AutoAppInstallDlg.h : 標頭檔
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAutoAppInstallDlg 對話方塊
class CAutoAppInstallDlg : public CDialog
{
// 建構
public:
	CAutoAppInstallDlg(CWnd* pParent = NULL);	// 標準建構函式
    void Install_PICVIEWER_APK( void );
// 對話方塊資料
	enum { IDD = IDD_AUTOAPPINSTALL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTest1();
	CEdit m_edit1;
	afx_msg void OnBnClickedOk();
	CStatic m_txt1;
	afx_msg void OnBnClickedButtonInstall();
	void Install_process();
	CButton m_install_button;
	CProgressCtrl m_progress1;
	afx_msg void OnBnClickedButton2();
	CButton m_button_next;
	CButton m_check1;
	CButton m_button_force;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonForce();
	CButton m_button_next1;
	CButton m_button_force1;
	CEdit m_edit_mac;
	CEdit m_edit_user;
	CButton m_check2;
	CComboBox m_combo_user;
	afx_msg void OnCbnSelchangeComboUser();
};
