#include "stdafx.h"
#include "AutoAppInstall.h"
#include "AutoAppInstallDlg.h"
#include "tomysql.h"

#pragma   comment(lib, "ws2_32.lib")

#define  DEFAULT_XLSFILE   "test.xls"
#define  DEFAULT_SQLDBNAME   "test"
#define  DEFAULT_TABLENAME   "class1"
#define  TABLE_SELECT_COMMAND3    "select sid,name from class1"

ToMysql* schoolsql = new ToMysql;
extern CAutoAppInstallDlg *pDlg;
CComboBox* cb;

bool Open_SchoolSQL( void )
{
bool flag;
char dbname[100];

     strcpy(dbname, DEFAULT_SQLDBNAME );
   //  pDlg->m_dbname.GetWindowText(dbname,99);

   if( schoolsql->ConnMySQL("localhost","3306" ,dbname,"root","0000","utf8","ok")==0 )
	{
		//m_txt1.SetWindowText("Connect MySQL success!"); //   
		// AfxMessageBox("連接 MySQL Server: test 成功");
		//pDlg->m_txt1.SetWindowText( "連接 MySQL Server: test 成功" );
		flag = true;

		char *setname = "set names big5";
		mysql_query(&schoolsql->mysql,setname); 
	}
	else
	{
		AfxMessageBox("連接 MySQL  Server 失敗");
		//m_txt1.SetWindowText("FAIL");
		return false;
	}
	
	return flag;
}

void Close_SchoolSQL( void )
{
     schoolsql->CloseMySQLConn();
}


int GetName_from_MySQL( char *name , char *sid)
{
	char Msg[2000];
	char idbuf[100];
	char *buf;
	char *p1;
	int i, len , j, k;
	//查詢
	
	 // char * SQL1 = "SELECT name,sid,MAC  FROM class1 WHERE sid=1
	//char * SQL1 = "SELECT id,name,sid,MAC  FROM class1 WHERE MAC=''"; // 還沒有MAC登記過的機器
	char * SQL1 = "SELECT id,name,sid,MAC  FROM class1 WHERE LENGTH(MAC)<13"; // 還沒有MAC登記過的機器
	
    len =0;

	string str = schoolsql->SelectData(SQL1,4,Msg);
	
	if( str.length() > 0 )
	{
		len = str.length();
		buf=(char *)HeapAlloc(GetProcessHeap(),0, len+1);
        memcpy(buf,str.data(),len);

		for(k=0;k< len; k++)
		{
			if( *(buf+k) == 0x06 )
			{
				*(buf+k)=0;
				strcpy( idbuf, buf );
				//AfxMessageBox(str.data());
				break;
			}
		}
		for(i=k+1;i< len; i++)
		{
			if( *(buf+i) == 0x06 )
			{
				*(buf+i)=0;
				strcpy( name, (buf+k+1) );
				//AfxMessageBox(str.data());
				break;
			}
		}

		for(j=i+1;j< len; j++)
		{
			if( *(buf+j) == 0x06 )
			{
				*(buf+j)=0;
				strcpy( sid, (buf+i+1) );
				//AfxMessageBox(str.data());
				break;
			}
		}
		HeapFree(GetProcessHeap(),0,buf);
		//m_txt1.SetWindowText("查詢成功!");
		return atoi(idbuf);
	}

	return 0;
}

void GetAllUser_from_MySQL( void )
{
    char name[100];
	char Msg[2000];
	char idbuf[100];
	char *buf;
	char *p1;
	int i, len , j, k;
	//查詢
	
	 // "select * from class1"
	char * SQL1 = TABLE_SELECT_COMMAND3;   
    len =0;

	string str = schoolsql->SelectData(SQL1,2,Msg);

    if( str.length() > 0 )
	{
		len = str.length();
		buf=(char *)HeapAlloc(GetProcessHeap(),0, len+1);
        memcpy(buf,str.data(),len);
        j=0;
		cb = ((CComboBox*)pDlg->GetDlgItem(IDC_COMBO_USER));
		cb->ResetContent();  
		for(i=0;i< len; i++)
		{
			if( ( *(buf+i) == 0x06 ) && ( *(buf+i+1) == 0x06 ) )
			{
				*(buf+i)=0;
				strcpy( name, (buf+j) );
				for( k=0;k<strlen(name);k++)
				{
				   if( *(name+k) == 0x06 )
					   *(name+k) = '.';
				}
				cb->AddString(name);
				j = i+2;
				i++;
			}
		}
	}
	HeapFree(GetProcessHeap(),0,buf);
}

