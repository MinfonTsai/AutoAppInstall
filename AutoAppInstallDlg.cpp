// AutoAppInstallDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "AutoAppInstall.h"
#include "AutoAppInstallDlg.h"
#include "InfoZip\InfoZip.h"
#include "io.h"
#include "fcntl.h"
#include "sys/stat.h"

#define  APKZIP_FILE        "I.bin"           // APK集合的壓縮檔
#define  PROTAPKZIP_FILE    "IP.bin"          // 已保護的APK集合的壓縮檔
#define  INSTALLED_FILE      "Installed.txt"     //已安裝-內含有版本號
#define  FILES_LIST_INDEX    "I.txt"             //預備安裝的清單(索引表)

#define  CONFIG_LIST_INDEX    "C.txt"            //自動設置的清單(索引表)
#define  CONFIGZIP_FILE       "C.bin"            // Config集合的壓縮檔

#define  Device_NAME1        "PD_Universe"         //機器名稱
#define  Device_PATH         "/mnt/PD_Universe"    //C3機的內部路徑
#define  Device_PATHX        "///PD_Universe/"   // "//mnt//PD_Universe/"
#define  MSG_DIALOG_1        "msg1.jpg" 
#define  PACKAGE_IDX_FILE    "/data/system/packages.xml"  //系統中有所已安裝APK的列表索引
#define  QUICK_VIEW_APK        "QuickPic_1.0.apk"         //快圖瀏覽 APK
#define  QUICK_VIEW_PKG        "com.alensw.PicFolder"     //快圖瀏覽 package name
#define  SETTING_DB_FILE     "/data/data/com.android.providers.settings/databases/settings.db"

#define ID_TIMER 1001 
VOID CALLBACK TimerProc1 ( HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime);
unsigned  ExecDosCmd(char *_ADBcmd);
bool Start_install_apk( char *filename ,char *activity, char flag);
bool Start_copy_apk( char *filename );
void Update_Installed_status( char *filename );
bool Download_Installed_status_from_device( char *filename );
bool Get_Installed_version( char *filename, char *ver_num ); 
void Show_picture_in_Android( void );
bool check_Install_index_file( char *index_file , char *path );  //檢查有無索引檔
void SQLite_Process( char *display_name ); 
bool Open_SchoolSQL( void );
int GetName_from_MySQL( char *name , char *sid);
bool Get_MAC_address_from_device( char *mac_address );
void UpdateMAC_to_MySQL( int user_id , char *new_macaddress);
void UpdateMAC_byName_to_MySQL( char *name, char *new_macaddress);
void Close_SchoolSQL( void );
bool Download_Packages_index( char *filepath );  
bool Upload_Packages_index( void );
bool Find_packages_inXML( char *package_name );
void Fixed_preferred_Item(void);
bool Download_Setting_DBfile( char *filepath );
bool Upload_Setting_DBfile( void );
void Check_Android_SettingDB2( void );
void Check_Android_SettingDB( int user_IP );
void Set_Language( void );
void show_SettingPage( void );
void set_System_Time( void );
bool Download_LauncherDB_file( void );  
bool Upload_LauncherDB_file( void );
void GetAllUser_from_MySQL(void);



CAutoAppInstallDlg *pDlg;
SECURITY_ATTRIBUTES sa; 
HANDLE hRead,hWrite;
char command[1024];   
STARTUPINFO si; 
PROCESS_INFORMATION pi; 
char bufferx1[4096];         
char bufferx2[4096];
DWORD bytesRead; 
char *found;
char pagetext[512];
char this_MAC_address[20];
char this_sid[20];
char this_username[100];
int  this_id;

char *indexbuf;
unsigned Install_sum;
bool force_install_flag;
bool remove_version_flag;
bool listAll_flag;

struct Install_IDX
{
	char action;
	char name[100];
    char activity[100];
};

Install_IDX  inst1[500];    //安裝最多500個

char *extract_filename;
char *activity_name;

CInfoZip InfoZip;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 每隔 0.5 秒檢查一下 device有無連接在USB上
VOID CALLBACK TimerProc1 ( HWND hwnd, UINT message, UINT iTimerID, DWORD dwTime)
{
   int flag,item;
   char mac_address[200],buf1[200];
   CComboBox* cc;

	//pDlg->KillTimer(ID_TIMER);
    //PostMessage (helpWnd, WM_KEYDOWN, 40, 0);
     strcpy( this_username ,"");

	 flag = ExecDosCmd( "adb devices"); 
     if( flag == 2 )
	 {      
		 
		 	Get_MAC_address_from_device( mac_address );
			strcpy(buf1,mac_address);
			*(buf1+6)=0;
			if( strcmp(buf1,"<none>" ) )
			{
				strcpy(this_MAC_address,mac_address); 
    			pDlg->GetDlgItem(IDC_EDIT_MAC)->SetWindowText( mac_address );
			
				if( Open_SchoolSQL() )
				{
					if( ! listAll_flag )
					{
					   GetAllUser_from_MySQL();
					   listAll_flag = true;
						this_id = GetName_from_MySQL( this_username , this_sid );   // 取得MySQL中還沒有登記 MAC的機器
						if( this_id )
						{
						  strcpy( buf1, this_sid );
						  strcat( buf1, "." );
						  strcat( buf1, this_username);
						  cc = ((CComboBox*)pDlg->GetDlgItem(IDC_COMBO_USER));

			     		  pDlg->GetDlgItem(IDC_EDIT_USER)->SetWindowText( buf1 ); 
						  item = cc->FindString( 1, buf1);
							cc->SetCurSel(item);
						   //item = (CComboBox*)pDlg->GetDlgItem(IDC_EDIT_USER)->FindString(buf1);
					    }	
					}

			        Close_SchoolSQL();
				}

				int check=((CButton*)(CWnd::FromHandle(GetDlgItem(hwnd,IDC_CHECK1))))->GetCheck();

			 pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("電子書包已備妥,可開始進行安裝.");
		 
			 if( !check )
   				 pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(TRUE);
         
			//		 status = pDlg->GetDlgItem(IDC_CHECK1)->GetCheck();

			 if( check ) pDlg->OnBnClickedButtonInstall();         // 免按鍵, 自動安裝
			}
			else
			{
				strcpy(this_MAC_address,"");
				pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("機器內的MAC地址無法取得,請先啟動WiFi");
				show_SettingPage();
				pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(FALSE);
			}
	 }
	 else if( flag == 3 )
	 {
         pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("同時有多台連接USB? 請保持只有1台連接!"); 
		 pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(FALSE);
	 }
	 else if( flag == 0 )
	 {
         pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("請確認驅動程式已經安裝,或檢查USB線已接上(應用->開發->偵錯)."); 
		 pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(FALSE);
	 }
}

