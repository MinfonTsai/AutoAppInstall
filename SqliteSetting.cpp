#include "stdafx.h"
#include "sqlite3.h"
#include <string>
#include <vector>
#include <sstream>
#include	<fcntl.h>
#include	<sys/stat.h>
#include	<io.h>
#define  DEFAULT_DB_FILE    "settings.db"    //預設檔名
#define  TABLE_SELECT_COMMAND    "select * from system;"
#define  TABLE_SELECT_COMMAND2    "select * from secure;"
#define  TABLE_CREATE_COMMAND    "CREATE TABLE system ([_id] INTEGER PRIMARY KEY, [name] varchar(256), [value] varchar(256)"  

#define  TABLE_UPDATE_COMMAND    "UPDATE system SET value = \"%s\" WHERE _id=%d"
#define  TABLE_UPDATE_COMMAND2    "UPDATE secure SET value = \"%s\" WHERE _id=%d"
#define  TABLE_INSERT_COMMAND    "INSERT INTO system( \'name\', \'value\') VALUES(\'%s\', \'%s\');"
#define  TABLE_INSERT_COMMAND2    "INSERT INTO secure( \'name\', \'value\') VALUES(\'%s\', \'%s\');"

#define  X_TABLE_INSERT_COMMAND    "INSERT INTO [system] ([name], [value]) VALUES(?,?);"


#define  STR_STATIC_IP_1   "wifi_use_static_ip"
#define  STR_STATIC_IP_2   "wifi_static_ip"
#define  STR_STATIC_IP_3   "wifi_static_gateway"
#define  STR_STATIC_IP_4   "wifi_static_netmask"
#define  STR_STATIC_IP_5   "wifi_static_dns1"
#define  STR_STATIC_IP_6   "wifi_static_dns2"

#define  STATIC_IP_HEAD  "192.168.1."
#define  STATIC_IP_GATE  "192.168.1.1"
#define  STATIC_IP_MASK  "255.255.255.0"
#define  STATIC_IP_DNS1  "168.95.1.1"
#define  STATIC_IP_DNS2  "192.168.1.1"


#define  STR_ENINPUT_METHODS   "enabled_input_methods"
#define  SET_ENINPUT_METHODS   "com.android.inputmethod.latin/.LathinIME:jp.co.omronsoft.openwnn/.OpenWnnJAJP\
:com.android.inputmethod.pinyin/.PinyinIME:com.iqt.iqqijni.fc/.ZhuYinIME"

bool setflag[6]; 
int  id_forset[6];

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

sqlite3 *db2;
char dbfile2[MAX_PATH+1];

extern void UTF8_to_Code( UINT CodePage1 , UINT CodePage2 , char *source, char *result );


BOOL CheckFile1(char * file)
{
	bool r = false;

	if (_access(file, 0) != -1)
	{
		r = true;
	}

	return r;
}

int initDataBase1()
{
	
//	m_dbfile.GetWindowText(dbfile,MAX_PATH); 

	strcpy(dbfile2,DEFAULT_DB_FILE);

	if (CheckFile1(dbfile2))
	{
		sqlite3_open(dbfile2, &db2);

		return 0;
	}
	else
	{
		char *zErrMsg = 0;
		int rc;

		sqlite3_open(dbfile2, &db2);

		if(db2 == NULL)
		{ 
			return -1;
		}

		rc = sqlite3_exec(db2, "BEGIN;", 0, 0, &zErrMsg);

		

		rc = sqlite3_exec(db2, TABLE_CREATE_COMMAND, 0, 0, &zErrMsg);

		rc = sqlite3_exec(db2, "COMMIT;", 0, 0, &zErrMsg);

		return 0;
	}
}

