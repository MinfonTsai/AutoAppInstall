#include "stdafx.h"
#include "tomysql.h"

//=======================================================================
//初始化數據
int ToMysql::ConnMySQL(char *host,char * port ,char * Db,char * user,char* passwd,char * charset,char * Msg)
{
	if( mysql_init(&mysql) == NULL ) 
	{ 
	Msg = "inital mysql handle error"; 
	return 1; 
	} 

		if (mysql_real_connect(&mysql,host,user,passwd,Db,0,NULL,0) == NULL) 
		{ 
			Msg = "Failed to connect to database: Error"; 
			return 1; 
		} 
		if(mysql_set_character_set(&mysql,"GBK") != 0)
		{
			Msg = "mysql_set_character_set Error";
			return 1;
		}
		return 0;
}
//=======================================================================
//查詢資料
string ToMysql::SelectData(char * SQL,int Cnum,char * Msg)
{
MYSQL_ROW m_row; 
MYSQL_RES *m_res; 
char sql[2048]; 
sprintf(sql,SQL); 
int rnum = 0;
char rg = 0x06;//行隔開
char cg = {0x05};//欄位隔開
if(mysql_query(&mysql,sql) != 0) 
{ 
Msg = "select ps_info Error"; 
return ""; 
} 
m_res = mysql_store_result(&mysql); 
if(m_res==NULL) 
{ 
Msg = "select username Error"; 
return ""; 
} 
string str("");
while(m_row = mysql_fetch_row(m_res)) 
{ 
for(int i = 0;i < Cnum;i++)
{
str += m_row[i];
str += rg;
}
str += rg; 
rnum++;
} 
mysql_free_result(m_res); 
return str;
}

//===================================================================
//插入資料
int ToMysql::InsertData(char * SQL,char * Msg)
{
char sql[2048]; 
sprintf(sql,SQL); 
if(mysql_query(&mysql,sql) != 0) 
{ 
Msg = "Insert Data Error"; 
return 1; 
} 
return 0;
}

//===================================================================
//更新資料
int ToMysql::UpdateData(char * SQL,char * Msg)
{
char sql[2048]; 
sprintf(sql,SQL); 
if(mysql_query(&mysql,sql) != 0) 
{ 
Msg = "Update Data Error"; 
return 1; 
} 
return 0;
}

//===================================================================
//刪除資料
int ToMysql::DeleteData(char * SQL,char * Msg)
{
char sql[2048]; 
sprintf(sql,SQL); 
if(mysql_query(&mysql,sql) != 0) 
{ 
Msg = "Delete Data error";
return 1; 
} 
return 0;
}
//===================================================================
//關閉資料庫連接
void ToMysql::CloseMySQLConn()
{
mysql_close(&mysql); 
}