unsigned  ExecDosCmd_noWait(char *_ADBcmd) 
{    
  char *fnext,*fnext1;
  unsigned flag;

sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
sa.lpSecurityDescriptor = NULL; 
sa.bInheritHandle = TRUE; 

if (!CreatePipe(&hRead,&hWrite,&sa,0)) 
{ 
    return 0; 
} 

strcpy(command,"Cmd.exe /C "); 
strcat(command, _ADBcmd); 

si.cb = sizeof(STARTUPINFO); 
GetStartupInfo(&si);
strcpy(si.lpTitle,"cmd.exe");
si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入 
si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入 
si.wShowWindow = SW_HIDE; //SSW_SHOWNORMAL; 
si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 

BOOL   res;  
//关键步骤，CreateProcess函数参数意义请查阅MSDN 
res = CreateProcess(NULL, command,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi);

  WaitForSingleObject(pi.hProcess, 300);    

	CloseHandle(pi.hThread); 
	CloseHandle(pi.hProcess); 
	CloseHandle(hWrite); 
    CloseHandle(hRead); 
	WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
    return res; 

}
//========================================================================
unsigned  ExecDosCmd(char *_ADBcmd) 
{    
  char *fnext,*fnext1;
  unsigned flag;

sa.nLength = sizeof(SECURITY_ATTRIBUTES); 
sa.lpSecurityDescriptor = NULL; 
sa.bInheritHandle = TRUE; 

if (!CreatePipe(&hRead,&hWrite,&sa,0)) 
{ 
    return 0; 
} 

strcpy(command,"Cmd.exe /C "); 
strcat(command, _ADBcmd); 

si.cb = sizeof(STARTUPINFO); 
GetStartupInfo(&si);
strcpy(si.lpTitle,"cmd.exe");
si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入 
si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入 
si.wShowWindow = SW_HIDE; //SSW_SHOWNORMAL; 
si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 

BOOL   res;  
//关键步骤，CreateProcess函数参数意义请查阅MSDN 
res = CreateProcess(NULL, command,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi);
if (res == FALSE) 
{ 
	CloseHandle(pi.hThread); 
	CloseHandle(pi.hProcess); 
	CloseHandle(hWrite); 
    CloseHandle(hRead); 
	WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
    return 0; 
} 

WaitForSingleObject(pi.hProcess,   INFINITE);      

/*
 if (WaitForSingleObject(pi.hProcess, 2000) != WAIT_OBJECT_0)
 {
    HWND hwnd1 = ::FindWindow(NULL,"cmd.exe"); 
  //::DestroyWindow(hwnd); 
   ::SendMessage(hwnd1,WM_CLOSE,0,0);
    return FALSE;
 }
*/

   while (true) 
   { 
	ZeroMemory( bufferx1,sizeof(bufferx1));
	ZeroMemory( bufferx2,sizeof(bufferx2));
    if (ReadFile(hRead,bufferx1,1000,&bytesRead,NULL) == NULL) 
   break; 
    //buffer中就是执行的结果，可以保存到文本，也可以直接输出 
   // AfxMessageBox(buffer);   //这里是弹出对话框显示 
	memcpy(bufferx2,bufferx1,4096);
	found = strstr(bufferx1,"connected"); 
	
	if( ! found )
	{
		if( strstr(bufferx1,"TTL") )
		{
          CloseHandle(pi.hThread); 
          CloseHandle(pi.hProcess); 
          CloseHandle(hWrite); 
	      CloseHandle(hRead); 
		  WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
           return 1; 
		}
		else if( strstr(bufferx1,"unable") )
		{
			CloseHandle(pi.hThread); 
            CloseHandle(pi.hProcess); 
            CloseHandle(hWrite); 
	        CloseHandle(hRead); 
		    WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
			return 0xFFFF;
		}
		else if( fnext = strstr(bufferx1,Device_NAME1) )
		{
			flag = 0x02;
            fnext1 = strstr(fnext+1,Device_NAME1);
			if( fnext1 )
               flag= 0x03;

			CloseHandle(pi.hThread); 
            CloseHandle(pi.hProcess); 
            CloseHandle(hWrite); 
	        CloseHandle(hRead); 
		    WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
			return flag;
		}
		else if( fnext = strstr(bufferx1,"MAC: ") )
		{
			flag = 0x04;
            *( fnext + 22 )=0;
			strcpy( this_MAC_address , ( fnext + 5 ) );

			CloseHandle(pi.hThread); 
            CloseHandle(pi.hProcess); 
            CloseHandle(hWrite); 
	        CloseHandle(hRead); 
		    WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
			return flag;	
		}
		else if( fnext = strstr(bufferx1,"Success") )
		{
			flag = 0x05;
			CloseHandle(pi.hThread); 
            CloseHandle(pi.hProcess); 
            CloseHandle(hWrite); 
	        CloseHandle(hRead); 
		    WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
			return flag;	
		}
		else if( fnext = strstr(bufferx1,"KB/s") )
		{
			flag = 0x06;
			CloseHandle(pi.hThread); 
            CloseHandle(pi.hProcess); 
            CloseHandle(hWrite); 
	        CloseHandle(hRead); 
		    WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
			return flag;	
		}
		else
		{
          CloseHandle(pi.hThread); 
          CloseHandle(pi.hProcess); 
          CloseHandle(hWrite); 
	      CloseHandle(hRead); 
		  WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
			return 0;
		}
	} //if( ! found )
    
    CloseHandle(pi.hThread); 
    CloseHandle(pi.hProcess); 
    CloseHandle(hWrite); 
	CloseHandle(hRead); 
	WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
	return 1; 
  } // while( ) 
    
    CloseHandle(pi.hThread); 
     CloseHandle(pi.hProcess); 
    CloseHandle(hWrite); 
    CloseHandle(hRead); 
	WriteFile(hWrite, "exit\r\n",6,&bytesRead,NULL);
   return 0;
}

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAutoAppInstallDlg 對話方塊