void Check_Android_SettingDB2( void )
{
LVITEM lvi;
	int nrow = 0, ncolumn = 0, i = 0;
	char ** azResult;
	char *zErrMsg = 0;
  	char S1[256],S2[256],S3[256];
	int rc;
	sqlite3_stmt *stat;
	char exec[1024]="";
	int  device_IP;
    char buf1[500],buf2[100];

	setflag[0]=false; setflag[1]=false; setflag[2]=false; setflag[3]=false; setflag[4]=false; setflag[5]=false; 

	initDataBase1();
	
	rc = sqlite3_get_table(db2, TABLE_SELECT_COMMAND2 , &azResult, &nrow, &ncolumn, &zErrMsg);
	
	for(i = 1; i <= nrow; i++)
	{
		lvi.mask = LVIF_TEXT;		//以結構體的方式插入列表項
		lvi.iItem = i-1;
		lvi.pszText=(LPTSTR)(LPCTSTR)(azResult[i*ncolumn]);
		lvi.iSubItem = 0;

		//this->m_List.InsertItem(&lvi);

		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+0], S1);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+1], S2);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+2], S3);

		if( strstr( S2, STR_ENINPUT_METHODS ) )
		{
              setflag[0] = true;
			  id_forset[0] = atoi( S1 );
		}

	}

	 //============================================================================
	   if(  !setflag[0]  )  // 找不到 參數值
	   {
			strcpy(buf1,STR_ENINPUT_METHODS);
		    itoa(device_IP,buf2,10);
			strcat(buf1,buf2);
		    
			sprintf(exec, TABLE_INSERT_COMMAND2, SET_ENINPUT_METHODS, buf1 );
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 
	   }
	   else
	   {
		   memset(buf1,0,sizeof(buf1));
		    strcpy(buf1,SET_ENINPUT_METHODS);
		
			sprintf(exec, TABLE_UPDATE_COMMAND2, buf1 , id_forset[0] );  // value, ID in table
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 

	   }

	  
		 ///free azResult
		sqlite3_free_table( azResult );

			///close db
		sqlite3_close(db2);

}


