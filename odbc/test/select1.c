/*
create table version(id1 char, id2 timestamp);
insert into version values('a', now());


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
 void* hsmt; //语句句柄
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
 sret = SQLAllocHandle(SQL_HANDLE_STMT,hdbc,&hsmt);
 if(!isSuc(sret))printf("分配语句句柄出错\n");
 /** 执行语句 **/
 printf("execute sql: %s\n", szSQL);
 sret = SQLExecDirect(hsmt,(SQLCHAR *)szSQL,256);
 if(!isSuc(sret))printf("执行语句出错\n");
 /** 绑定结果集和获得数据 **/
 /**
 小提示，这里应该可以先取得字段列表然后用循环做成自动邦定的。 
 **/
 char szVersion[255];
 SQLBindCol(hsmt,1,SQL_C_CHAR,szVersion,256,&temp);//测试时
 SQLBindCol(hsmt,2, SQL_C_TIMESTAMP,&curTime,sizeof(TIMESTAMP_STRUCT),&temp);//测试时
 
 //SQLBindCol(hsmt,2,SQL_C_CHAR,&b,sizeof(b),&temp);//测试时
  /** 取得数据 **/
 while(1){
  //一般方式
  sret = SQLFetch(hsmt);
  /** 滚动 **/
  //sret = SQLFetchScroll(hsmt,SQL_FETCH_NEXT,0);
  /**
  前面如果发生错误,这里很可能得不到SQL_NO_DATA_FOUND而发生死循环，
  所以我不用而用SQL_SUCCESS来判断，但这句在杂复的程序中里依然有用。 
  **/
  if (sret == SQL_NO_DATA_FOUND) break; //如果没有数据就打断。
  //if (sret != SQL_SUCCESS) break;
  printf("a = %s\t",szVersion);//测试时
   printf("retTemp=%d\n",temp);
  printf("curTime =%d-%d-%d %d:%d:%d \t",curTime.year ,curTime.month,curTime.day,
    curTime.hour,curTime.minute,curTime.second);//测试时
    printf("retTemp=%d\n",temp);

 }
 /** 释放语句句柄 **/
 SQLFreeHandle(SQL_HANDLE_STMT,hsmt);
 /** 提交连接的事务 **/
 SQLEndTran(SQL_HANDLE_DBC,hdbc,SQL_COMMIT);
 /** 断开与数据源的连接 **/
 SQLDisconnect(hdbc);
 /** 释放连接句柄 **/
 SQLFreeHandle(SQL_HANDLE_DBC,hdbc);
 /** 释放环境句柄 **/
 SQLFreeHandle(SQL_HANDLE_ENV,henv);

}