CAutoAppInstallDlg::CAutoAppInstallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoAppInstallDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoAppInstallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC1, m_txt1);
	DDX_Control(pDX, IDC_BUTTON_INSTALL, m_install_button);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress1);
	//	DDX_Control(pDX, IDC_BUTTON2, m_button_next);
	DDX_Control(pDX, IDC_CHECK1, m_check1);
	//DDX_Control(pDX, IDC_BUTTON1, m_button_force);
	DDX_Control(pDX, IDC_BUTTON2, m_button_next1);
	DDX_Control(pDX, IDC_BUTTON_FORCE, m_button_force1);
	DDX_Control(pDX, IDC_EDIT_MAC, m_edit_mac);
	DDX_Control(pDX, IDC_EDIT_USER, m_edit_user);
	DDX_Control(pDX, IDC_CHECK2, m_check2);
	DDX_Control(pDX, IDC_COMBO_USER, m_combo_user);
}

BEGIN_MESSAGE_MAP(CAutoAppInstallDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDTEST1, &CAutoAppInstallDlg::OnBnClickedTest1)
//	ON_BN_CLICKED(IDOK, &CAutoAppInstallDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_INSTALL, &CAutoAppInstallDlg::OnBnClickedButtonInstall)
	ON_BN_CLICKED(IDC_BUTTON2, &CAutoAppInstallDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_FORCE, &CAutoAppInstallDlg::OnBnClickedButtonForce)
	ON_CBN_SELCHANGE(IDC_COMBO_USER, &CAutoAppInstallDlg::OnCbnSelchangeComboUser)
END_MESSAGE_MAP()


// CAutoAppInstallDlg 訊息處理常式

BOOL CAutoAppInstallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	pDlg=(CAutoAppInstallDlg *)AfxGetMainWnd(); 
    pDlg->SetTimer(ID_TIMER,5000,TimerProc1);
	
	pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(FALSE);
	m_button_next1.ShowWindow(FALSE);
	m_progress1.ShowWindow(FALSE);
	m_button_force1.ShowWindow(FALSE);
    force_install_flag = FALSE;         // 不強迫安裝
    listAll_flag = FALSE;

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CAutoAppInstallDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CAutoAppInstallDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CAutoAppInstallDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
void CAutoAppInstallDlg::OnBnClickedTest1()
{
	// TODO: Add your control notification handler code here

	HANDLE hFile, hFileToWrite;
 CString strFilePath;
 m_edit1.GetWindowText(strFilePath);
 
 //打开要进行解压缩的文件
 hFile = CreateFile(strFilePath, // file name
  GENERIC_READ, // open for reading
  FILE_SHARE_READ, // share for reading
  NULL, // no security
  OPEN_EXISTING, // existing file only
  FILE_ATTRIBUTE_NORMAL, // normal file
  NULL); // no attr. template
 
 if (hFile == INVALID_HANDLE_VALUE)
 {
  AfxMessageBox("Could not open file to read"); // process error
  return;
 }
 
 HANDLE hMapFile, hMapFileToWrite;
 
 //创建一个文件映射
 hMapFile = CreateFileMapping(hFile, // Current file handle.
  NULL, // Default security.
  PAGE_READONLY, // Read/write permission.
  0, // Max. object size.
  0, // Size of hFile.
  "ZipTestMappingObjectForRead"); // Name of mapping object.
 
 if (hMapFile == NULL)
 {
  AfxMessageBox("Could not create file mapping object");
  return;
 }
 
 LPVOID lpMapAddress, lpMapAddressToWrite;
 
 //创建一个文件映射的视图用来作为source
 lpMapAddress = MapViewOfFile(hMapFile, // Handle to mapping object.
  FILE_MAP_READ, // Read/write permission
  0, // Max. object size.
  0, // Size of hFile.
  0); // Map entire file.
 
 if (lpMapAddress == NULL)
 {
  AfxMessageBox("Could not map view of file");
  return;
 }
 
 //////////////////////////////////////////////////////////////////////////////////
 DWORD dwFileLength,dwFileLengthToWrite;
 dwFileLength = GetFileSize(hFile, NULL) - sizeof(DWORD);
 //因为压缩函数的输出缓冲必须比输入大0.1% + 12 然后一个DWORD用来保存压缩前的大小，
 // 解压缩的时候用，当然还可以保存更多的信息，这里用不到
// dwFileLengthToWrite = (double)dwFileLength*1.001 + 12 + sizeof(DWORD);
 dwFileLengthToWrite = (*(DWORD*)lpMapAddress);

 LPVOID pSourceBuf = lpMapAddress;
 pSourceBuf = (DWORD*)pSourceBuf + 1;
 
 //以下是创建一个文件，用来保存压缩后的文件
 hFileToWrite = CreateFile("demo.jpg", // create demo.gz
  GENERIC_WRITE|GENERIC_READ, // open for writing
  0, // do not share
  NULL, // no security
  CREATE_ALWAYS, // overwrite existing
  FILE_ATTRIBUTE_NORMAL , // normal file
  NULL); // no attr. template
 
 if (hFileToWrite == INVALID_HANDLE_VALUE)
 {
  AfxMessageBox("Could not open file to write"); // process error
  return;
 }
 
 hMapFileToWrite = CreateFileMapping(hFileToWrite, // Current file handle.
  NULL, // Default security.
  PAGE_READWRITE, // Read/write permission.
  0, // Max. object size.
  dwFileLengthToWrite, // Size of hFile.
  "ZipTestMappingObjectForWrite"); // Name of mapping object.
 
 if (hMapFileToWrite == NULL)
 {
  AfxMessageBox("Could not create file mapping object for write");
  return;
 }
 
 lpMapAddressToWrite = MapViewOfFile(hMapFileToWrite, // Handle to mapping object.
  FILE_MAP_WRITE, // Read/write permission
  0, // Max. object size.
  0, // Size of hFile.
  0); // Map entire file.
 
 if (lpMapAddressToWrite == NULL)
 {
  AfxMessageBox("Could not map view of file");
  return;
 }
 
 //这里是将压缩前的大小保存在文件的第一个DWORD里面
 LPVOID pBuf = lpMapAddressToWrite;
 
 
 //////////////////////////////////////////////////////////////////////
 int i;
 //这里就是最重要的，zlib里面提供的一个方法，将源缓存的数据压缩至目的缓存
 //原形如下：
 //int compress (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
 //参数destLen返回实际压缩后的文件大小。
 i = uncompress((Bytef*)pBuf,&dwFileLengthToWrite, (Bytef*)pSourceBuf, dwFileLength);
 
 if(i==Z_OK) 
	 printf("1111111111111111111111111111111111\n");
  else if(i==Z_MEM_ERROR) 
	  printf("2222222222222222222222222222222222\n");
  else if(i==Z_BUF_ERROR) 
	  printf("3333333333333333333333333333333333\n");
  else if(i==Z_DATA_ERROR) 
	  printf("4444444444444444444444444444444444\n");
  else 
	  printf("5555555555555555555555555555555555\n");

 //////////////////////////////////////////////////////////////////////
 
 UnmapViewOfFile(lpMapAddress);
 CloseHandle(hMapFile);
 CloseHandle(hFile);
 
 UnmapViewOfFile(lpMapAddressToWrite);
 CloseHandle(hMapFileToWrite);
 //这里将文件大小重新设置一下
 SetFilePointer(hFileToWrite,dwFileLengthToWrite ,NULL,FILE_BEGIN);
 SetEndOfFile(hFileToWrite);
 CloseHandle(hFileToWrite);
}
*/
/*
void CAutoAppInstallDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
	
	CInfoZip InfoZip;
	if (!InfoZip.InitializeUnzip())
	{
		EndWaitCursor();
		AfxMessageBox("Must be initialized", MB_OK);
		return;
	}

	if (!InfoZip.ExtractFiles("I.bin", "."))
	{
		EndWaitCursor();
		AfxMessageBox("Files not added", MB_OK);
		return;
	}

	if (!InfoZip.FinalizeUnzip())
	{
		EndWaitCursor();
		AfxMessageBox("Cannot finalize", MB_OK);
		return;
	}
}
*/

