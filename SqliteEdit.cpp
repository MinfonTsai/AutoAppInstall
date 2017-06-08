#include "stdafx.h"
#include "sqlite3.h"
#include <string>
#include <vector>
#include <sstream>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<io.h>

#define  DEFAULT_DB_FILE    "tiledb"    //預設檔名
#define  TABLE_SELECT_COMMAND    "select * from tiles;"
#define  TABLE_CREATE_COMMAND    "CREATE TABLE tiles ([id] INTEGER PRIMARY KEY, \
[title] varchar(256), [tiletype] varchar(64), [appname] varchar(64), [pkgname] varchar(64) ,[iconuri] varchar(64), [x] varchar(4), [y] varchar(4), [stockicon] varchar(4)"  

#define  TABLE_INSERT_COMMAND "INSERT INTO [tiles] ([title], [tiletype], [appname], [pkgname], [iconuri], [x], [y], [stockicon]) VALUES(?,?,?,?,?,?,?,?);"

#define  TABLE_UPDATE_COMMAND "UPDATE tiles SET title=?;"

//#define  TABLE_DELETE_COMMAND "DELETE FROM tiles where id=?;"
#define  TABLE_DELETE_COMMAND "DELETE FROM tiles where id=%d"

char dbfile[MAX_PATH+1];
sqlite3 *db;

int target_item;


	CString	m_Sname;
	CString	m_Sauthor;
	CString	m_Sinfo;
	CString	m_Ssize;
	CString	m_Surl;
	CString	m_Sposx;
	CString	m_Sposy;
	CString	m_Sicon;

void SavetoSQLite(void);
void UTF8_to_Code( UINT CodePage1 , UINT CodePage2 , char *source, char *result );

BOOL CheckFile(char * file)
{
	bool r = false;

	if (_access(file, 0) != -1)
	{
		r = true;
	}

	return r;
}

int initDataBase(void)
{
	strcpy( dbfile , DEFAULT_DB_FILE ); 

	
//	m_dbfile.GetWindowText(dbfile,MAX_PATH); 

	if (CheckFile(dbfile))
	{
		sqlite3_open(dbfile, &db);

		return 0;
	}
	else
	{
		char *zErrMsg = 0;
		int rc;

		sqlite3_open(dbfile, &db);

		if(db == NULL)
		{ 
			return -1;
		}

		rc = sqlite3_exec(db, "BEGIN;", 0, 0, &zErrMsg);

		

		rc = sqlite3_exec(db, TABLE_CREATE_COMMAND, 0, 0, &zErrMsg);

		rc = sqlite3_exec(db, "COMMIT;", 0, 0, &zErrMsg);

		return 0;
	}
}

int closeDatabase()
{
	return sqlite3_close(db);
}

bool CheckVaild()
{
	if (m_Sname == "")
	{
		return FALSE;
	}
	else if (m_Sauthor == "")
	{
		return FALSE;
	}
	else if (m_Sinfo == "")
	{
		return FALSE;
	}
	else if (m_Ssize == "MB")
	{
		return FALSE;
	}

	return TRUE;
}

void UTF8_to_Code( UINT CodePage1 , UINT CodePage2 , char *source, char *result )
{
	LPCSTR  t;
    wchar_t  U[2048]; 
	char S[2048];
    int utf16size;
    int   nLen;

    if ( source )
	{
		memset(S,0,sizeof(S));
		t = source;
		utf16size = MultiByteToWideChar(CodePage1, 0, t, -1, NULL, 0);
		MultiByteToWideChar   (CodePage1,   0,   t,   -1,   U , utf16size ); 
		nLen = wcslen(U)+1;  

		if( CodePage2 == CP_UTF8 )
			WideCharToMultiByte(CodePage2,   0,   U,   nLen,   S,   3*nLen,   NULL,   NULL);
		else
			WideCharToMultiByte(CodePage2,   0,   U,   nLen,   S,   2*nLen,   NULL,   NULL);
		//  變回原來的 一個中文字 3 bytes 碼
		strcpy( result , S );
	}
	else
        strcpy( result , "" );

}

