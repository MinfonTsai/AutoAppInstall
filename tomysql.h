#include <stdio.h>
#include <string>
#include <afxsock.h>
#include "mysql.h" 

using namespace std;
class ToMysql 
{
public:
//變數
MYSQL mysql; 
/*
構造函數和稀構函數
*/
//VspdCToMySQL();
//~VspdCToMySQL();
/*
主要的功能：
初始化資料庫
連接資料庫
設置字元集
入口參數：
host ：MYSQL伺服器IP
port:資料庫埠
Db：資料庫名稱
user：資料庫使用者
passwd：資料庫使用者的密碼
charset：希望使用的字元集
Msg:返回的消息，包括錯誤消息
出口參數：
int ：0表示成功；1表示失敗
*/
int ConnMySQL(char *host,char * port,char * Db,char * user,char* passwd,char * charset,char * Msg);
/*
主要的功能：
查詢資料
入口參數：
SQL：查詢的SQL語句
Cnum:查詢的列數
Msg:返回的消息，包括錯誤消息
出口參數：
string 準備放置返回的資料，多條記錄則用0x06隔開,多個欄位用0x05隔開
如果 返回的長度＝ 0，責表示舞結果
*/
string SelectData(char * SQL,int Cnum ,char * Msg);
/*
主要功能：
插入資料
入口參數
SQL：查詢的SQL語句
Msg:返回的消息，包括錯誤消息
出口參數：
int ：0表示成功；1表示失敗
*/
int InsertData(char * SQL,char * Msg);
/*
主要功能：
修改資料
入口參數
SQL：查詢的SQL語句
Msg:返回的消息，包括錯誤消息
出口參數：
int ：0表示成功；1表示失敗
*/
int UpdateData(char * SQL,char * Msg);
/*
主要功能：
刪除資料
入口參數
SQL：查詢的SQL語句
Msg:返回的消息，包括錯誤消息
出口參數：
int ：0表示成功；1表示失敗
*/
int DeleteData(char * SQL,char * Msg);
/*
主要功能：
關閉資料庫連接
*/
void CloseMySQLConn();
};