CWinThread   *pThread;
//CAutoAppInstallDlg *pDlg; 

void make_into_install_item(void);
void Delete_file( char *filename , char *path );

UINT ThreadProc(LPVOID param)
{
   pDlg->Install_process();

   return FALSE;
}


void CAutoAppInstallDlg::OnBnClickedButtonInstall()
{
	
   pDlg=(CAutoAppInstallDlg *)AfxGetMainWnd(); 

    HWND hWnd=GetSafeHwnd();
    ( CWinThread *) pThread = AfxBeginThread(ThreadProc,hWnd);

}

void Personal_launcher7( void )
{
  	char  pszCurrentDir[MAX_PATH+1];
    char  temp_path[512];   
    char  display_idname[300];


	ZeroMemory(pszCurrentDir, MAX_PATH+1);
	GetCurrentDirectory(MAX_PATH+1, pszCurrentDir);

    GetTempPath(512,temp_path );
	SetCurrentDirectory( temp_path );
  
	strcpy(display_idname, this_sid );
	strcat(display_idname, ". " );
    strcat(display_idname, this_username );

    SQLite_Process( display_idname );   // 修改 tiledb 畫面上有寫上學生的名字
	
	SetCurrentDirectory(pszCurrentDir);

	/*
     strcpy(buf1,"adb push ");
	 strcat(buf1,temp_path);
	 strcat(buf1,"tiledb");
	 strcat(buf1," ");
	 strcat(buf1,"/data/data/info.tikusoft.launcher7/databases/");
	 ExecDosCmd(buf1); 
     Start_install_apk( "info.tikusoft.launcher7" , "", 'K' );   //重新啟動 Launcher
    */

}