void SQLite_Process( char *display_name ) 
{
	// TODO: Add your control notification handler code here
	//m_List_Date.DeleteAllItems();
    int len;

	len =strlen(display_name);
	if( len <= 0 || len > 100 )
      return;

	initDataBase();

	LVITEM lvi;
	int nrow = 0, ncolumn = 0, i = 0, id;
	char ** azResult;
	char *zErrMsg = 0;
  	char S1[256],S2[256],S3[256],S4[256],S5[256],S6[256],S7[256],S8[256];


	sqlite3_get_table(db, TABLE_SELECT_COMMAND , &azResult, &nrow, &ncolumn, &zErrMsg);

	for(i = 1; i <= nrow; i++)
	{
		lvi.mask = LVIF_TEXT;		//以結構體的方式插入列表項
		lvi.iItem = i-1;
		lvi.pszText=(LPTSTR)(LPCTSTR)(azResult[i*ncolumn]);
		lvi.iSubItem = 0;
/*
		this->m_List_Date.InsertItem(&lvi);
		this->m_List_Date.SetItemText(i-1,1,azResult[i*ncolumn+1]);
		this->m_List_Date.SetItemText(i-1,2,azResult[i*ncolumn+2]);
		this->m_List_Date.SetItemText(i-1,3,azResult[i*ncolumn+3]);
		this->m_List_Date.SetItemText(i-1,4,azResult[i*ncolumn+4]);
*/
/*
		this->m_List_Date.InsertItem(&lvi);
*/
		id = atoi(azResult[i*ncolumn+0]);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+4], S1);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+3], S2);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+6], S3);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+7], S4);

		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+8], S5);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+1], S6);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+2], S7);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+5], S8);
			
	m_Sname = _T("");
	m_Sauthor = _T("");
	m_Sinfo = _T("");
	m_Ssize = _T("");
	m_Surl = _T("");
	m_Sposx = _T("");
	m_Sposy = _T("");
	m_Sicon = _T("");

        m_Sname.Format(_T("%s"),S1);
		m_Sauthor.Format(_T("%s"),S2);
		m_Sinfo.Format(_T("%s"),S3);
		m_Ssize.Format(_T("%s"),S4);
		m_Surl.Format(_T("%s"),S5);
		m_Sposx.Format(_T("%s"),S6);
		m_Sposy.Format(_T("%s"),S7);
		m_Sicon.Format(_T("%s"),S8);

		if ( !strcmp( S6, "0" ) &&  !strcmp( S7, "0" ))
		{
			m_Sname.Format(_T("%s"), display_name );
			target_item = id;
			 SavetoSQLite();
			 break;
		}
		/*
		this->m_List_Date.SetItemText(i-1,1,azResult[i*ncolumn+4]);
		this->m_List_Date.SetItemText(i-1,2,azResult[i*ncolumn+3]);
		this->m_List_Date.SetItemText(i-1,3,azResult[i*ncolumn+6]);
		this->m_List_Date.SetItemText(i-1,4,azResult[i*ncolumn+7]);
        */

		/*
		this->m_List_Date.SetItemText(i-1,1,S1);
		this->m_List_Date.SetItemText(i-1,2,S2);
		this->m_List_Date.SetItemText(i-1,3,S3);
		this->m_List_Date.SetItemText(i-1,4,S4);
		this->m_List_Date.SetItemText(i-1,5,S5);
		this->m_List_Date.SetItemText(i-1,6,S6);
		this->m_List_Date.SetItemText(i-1,7,S7);
		this->m_List_Date.SetItemText(i-1,8,S8);
		*/
		
	}

	sqlite3_free_table(azResult);
	
	closeDatabase();
}

