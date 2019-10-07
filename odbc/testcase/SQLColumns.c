/*
create table version(id1 char, id2 timestamp);
insert into version values('a', now());

https://www.easysoft.com/developer/languages/c/examples/ListTableColumns.html
*/
#include<sql.h>
#include<sqlext.h> 
#include<string.h>
#include<stdio.h>  
 


/** 简单的宏,用于判断ODBCAPI的执行结果是否正确 **/
#define isSuc(result) ((result) == SQL_SUCCESS || (result) == SQL_SUCCESS_WITH_INFO)

/** 可扩展设置 **/
char szConnectString[256] ="Driver={MYSQL};Database=db1;Server=127.0.0.1;uid=root;pwd=admin;";

char szSQL[256] = "select  * from version";
/** 输出缓存 **/
 char szDriverOutput[256];
short sDriverOutputLength;
/** 用于测试的变量 **/
int a = 0;
char b[255];
char c[255];
long temp = 0;

/** 用于测试的函数 **/
int main(){
 short sret; //返回代码
 void* henv;  //环境句柄
 void* hdbc;  //连接句柄
 long mode;//提交模式
 void* hstmt; //语句句柄
 TIMESTAMP_STRUCT curTime;
 /** 申请环境句柄 **/
 sret = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&henv);
 if(!isSuc(sret))printf("申请环境句柄出错\n");
 /** 设置环境属性,声明ODBC版本 **/
 sret = SQLSetEnvAttr(henv,SQL_ATTR_ODBC_VERSION,(void*)SQL_OV_ODBC3,SQL_IS_INTEGER);
 if(!isSuc(sret))printf("声明ODBC版本出错\n");
 /** 申请连接句柄 **/
 sret = SQLAllocHandle(SQL_HANDLE_DBC,henv,&hdbc);
 if(!isSuc(sret))printf("申请连接句柄出错\n");
 /** 设置连接属性 **/
 //sret = SQLSetConnectAttr(hdbc,SQL_ATTR_AUTOCOMMIT,(void*)SQL_AUTOCOMMIT_OFF,SQL_IS_INTEGER);
 //if(!isSuc(sret))printf("设置连接属性出错\n");
 /** 取得连接属性 **/
 //sret = SQLGetConnectAttr(hdbc,SQL_ATTR_AUTOCOMMIT,(void*)&mode,sizeof(mode) + 1,NULL);
 //if(!isSuc(sret))printf("取得连接属性出错\n");
 /** 连接数据源 **/
   sret = SQLDriverConnect(hdbc,NULL,(SQLCHAR*)szConnectString,SQL_NTS, (SQLCHAR *)szDriverOutput,256,&sDriverOutputLength,SQL_DRIVER_NOPROMPT);
  if(!isSuc(sret))printf("连接数据源出错\n");
 /** 分配语句句柄 **/
 sret = SQLAllocHandle(SQL_HANDLE_STMT,hdbc,&hstmt);
 if(!isSuc(sret))printf("分配语句句柄出错\n");
 

////////////////////////////////

#define STR_LEN 128 + 1
#define REM_LEN 254 + 1
// Arbitary sizes for number of tables and
#define MAX_TABLES 100

#define BUFFERSIZE 1024


SQLCHAR strSchema[STR_LEN];
SQLCHAR strCatalog[STR_LEN];
SQLCHAR strColumnName[STR_LEN];
SQLCHAR strTableName[STR_LEN];
SQLCHAR strTypeName[STR_LEN];
SQLCHAR strRemarks[REM_LEN];
SQLCHAR strColumnDefault[STR_LEN];
SQLCHAR strIsNullable[STR_LEN];

SQLINTEGER ColumnSize;
SQLINTEGER BufferLength;
SQLINTEGER CharOctetLength;
SQLINTEGER OrdinalPosition;

SQLSMALLINT DataType;
SQLSMALLINT DecimalDigits;
SQLSMALLINT NumPrecRadix;
SQLSMALLINT Nullable;
SQLSMALLINT SQLDataType;
SQLSMALLINT DatetimeSubtypeCode;
 