void CAutoAppInstallDlg::Install_process()
{
	char temp_path[512];
    int i;
	char buf1[100];
	char name[256];
    char version_num1[20],version_num2[20];
    bool need_upgrade,ver_flag1,ver_flag2,flag,viewapk_flag,macOK_flag;
    char strbuf[100];
//	char username[100];
    char mac_address[20];
    bool check2;
	char pszCurrentDir[MAX_PATH+1],pszCurrentDir2[MAX_PATH+1];

	remove_version_flag=false;
    viewapk_flag = false;
    
    GetCurrentDirectory(MAX_PATH+1, pszCurrentDir2);
 
	pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(FALSE);
    pDlg->KillTimer(ID_TIMER);
	pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("正在檢查有無新版本...");

    //----------------------------------------    // 更新 Launcher桌面的 tiledb檔的用戶名稱
//	macOK_flag = false;
	
//	if( strcmp(this_MAC_address,"none" ) )
//	   macOK_flag = true;
  
    
	char strTemp[512];
     int len;

	    CComboBox  *pCombo = (CComboBox*)GetDlgItem(IDC_COMBO1);   
        pCombo->GetLBText(pCombo->GetCurSel(),strTemp);
        len = strlen( strTemp );
	    
		for(i=0;i<len;i++)
	    {
		  if( strTemp[i] == '.' )
		  {
			  strcpy( this_username , &strTemp[i+1] );
			  break;
		  }
	    }

	if( Open_SchoolSQL() )
	{
		UpdateMAC_byName_to_MySQL( this_username , this_MAC_address );  //依變更寫入新的Mac address.
		//Close_SchoolSQL();
    }

	Download_Installed_status_from_device( INSTALLED_FILE );
	ver_flag1 = Get_Installed_version( INSTALLED_FILE, version_num1 ); 
    
	need_upgrade = false;
	if( ! ver_flag1 ) need_upgrade = true;  // 如果沒有版本號, 本機器沒有安裝過
 	
	if (!InfoZip.InitializeUnzip())
	{
		EndWaitCursor();
		AfxMessageBox("Must be initialized", MB_OK);
		return;
	}

	// strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\ZIP\\");  // 測試看看
	GetTempPath(512,temp_path );

	Delete_file(  APKZIP_FILE , temp_path ); 
    Delete_file(  PROTAPKZIP_FILE , temp_path ); 
    Delete_file(  CONFIGZIP_FILE , temp_path ); 

    //----------------- 比對 ZIP內的最新版本與 機器內的已安裝 版本號 ----------- 
	    strcpy(name, INSTALLED_FILE );
        extract_filename = name;
        InfoZip.ExtractFiles( APKZIP_FILE, temp_path );
        ver_flag2 = Get_Installed_version( INSTALLED_FILE, version_num2 );

		if( ! need_upgrade )
		{
            if( ver_flag2  )
			{
				flag = strcmp( version_num1 , version_num2 );  //如果相同,flag值是FALSE
				if( flag )                                    //字串比對不同,flag值是TRUE
                  need_upgrade = TRUE;
			}
		}

        if( ! need_upgrade )   // 已經是最新版本, 不需要昇級了
		{
			strcpy( strbuf ,"已完成! 本機內已經是最新版本:");
			strcat( strbuf ,version_num1);
			strcat( strbuf , ",無需再安裝." );
		    
			m_txt1.SetWindowText(strbuf);
			
			if( force_install_flag ==FALSE )
			{
				m_button_force1.ShowWindow(TRUE);
				goto _Install_over;
			}
		 }


	check2 = m_check2.GetCheck();

    pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("正在檢查,請稍待...");

	
    GetTempPath(512,temp_path );
	ZeroMemory(pszCurrentDir, MAX_PATH+1);
	GetCurrentDirectory(MAX_PATH+1, pszCurrentDir);
	SetCurrentDirectory( temp_path );
   
	if( Download_Packages_index( PACKAGE_IDX_FILE ) )  // 取得機器內的已安裝列表檔
	{
        viewapk_flag = Find_packages_inXML( QUICK_VIEW_PKG );
		if( !viewapk_flag )             // 如果 快圖瀏覽 APK 沒有安裝
		{
		   Install_PICVIEWER_APK();          // 安裝 
	//	   Download_Packages_index( PACKAGE_IDX_FILE );   // 重新下載列表索引
	//	   Fixed_preferred_Item();           // 修正 prefer default 的列表
	//	   Upload_Packages_index();          // 上傳到 device
		}
	}
	Delete_file( "packages.xml" , temp_path );
    SetCurrentDirectory(pszCurrentDir);

	if( Download_Setting_DBfile(SETTING_DB_FILE) )  // 取得機器內的系統設定DB檔
	{
        Check_Android_SettingDB( atoi(this_sid) );
        Upload_Setting_DBfile(); 
		Check_Android_SettingDB2();
	}
    
	Set_Language(); 
	set_System_Time();

    Show_picture_in_Android();

 if( !check2 )    // 要安裝APK
 {
	  pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("正在安裝,請稍待...");
	if( ! check_Install_index_file( FILES_LIST_INDEX , "" )  )  // 載入本地端的索引檔
	{
		strcpy(name, FILES_LIST_INDEX );
		extract_filename = name;
		InfoZip.ExtractFiles( APKZIP_FILE, temp_path );
		check_Install_index_file( FILES_LIST_INDEX , temp_path ); // 載入從ZIP中取出來的索引檔
		Delete_file(  FILES_LIST_INDEX , temp_path ); 
        //load_Install_index_file();   
	}
    make_into_install_item();    // 建立安裝的項目, 計算總數
	
	//Install_sum = 2;  // 測試看看
    m_progress1.ShowWindow(TRUE);
	m_progress1.SetRange(0, Install_sum );
    m_progress1.SetStep(1);
 
	for(i=0;i< Install_sum ;i++)
	{
		extract_filename = inst1[i].name;
		activity_name = inst1[i].activity;
		

		//----------------- 一般Android 安裝 ------------------- 
		if( inst1[i].action == 'I' ||  inst1[i].action == 'U')
		{
			if (!InfoZip.ExtractFiles( APKZIP_FILE, temp_path ))
			{
				EndWaitCursor();
				AfxMessageBox("Files not added", MB_OK);
				return;
			}
			Start_install_apk( extract_filename , activity_name, inst1[i].action );

			// 要記得刪除檔案(ZIP和解壓的檔) , 以免出現安全漏洞
			Delete_file(  extract_filename , temp_path );   //刪除 APK 檔
			
		}
		
       //----------------- 已保護的 APK 複製到device內部 ----------- 
	   	if( inst1[i].action == 'I' )
		{
			if (!InfoZip.ExtractFiles( PROTAPKZIP_FILE, temp_path ))
			{
				EndWaitCursor();
				AfxMessageBox("Files not added", MB_OK);
				return;
			}
			
			Start_copy_apk( extract_filename  );

			// 要記得刪除檔案(ZIP和解壓的檔) , 以免出現安全漏洞
			Delete_file(  extract_filename , temp_path );   //刪除 APK 檔

		}

	   m_progress1.StepIt();
	}

	   Delete_file(  APKZIP_FILE , temp_path ); 
	   Delete_file(  PROTAPKZIP_FILE , temp_path ); 
       
	   //----------------- 校正版本已安裝的 版本號 ----------- 
	    strcpy(name, INSTALLED_FILE );
        extract_filename = name;
        Update_Installed_status( extract_filename  );
 }

//=================================================================================

 if( remove_version_flag == false)   // 移除 多於 安裝 , 不run設置
 {
      pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("系統設置中,請稍待...");

      if( ! check_Install_index_file( CONFIG_LIST_INDEX , "" )  )  // 載入本地端的索引檔
	{
		memset(name,0,sizeof(name));
		strcpy(name, CONFIG_LIST_INDEX );
		extract_filename = name;
		InfoZip.ExtractFiles( CONFIGZIP_FILE, temp_path );
		check_Install_index_file( CONFIG_LIST_INDEX , temp_path ); // 載入從ZIP中取出來的索引檔
		Delete_file(  CONFIG_LIST_INDEX , temp_path ); 
        //load_Install_index_file();   
	}
    make_into_install_item();    // 建立安裝的項目, 計算總數

    //Install_sum = 2;  // 測試看看
    m_progress1.ShowWindow(TRUE);
	m_progress1.SetRange(0, Install_sum );
    m_progress1.SetStep(1);
 
	for(i=0;i< Install_sum ;i++)
	{
			strcpy(strbuf,"adb remount");  // 變更 /system/為讀寫模式
			ExecDosCmd(strbuf); 

		//----------------- 設置檔 上傳 ------------------- 
		if( inst1[i].action == 'M' )  // 建立目錄 : Mkdir
		{
			extract_filename = inst1[i].name;
		    activity_name = inst1[i].activity;

			Start_install_apk( "", extract_filename , inst1[i].action );		
		}
		else  if( inst1[i].action == 'P' )  // 上傳到 Android機 : Push
		{
			extract_filename = inst1[i].activity;
		    activity_name = inst1[i].name;

			if (!InfoZip.ExtractFiles( CONFIGZIP_FILE, temp_path ))
			{
				EndWaitCursor();
				AfxMessageBox("Files not added", MB_OK);
				return;
			}
			Start_install_apk( activity_name,  extract_filename, inst1[i].action );

			// 要記得刪除檔案, 以免垃坄佔用硬碟
			Delete_file(  extract_filename , temp_path );   //刪除
		}
		else  if( inst1[i].action == 'H' )  // 變更檔案的讀寫權限 : chmod
		{
			extract_filename = inst1[i].name;
		    activity_name = inst1[i].activity;

			Start_install_apk( extract_filename , activity_name, inst1[i].action );
		}
		else if( inst1[i].action == 'O' )  // 變更檔案的用戶歸屬 : chown
		{
			extract_filename = inst1[i].name;
		    activity_name = inst1[i].activity;

			Start_install_apk( extract_filename , activity_name, inst1[i].action );
		}
        m_progress1.StepIt();
	}

      Delete_file(  CONFIGZIP_FILE , temp_path ); 

 }

	 if( this_id )
	 {
		  strcpy( buf1, this_sid );
		  strcat( buf1, ". " );
		  strcat( buf1, this_username);
		 Download_LauncherDB_file();  
		 SQLite_Process( buf1 );    // 修改 tiledb 畫面上有寫上學生的名字     
		 Upload_LauncherDB_file();

          UpdateMAC_to_MySQL( this_id , this_MAC_address );  // 加入機器的mac到 MySQL資料庫
	 }

	 Start_install_apk( "input keyevent ", "4" , 'X' );          //按下[Back] KEY  
     Start_install_apk( "info.tikusoft.launcher7" , "", 'K' );   //重新啟動 Launcher
     Start_install_apk( "input keyevent ", "4" , 'X' );          //按下[Back] KEY   

	  m_txt1.SetWindowText("安裝/設置全部完成!");
        force_install_flag = FALSE;

_Install_over:

		Close_SchoolSQL();

        strcpy(name, INSTALLED_FILE );
        extract_filename = name;
		Delete_file(  extract_filename , temp_path );   //刪除  

		SetCurrentDirectory(pszCurrentDir2);
		Delete_file( "settings.db" , "" );
		Delete_file( "tiledb" ,"" );


	if (!InfoZip.FinalizeUnzip())
	{
		EndWaitCursor();
		AfxMessageBox("Cannot finalize", MB_OK);
		return;
	}
    
    m_progress1.ShowWindow(FALSE);

	//int check=((CButton*)(CWnd::FromHandle(GetDlgItem(hwnd,IDC_CHECK1))))->GetCheck();
	int check = m_check1.GetCheck();
	
	if( !check )
	   m_button_next1.ShowWindow(TRUE);
	else
	{
       if( ! need_upgrade ) Sleep(5000);
 	   
       OnBnClickedButton2();
	}	
	AfxEndThread(0,1);
}