void UpdateMAC_to_MySQL( int this_id , char *new_macaddress)
{
   char * SQL1 = "SELECT name,sid,MAC  FROM class1 WHERE id=%d"; // 取出記錄
   char * SQL2 = "update class1 set name='%s',sid=%d, MAC='%s';";
   char * SQL3 = "insert into class1(name,sid,MAC) values('%s',%d,'%s');"; 
   char * SQL4 = "delete from class1 where id=%d;";
   char * SQL5 = "update class1 set name='%s',sid=%d ,MAC='%s' WHERE id=%d;";
   char Msg[2000];
   char sql_cmd[500];
   char name[100];
   char sidbuf[100];
   char *buf;
   char *p1;
	int i, len , j , k;
	string str;
	//查詢
	
	 // char * SQL1 = "SELECT name,sid,MAC  FROM class1 WHERE sid=1
	
    len =0;
    sprintf(sql_cmd,SQL1,this_id);
	str = schoolsql->SelectData(sql_cmd,3,Msg);
	
	if( str.length() > 0 )
	{
		len = str.length();
		buf=(char *)HeapAlloc(GetProcessHeap(),0, len+1);
        memcpy(buf,str.data(),len);

		for(i=0;i< len; i++)
		{
			if( *(buf+i) == 0x06 )
			{
				*(buf+i)=0;
				strcpy( name, buf );
				//AfxMessageBox(str.data());
				break;
			}
		}

		for(j=i+1;j< len; j++)
		{
			if( *(buf+j) == 0x06 )
			{
				*(buf+j)=0;
				strcpy( sidbuf, (buf+i+1) );
				//AfxMessageBox(str.data());
				break;
			}
		}

		/*	
        sprintf(sql_cmd,SQL4,this_id);
		//schoolsql->DeleteData(sql_cmd,Msg); 

		sprintf(sql_cmd,SQL3,name,atoi(sidbuf), new_macaddress); 

        if( schoolsql->InsertData( sql_cmd , Msg) == 0 )
           AfxMessageBox("更新記錄成功!");
		*/

		 sprintf(sql_cmd,SQL5,name,atoi(sidbuf), new_macaddress,this_id);
		 if( schoolsql->UpdateData(sql_cmd,Msg) == 0 )
			 ;
		//	AfxMessageBox("更新記錄成功!");
         
	} // if( str.length
		
HeapFree(GetProcessHeap(),0,buf);

}

void UpdateMAC_byName_to_MySQL( char *name, char *new_macaddress)
{
   char * SQL6 = "SELECT name,sid,MAC FROM class1 WHERE name='%s'"; // 取出記錄
 //  char * SQL2 = "update class1 set name='%s',sid=%d, MAC='%s';";
 //  char * SQL3 = "insert into class1(name,sid,MAC) values('%s',%d,'%s');"; 
 //  char * SQL4 = "delete from class1 where id=%d;";
   char * SQL5 = "update class1 set sid=%d ,MAC='%s' WHERE name='%s'";
   char sql_cmd[500];
   char name2[100];
   char sidbuf[100];
   char *buf;
   char *p1;
	int i, len , j , k;
	string str;
    char Msg[8000];

	len =0;
    sprintf(sql_cmd,SQL6,name);
	str = schoolsql->SelectData(sql_cmd,3,Msg);
	
	if( str.length() > 0 )
	{
		len = str.length();
		buf=(char *)HeapAlloc(GetProcessHeap(),0, len+1);
        memcpy(buf,str.data(),len);

		for(i=0;i< len; i++)
		{
			if( *(buf+i) == 0x06 )
			{
				*(buf+i)=0;
				strcpy( name2, buf );
				//AfxMessageBox(str.data());
				break;
			}
		}

		for(j=i+1;j< len; j++)
		{
			if( *(buf+j) == 0x06 )
			{
				*(buf+j)=0;
				strcpy( sidbuf, (buf+i+1) );
				//AfxMessageBox(str.data());
				break;
			}
		}

		
		 sprintf(sql_cmd,SQL5,atoi(sidbuf), new_macaddress,name);
		 if( schoolsql->UpdateData(sql_cmd,Msg) == 0 )
			 ;
		//	AfxMessageBox("更新記錄成功!");
         
	} // if( str.length
		
HeapFree(GetProcessHeap(),0,buf);
}