void Delete_SQLite(void) 
{
	// TODO: Add your control notification handler code here
	//獲取當前選擇的項目
//	int n=m_List_Date.GetSelectionMark();

	//如果確實有項目存在
//	if(n!=-1)
//	{
//		initDataBase();

		CString s_id,s_name,s_size,s_author,s_info,s_iconurl;
		
		//分別獲取它們的值
/*	
		s_id=m_List_Date.GetItemText( n, 0 );
		s_name=m_List_Date.GetItemText( n, 1 );
		s_size=m_List_Date.GetItemText( n, 2 );
		s_author=m_List_Date.GetItemText( n, 3 );
		s_info=m_List_Date.GetItemText( n, 4 );
		s_iconurl=m_List_Date.GetItemText( n, 5 );
*/
		char *zErrMsg = 0;
		int rc;
		sqlite3_stmt *stat;
		char exec[1024]="";

		

		rc = sqlite3_exec(db, "BEGIN;", 0, 0, &zErrMsg);
	
		sprintf(exec, TABLE_DELETE_COMMAND , target_item);
		rc = sqlite3_prepare(db, exec, -1, &stat, 0);

		//rc = sqlite3_bind_int(stat, 1, atoi(s_id.GetBuffer(s_id.GetLength())));
		rc = sqlite3_bind_int(stat, 1, target_item);
		rc = sqlite3_step(stat);

		rc = sqlite3_exec(db, "COMMIT;", 0, 0, &zErrMsg);

//		closeDatabase();

	//	this->OnRead();
//	}
}

void SavetoSQLite(void)   // 寫入資料庫
{
//	UpdateData(TRUE);

	if (!CheckVaild())
	{
		AfxMessageBox("輸入資料不完整!");
		
		return;
	}

	// TODO: Add extra validation here
	// initDataBase();

	int rc;
	sqlite3_stmt *stat;
	char exec[1024]="", insert_id[16]="";
    int   nLen;
	char  UTF8_str[1024];
    wchar_t  U1[512];
    LPCSTR  tt;
    int utf16size;
   
	sprintf(exec, TABLE_INSERT_COMMAND );

	rc = sqlite3_prepare(db, exec, -1, &stat, 0);

   UTF8_to_Code( 950, CP_UTF8 ,  m_Sname.GetBuffer(m_Sname.GetLength()), UTF8_str );
    
	//	rc = sqlite3_bind_text(stat, 1, m_Sname.GetBuffer(m_Sname.GetLength()), -1, NULL);

    rc = sqlite3_bind_text(stat, 1, UTF8_str, -1, NULL);
	rc = sqlite3_bind_text(stat, 2, m_Sauthor.GetBuffer(m_Sauthor.GetLength()), -1, NULL);
	rc = sqlite3_bind_text(stat, 3, m_Sinfo.GetBuffer(m_Sinfo.GetLength()), -1, NULL);
	rc = sqlite3_bind_text(stat, 4, m_Ssize.GetBuffer(m_Ssize.GetLength()), -1, NULL);
    rc = sqlite3_bind_text(stat, 5, m_Surl.GetBuffer(m_Surl.GetLength()), -1, NULL);
	rc = sqlite3_bind_text(stat, 6, m_Sposx.GetBuffer(m_Sposx.GetLength()), -1, NULL);
	rc = sqlite3_bind_text(stat, 7, m_Sposy.GetBuffer(m_Sposy.GetLength()), -1, NULL);
	rc = sqlite3_bind_text(stat, 8, m_Sicon.GetBuffer(m_Sicon.GetLength()), -1, NULL);


	rc = sqlite3_step(stat);

	rc = sqlite3_finalize(stat);

	sprintf(insert_id, "%ld", sqlite3_last_insert_rowid(db));

	//closeDatabase();
	
	//AfxMessageBox("已將記錄存入SQLite資料庫!!");

	m_Sname = _T("");
	m_Sauthor = _T("");
	m_Sinfo = _T("");
	m_Ssize = _T("");
	m_Surl = _T("");
	m_Sposx = _T("");
	m_Sposy = _T("");
	m_Sicon = _T("");
//	UpdateData(FALSE);

//	this->OnRead();

		Delete_SQLite();   //把原記錄 , 刪除
}