bool check_Install_index_file( char *index_file , char *path )  //檢查有無索引檔 
{
 WORD   handle;
 DWORD  filelen;
 char  full_path[512];

   strcpy( full_path, path );
   strcat( full_path, index_file );

    handle=_open((LPTSTR)full_path,_O_RDONLY | _O_BINARY);
	if(handle==0xffff)	return FALSE;
	_setmode(handle,_O_BINARY);
		
	filelen=filelength(handle);
	indexbuf=(char*)HeapAlloc(GetProcessHeap(),0,filelen); //配置記憶體
	_read(handle,indexbuf,filelen);
	_close(handle);

return TRUE;
}

bool Get_Installed_version( char *filename, char *ver_num )  // 取得已安裝在機器內的版本號
{
  WORD   handle;
  DWORD  filelen;
  char *f1,*f2;
  char temp_path[512];

   GetTempPath(512,temp_path );
   strcat(temp_path , filename );
    
    filelen=0;
    handle=_open((LPTSTR)temp_path,_O_RDONLY | _O_BINARY);
	if(handle==0xffff)	return FALSE;
	_setmode(handle,_O_BINARY);
		
	filelen=filelength(handle);
	if( filelen )
	{
		indexbuf=(char*)HeapAlloc(GetProcessHeap(),0,filelen); //配置記憶體
		_read(handle,indexbuf,filelen);
		_close(handle);

		f1 = indexbuf;
		f2 = strstr(f1,"=");
		memcpy(ver_num,(f2+1),13);
		*(ver_num+13)=0;
		HeapFree(GetProcessHeap(),0,indexbuf);  //釋放記憶體
		return TRUE;
	}
 return FALSE;
}

void make_into_install_item(void)
{
	int  i;
	char *f1,*f2,*f3,*f4;
    int I_num, U_num;

	i = 0; 
	U_num = 0;
    I_num = 0;

	f4 = indexbuf;

	while( *(f4+1) == ',' )
	{
		 f1 = strstr(f4,",");
		 inst1[i].action = *(f1-1);

         if( inst1[i].action =='I')  I_num++;
		 if( inst1[i].action =='U')  U_num++;

		 f2 = strstr( (f1+1),",");
         *f2 = 0;
		 strcpy( (char *)&(inst1[i].name),(f1+1) );

		 if( *(f2+1)=='#' )
		 {
			 inst1[i].activity[0] = 0;
			 f3 = f2 +2;
		 }
		 else
		 {
			 f3 = strstr( (f2+1),",");
			*f3 = 0;
             strcpy( (char *)&(inst1[i].activity),(f2+1) );
		 }

		 i++;
		 f4 = strstr(f3+1,"#");
		 f4 = f4 + 3;
	} 

   Install_sum = i;  //要安裝的總數

   if( I_num < U_num ) 
	   remove_version_flag = true;

   HeapFree(GetProcessHeap(),0,indexbuf);  //釋放記憶體
}