SQLLEN lenCatalog;
SQLLEN lenSchema;
SQLLEN lenTableName;
SQLLEN lenColumnName;
SQLLEN lenDataType;
SQLLEN lenTypeName;
SQLLEN lenColumnSize;
SQLLEN lenBufferLength;
SQLLEN lenDecimalDigits;
SQLLEN lenNumPrecRadix;
SQLLEN lenNullable;
SQLLEN lenRemarks;
SQLLEN lenColumnDefault;
SQLLEN lenSQLDataType;
SQLLEN lenDatetimeSubtypeCode;
SQLLEN lenCharOctetLength;
SQLLEN lenOrdinalPosition;
SQLLEN lenIsNullable;


 char *tablename = "version";
 sret = SQLColumns(hstmt, NULL, 0, NULL, 0, tablename, SQL_NTS, NULL, 0);

	SQLBindCol(hstmt, 1,  SQL_C_CHAR, strCatalog, STR_LEN, &lenCatalog);
	SQLBindCol(hstmt, 2,  SQL_C_CHAR, strSchema,STR_LEN, &lenSchema);
	SQLBindCol(hstmt, 3,  SQL_C_CHAR, strTableName,STR_LEN,&lenTableName);
	SQLBindCol(hstmt, 4,  SQL_C_CHAR, strColumnName,STR_LEN, &lenColumnName);
	SQLBindCol(hstmt, 5,  SQL_C_SSHORT, &DataType,0, &lenDataType);
	SQLBindCol(hstmt, 6,  SQL_C_CHAR, strTypeName,STR_LEN, &lenTypeName);
	SQLBindCol(hstmt, 7,  SQL_C_SLONG, &ColumnSize, 0, &lenColumnSize);
	SQLBindCol(hstmt, 8,  SQL_C_SLONG, &BufferLength,0, &lenBufferLength);
	SQLBindCol(hstmt, 9,  SQL_C_SSHORT, &DecimalDigits,0, &lenDecimalDigits);
	SQLBindCol(hstmt, 10, SQL_C_SSHORT, &NumPrecRadix,0, &lenNumPrecRadix);
	SQLBindCol(hstmt, 11, SQL_C_SSHORT, &Nullable, 0, &lenNullable);
	SQLBindCol(hstmt, 12, SQL_C_CHAR, strRemarks,REM_LEN, &lenRemarks);
	SQLBindCol(hstmt, 13, SQL_C_CHAR, strColumnDefault,STR_LEN, &lenColumnDefault);
	SQLBindCol(hstmt, 14, SQL_C_SSHORT, &SQLDataType,0, &lenSQLDataType);
	SQLBindCol(hstmt, 15, SQL_C_SSHORT, &DatetimeSubtypeCode, 0, &lenDatetimeSubtypeCode);
	SQLBindCol(hstmt, 16, SQL_C_SLONG, &CharOctetLength, 0, &lenCharOctetLength);
	SQLBindCol(hstmt, 17, SQL_C_SLONG, &OrdinalPosition, 0, &lenOrdinalPosition);
	SQLBindCol(hstmt, 18, SQL_C_CHAR, strIsNullable,STR_LEN, &lenIsNullable);


while (SQL_SUCCESS == SQLFetch(hstmt)) 
{
	printf (" Column Name : %s, ", strColumnName);
	printf ("Column Size : %i, ", ColumnSize);
	printf ("Data Type   : %i\n", SQLDataType);
}	


////////////////////////////////


 
 
 /** 释放语句句柄 **/
 SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
 /** 提交连接的事务 **/
 SQLEndTran(SQL_HANDLE_DBC,hdbc,SQL_COMMIT);
 /** 断开与数据源的连接 **/
 SQLDisconnect(hdbc);
 /** 释放连接句柄 **/
 SQLFreeHandle(SQL_HANDLE_DBC,hdbc);
 /** 释放环境句柄 **/
 SQLFreeHandle(SQL_HANDLE_ENV,henv);

}