void Check_Android_SettingDB( int user_IP )
{
	// TODO: Add your control notification handler code here

	LVITEM lvi;
	int nrow = 0, ncolumn = 0, i = 0;
	char ** azResult;
	char *zErrMsg = 0;
  	char S1[256],S2[256],S3[256];
	int rc;
	sqlite3_stmt *stat;
	char exec[1024]="";
	int  device_IP;
    char buf1[100],buf2[100];

	 device_IP= user_IP + 100;

	 setflag[0]=false; setflag[1]=false; setflag[2]=false; setflag[3]=false; setflag[4]=false; setflag[5]=false; 

	initDataBase1();
	
	rc = sqlite3_get_table(db2, TABLE_SELECT_COMMAND , &azResult, &nrow, &ncolumn, &zErrMsg);


	// m_List.DeleteAllItems();
	//	initDataBase();
    //    rc = sqlite3_get_table(db, TABLE_SELECT_COMMAND , &azResult, &nrow, &ncolumn, &zErrMsg);

	for(i = 1; i <= nrow; i++)
	{
		lvi.mask = LVIF_TEXT;		//以結構體的方式插入列表項
		lvi.iItem = i-1;
		lvi.pszText=(LPTSTR)(LPCTSTR)(azResult[i*ncolumn]);
		lvi.iSubItem = 0;

		//this->m_List.InsertItem(&lvi);

		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+0], S1);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+1], S2);
		UTF8_to_Code( CP_UTF8 , 950 , azResult[i*ncolumn+2], S3);
	

	//	this->m_List.SetItemText(i-1,1,S1);
	//	this->m_List.SetItemText(i-1,2,S2);
	//	this->m_List.SetItemText(i-1,3,S3);

		if( strstr( S2, STR_STATIC_IP_1 ) )
		{
              setflag[0] = true;
			  id_forset[0] = atoi( S1 );
		}
		else if( strstr( S2, STR_STATIC_IP_2 ) ) 
		{
			   setflag[1] = true;
			   id_forset[1] = atoi( S1 );
		}
		else if( strstr( S2, STR_STATIC_IP_3 ) ) 
		{
			setflag[2] = true;
			 id_forset[2] = atoi( S1 );
		}
		else if( strstr( S2, STR_STATIC_IP_4 ) ) 
		{
			setflag[3] = true;
			 id_forset[3] = atoi( S1 );
		}
		else if( strstr( S2, STR_STATIC_IP_5 ) ) 
		{
			setflag[4] = true;
			 id_forset[4] = atoi( S1 );
		}
		else if( strstr( S2, STR_STATIC_IP_6 ) ) 
		{
			setflag[5] = true;
			id_forset[5] = atoi( S1 );
		}
	
	//	this->m_List.SetItemText(i-1,0,S1);
	//	this->m_List.SetItemText(i-1,1,S2);
	//	this->m_List.SetItemText(i-1,2,S3);

	}


	 //============================================================================
       if( !setflag[0] )   // 找不到 設定  : 動態 or 固定IP
	   {
			sprintf(exec, TABLE_INSERT_COMMAND, STR_STATIC_IP_1, "1" );
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 
	   }
	   else
	   {
			sprintf(exec, TABLE_UPDATE_COMMAND, "1" , id_forset[0] );  // value, ID in table
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 

	   }
	   //============================================================================
	   if(  !setflag[1]  )  // 找不到 設定 : 固定 ip的值
	   {
			strcpy(buf1,STATIC_IP_HEAD);
		    itoa(device_IP,buf2,10);
			strcat(buf1,buf2);
		    
			sprintf(exec, TABLE_INSERT_COMMAND, STR_STATIC_IP_2, buf1 );
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 
	   }
	   else
	   {
		    strcpy(buf1,STATIC_IP_HEAD);
		    itoa(device_IP,buf2,10);
			strcat(buf1,buf2);

			sprintf(exec, TABLE_UPDATE_COMMAND, buf1 , id_forset[1] );  // value, ID in table
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 

	   }
	   //============================================================================
	   if(  !setflag[2]  )  // 找不到 設定 : GATEWAY
	   {
		
			sprintf(exec, TABLE_INSERT_COMMAND, STR_STATIC_IP_3, STATIC_IP_GATE );
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 
	   }
	   else
	   {
		  	sprintf(exec, TABLE_UPDATE_COMMAND, STATIC_IP_GATE , id_forset[2] );  // Gateway, ID in table
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 

	   }
	   //============================================================================
	   if(  !setflag[3] )   // 找不到 設定: MASK
	   {
		
			sprintf(exec, TABLE_INSERT_COMMAND, STR_STATIC_IP_4, STATIC_IP_MASK );
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 
	   }
	   else
	   {
		  	sprintf(exec, TABLE_UPDATE_COMMAND, STATIC_IP_MASK , id_forset[3] );  // Mask, ID in table
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 

	   }
	   //============================================================================
	   if( !setflag[4] )   // 找不到 設定:DNS-1
	   {
		
			sprintf(exec, TABLE_INSERT_COMMAND, STR_STATIC_IP_5, STATIC_IP_DNS1 );
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 
	   }
	   else
	   {
		  	sprintf(exec, TABLE_UPDATE_COMMAND, STATIC_IP_DNS1 , id_forset[4] );  // DNS-1,ID in table
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 

	   }
	   //============================================================================
	   if(  !setflag[5] )   // 找不到 設定:DNS-2
	   {
		
			sprintf(exec, TABLE_INSERT_COMMAND, STR_STATIC_IP_6, STATIC_IP_DNS2 );
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 
	   }
	   else
	   {
		  	sprintf(exec, TABLE_UPDATE_COMMAND, STATIC_IP_DNS2 , id_forset[5] );  // DNS-2,ID in table
			rc = sqlite3_prepare(db2, exec, -1, &stat, 0);
			rc = sqlite3_step(stat); 

	   }
	   //============================================================================


	   	sqlite3_finalize(stat);
		 ///free azResult
		sqlite3_free_table( azResult );

			///close db
		sqlite3_close(db2);

}