bool Start_install_apk( char *filename ,char *activity , char flag)
{
   char temp_path[512];
   char buf1[500];
	
      GetTempPath(512,temp_path );
    //strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\");

   if( flag == 'I' )       //  =====  安裝 ======
   {
		strcpy(buf1,"adb install ");
		strcat(buf1,temp_path);
        strcat(buf1,filename);
		return ExecDosCmd(buf1); 
   }	
   else if( flag == 'R' )   //  =====  重新安裝 ======
   { 

		strcpy(buf1,"adb -r install ");
		strcat(buf1,temp_path);
        strcat(buf1,filename);
		return ExecDosCmd(buf1); 

   }
   else if( flag == 'U' )   //  =====  卸載 Un-Install ======
   {
         strcpy(buf1,"adb uninstall ");
	     strcat(buf1,activity);
		return ExecDosCmd(buf1); 

   }
   else if( flag == 'M' )   //  =====  建立目錄 : Mkdir ======
   {
         strcpy(buf1,"adb shell mkdir ");
	     strcat(buf1,activity);
		return ExecDosCmd_noWait(buf1); 

   }
   else if( flag == 'P' )   //  =====  上傳到Android機 : Push  ======
   {
	//   if( !strcmp(activity,"tiledb") )
	//	    Personal_launcher7();      // 個人化 桌面檔launcher7 (加上學生姓名)

         strcpy(buf1,"adb push ");
		 strcat(buf1,temp_path);
	     strcat(buf1,activity);
		 strcat(buf1," ");
		 strcat(buf1,filename);
		return ExecDosCmd(buf1); 
   }
   else if( flag == 'H' )   //  =====  變更檔案權限 : Chmod  ======
   {
         strcpy(buf1,"adb shell chmod ");
		 strcat(buf1,activity);
		 strcat(buf1," ");
		 strcat(buf1,filename);
		return ExecDosCmd_noWait(buf1); 
   }
   else if( flag == 'O' )   //  =====  變更檔案用戶 : chown  ======
   {
         strcpy(buf1,"adb shell chown ");
		 strcat(buf1,activity);
		 strcat(buf1," ");
		 strcat(buf1,filename);
		return ExecDosCmd_noWait(buf1); 
   }
    else if( flag == 'K' )   //  =====  停止行程 : killall  ======
   {
         strcpy(buf1,"adb shell killall ");
		 strcat(buf1,filename);
		return ExecDosCmd_noWait(buf1); 
   }
	else if( flag == 'X' )   //  =====  其他 :   ======
   {
         strcpy(buf1,"adb shell ");
		 strcat(buf1,filename);
		 strcat(buf1,activity);
		return ExecDosCmd_noWait(buf1); 
   }


 return TRUE;
}
//========================================================================
bool Start_copy_apk( char *filename )
{
   char temp_path[512];
   char buf1[100];
	
      GetTempPath(512,temp_path );
    //strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\");
  
	    strcpy(buf1,"adb shell chmod 777 /data/app");
		strcpy(buf1,"adb push ");
		strcat(buf1,temp_path);
        strcat(buf1,filename);
		strcat(buf1," /data/app");
		return ExecDosCmd(buf1); 
  	
 return TRUE;
}

void Delete_file( char *filename , char *path )
{
    char fpath[500];

	strcpy(fpath,path);
	strcat(fpath,filename);
	DeleteFile(fpath);
	/*
		SHFILEOPSTRUCT   fo1; 
	char pszCurrentDir[MAX_PATH+1];
   
	ZeroMemory(pszCurrentDir, MAX_PATH+1);
	GetCurrentDirectory(MAX_PATH+1, pszCurrentDir);
    SetCurrentDirectory(path);

   fo1.wFunc   =   FO_DELETE;//复制就为FO_COPY,删除就为FO_DELETE,移動為FO_MOVE;;   
   fo1.pFrom   =   filename;   
   fo1.pTo   =    NULL;   
   fo1.fFlags   =   FOF_NOCONFIRMATION;   
   SHFileOperation(&fo1); 

   SetCurrentDirectory(pszCurrentDir);
   */

}

void Update_Installed_status( char *filename  )
{
   char temp_path[512];
   char buf1[100];
	
   if( remove_version_flag == true )   // 要移除機器內的 [版本註記] => Installed.txt
   {
        strcpy(buf1,"adb shell rm ");
		strcat(buf1,Device_PATH);
		strcat(buf1,"/");
	    strcat(buf1,filename);
        ExecDosCmd_noWait(buf1); 
   }
   else
   {
      GetTempPath(512,temp_path );
    //strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\");
  
	 	strcpy(buf1,"adb push ");
		strcat(buf1,temp_path);
        strcat(buf1,filename);
		strcat(buf1," ");
        strcat(buf1,Device_PATH);
	     ExecDosCmd(buf1); 
   }
}

bool Download_Installed_status_from_device( char *filename )
{
   char temp_path[512];
   char buf1[100];
   char c;

      GetTempPath(512,temp_path );
    //strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\ZIP\\");
  
	 	strcpy(buf1,"adb pull ");
		strcat(buf1,Device_PATH);
		strcat(buf1,"/");
		strcat(buf1,filename);
		strcat(buf1," ");
		strcat(buf1,temp_path);
         
		c = *(buf1+strlen(buf1)-1);
		if( c=='\\' )
           *(buf1+strlen(buf1)-1)=0;

	     ExecDosCmd(buf1); 

return TRUE;
}


bool Get_MAC_address_from_device( char *mac_address )
{
   char temp_path[512];
   char buf1[100];
   char c;

      GetTempPath(512,temp_path );
    //strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\ZIP\\");
  
	 	strcpy(buf1,"adb shell dumpsys wifi");
		if( ExecDosCmd(buf1) == 4 )
			strcpy( mac_address , this_MAC_address );

		if( strcmp( this_MAC_address ,"none" ) )
			return FALSE;

return TRUE;
}
void Show_picture_in_Android( void )
{
   //char temp_path[512];
   char buf1[100];
    //GetTempPath(512,temp_path );
    //strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\");

  		strcpy(buf1,"adb push ");
		strcat(buf1,MSG_DIALOG_1);
        strcat(buf1," ");
        strcat(buf1,Device_PATH);
		ExecDosCmd(buf1);

		//strcpy(buf1,"adb shell am start -a android.intent.action.VIEW -d file:///mnt//PD_Universe/light1.jpg -t image/*");
        strcpy(buf1,"adb shell am start -n com.alensw.PicFolder/.PictureActivity -d file:");
		strcat(buf1,Device_PATHX);
		strcat(buf1,MSG_DIALOG_1);
       // strcat(buf1," -t image/*"); 
		ExecDosCmd(buf1);
}
 

void CAutoAppInstallDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	//pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(TRUE);
	m_button_next1.ShowWindow(FALSE);
	m_button_force1.ShowWindow(FALSE);
	pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("USB 連接偵測中...");
	pDlg->SetTimer(ID_TIMER,5000,TimerProc1);
	pDlg->GetDlgItem(IDC_EDIT_USER)->SetWindowText( "" ); 
	pDlg->GetDlgItem(IDC_EDIT_MAC)->SetWindowText( "" );
}

void CAutoAppInstallDlg::OnBnClickedButton1()  
{
	// TODO: Add your control notification handler code here
	m_button_next1.ShowWindow(FALSE);
	m_button_force1.ShowWindow(FALSE);
	pDlg->GetDlgItem(IDC_STATIC1)->SetWindowText("USB 連接偵測中...");
	pDlg->SetTimer(ID_TIMER,5000,TimerProc1);
	pDlg->GetDlgItem(IDC_EDIT_USER)->SetWindowText( "" ); 
	pDlg->GetDlgItem(IDC_EDIT_MAC)->SetWindowText( "" );
}

void CAutoAppInstallDlg::OnBnClickedButtonForce()  //強迫安裝
{
	// TODO: Add your control notification handler code here
	

	pDlg->GetDlgItem(IDC_BUTTON_INSTALL)->EnableWindow(FALSE);
	m_button_next1.ShowWindow(FALSE);
	m_button_force1.ShowWindow(FALSE);
	force_install_flag = TRUE; 
	OnBnClickedButtonInstall();
}

bool Download_Packages_index( char *filepath )  //PACKAGE_IDX_FILE
{
   char temp_path[512];
   char buf1[500];
	
       // GetTempPath(512,temp_path );
       // strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\");
 		strcpy(buf1,"adb pull ");
		strcat(buf1,filepath);
        strcat(buf1," .");
		return ExecDosCmd(buf1); 
}	

bool Upload_Packages_index( void ) 
{
     char buf1[500];

 		strcpy(buf1,"adb push ");
		strcat(buf1,"packages.xml");
        strcat(buf1," /data/system");
		return ExecDosCmd(buf1); 
}	

bool Download_Setting_DBfile( char *filepath )  // SETTING_DB_FILE
{
   //char temp_path[512];
   char buf1[500];
	
       // GetTempPath(512,temp_path );
       // strcpy( temp_path,"C:\\Users\\minfon\\AppData\\Local\\");
 		strcpy(buf1,"adb pull ");
		strcat(buf1,filepath);
        strcat(buf1," .");
		return ExecDosCmd(buf1); 
}	

bool Upload_Setting_DBfile( void ) 
{
     char buf1[500];

 		strcpy(buf1,"adb push ");
		strcat(buf1,"settings.db");
        strcat(buf1," /data/data/com.android.providers.settings/databases");
		return ExecDosCmd(buf1); 
}	

bool Download_LauncherDB_file( void )  
{
   char temp_path[512];
   char buf1[500];
	
   		strcpy(buf1,"adb pull ");
		strcat(buf1,"/data/data/info.tikusoft.launcher7/databases/tiledb");
        strcat(buf1," .");
		return ExecDosCmd(buf1); 
}	

bool Upload_LauncherDB_file( void ) 
{
     char buf1[500];

 		strcpy(buf1,"adb push ");
		strcat(buf1,"tiledb");
        strcat(buf1," /data/data/info.tikusoft.launcher7/databases");
		ExecDosCmd(buf1); 

		strcpy(buf1,"adb shell chmod ");
		strcat(buf1,"/data/data/info.tikusoft.launcher7/databases/tiledb ");
		strcat(buf1,"666");
		ExecDosCmd_noWait(buf1);

	return true;
}	

void Set_Language( void )
{
 char buf1[500];

 		strcpy(buf1,"adb shell setprop persist.sys.language zh");
		ExecDosCmd_noWait(buf1); 

//		strcpy(buf1,"adb sell setprop persist.sys.country CN");
		strcpy(buf1,"adb shell setprop persist.sys.country TW");
        ExecDosCmd_noWait(buf1);
}

void show_SettingPage( void )
{
 char buf1[500];

 		strcpy(buf1,"adb shell am start -n com.android.settings/.Settings");
		ExecDosCmd(buf1); 
}

void set_System_Time( void )
{
	char buf1[500];
    char timebuf1[50],timebuf2[50];
	SYSTEMTIME     st;     
 		strcpy(buf1,"adb shell setprop persist.sys.timezone Asia/Shanghai");
		ExecDosCmd_noWait(buf1); 

     //adb shell date -s "20120801.120503" 

		GetLocalTime(&st);

		itoa(st.wYear,timebuf1,10);
		sprintf(buf1,"%0d",st.wMonth);
		strcat( timebuf1, buf1 );
		sprintf(buf1,"%0d",st.wDay);
		strcat( timebuf1, buf1 );
		strcat( timebuf1, "." );
		sprintf(buf1,"%0d",st.wHour);
		strcat( timebuf1, buf1 );
		sprintf(buf1,"%0d",st.wMinute);
		strcat( timebuf1, buf1 );
		sprintf(buf1,"%0d",st.wSecond);
		strcat( timebuf1, buf1 );

		strcpy(timebuf2,"adb shell date -s \"");
		strcat( timebuf2, timebuf1 );
		strcat( timebuf2, "\"" );	
	
		ExecDosCmd_noWait(timebuf2); 
}
void CAutoAppInstallDlg::Install_PICVIEWER_APK( void )
{
   char temp_path[512];
   GetTempPath(512,temp_path );

    strcpy( inst1[0].name , QUICK_VIEW_APK );
    strcpy( inst1[0].activity , QUICK_VIEW_PKG );
	
	extract_filename = inst1[0].name;
	activity_name = inst1[0].activity;
		

		//----------------- 一般Android 安裝 ------------------- 
		
			if (!InfoZip.ExtractFiles( APKZIP_FILE, temp_path ))
			{
				EndWaitCursor();
				AfxMessageBox("Files not added", MB_OK);
				return;
			}
	Start_install_apk( extract_filename , activity_name, 'I' );

	// 刪除檔案(ZIP和解壓的檔) , 以免出現安全漏洞
	Delete_file(  extract_filename , temp_path );   //刪除 APK 檔
			
}
void CAutoAppInstallDlg::OnCbnSelchangeComboUser()
{
	// TODO: Add your control notification handler code here
	/* char strTemp[512];
     int i,len;

      CComboBox* cc;
	  cc = ((CComboBox*)pDlg->GetDlgItem(IDC_COMBO_USER));
	  cc->GetWindowTextA(strTemp,500);
	 // cc->GetWindowTextA(
	   len = strlen( strTemp );
	   for(i=0;i<len;i++)
	   {
		  if( strTemp[i] == '.' )
		  {
			  strcpy( this_username , &strTemp[i+1] );
			  break;
		  }
	   }
	 */

// CComboBox  *pCombo = (CComboBox*)GetDlgItem(IDC_COMBO1);   
//    pCombo->GetLBText(pCombo->GetCurSel(),strName);

}
