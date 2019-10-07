
/* setup: create table test1(id1 int, id2 int);
 * 
 *
 *
 */

#include<stdio.h>
#include <assert.h>
#include<sql.h>
#include <sqlext.h>
#include <stdlib.h>

SQLHENV serverhenv;
SQLHDBC serverhdbc;
SQLHSTMT serverhstmt;

typedef enum
{
    EXEC_FAIL,
    EXEC_SUCCESS
}EXEC_STATUS;


void PrintError(int handleType, void *handle, int line)
{
    SQLCHAR SqlState[6], SQLStmt[100], Msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER    NativeError;
    SQLSMALLINT   MsgLen;  
    SQLGetDiagRec(handle, handle, 1, SqlState, &NativeError,  
            Msg, sizeof(Msg), &MsgLen);
    fprintf(stderr, "line %d : %s - %d - %s", line, SqlState, NativeError, Msg);

}

/*
 * AllocHandle
 */
int AllocHandle()
{
    SQLRETURN iRet;
    //分配环境句柄
    iRet = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&serverhenv);
    if(!SQL_SUCCEEDED(iRet))
    {
        fprintf(stderr, "SQLAllocHandle error!\n");
        return EXEC_FAIL;
    }

    //设置环境属性
    iRet = SQLSetEnvAttr(serverhenv,SQL_ATTR_ODBC_VERSION,(void*)SQL_OV_ODBC3,0);
    if(!SQL_SUCCEEDED(iRet))
    {
        fprintf(stderr, "SQLSetEnvAttr error!\n");
        return EXEC_FAIL;
    }
 
    //分配连接句柄
    iRet = SQLAllocHandle(SQL_HANDLE_DBC,serverhenv,&serverhdbc);
    if(!SQL_SUCCEEDED(iRet))
    {
        fprintf(stderr, "SQLSetEnvAttr error!\n");
        return EXEC_FAIL;
    }

    return EXEC_SUCCESS;
}

/*
 * ConnectDB
 */
int ConnectDB()
{
    SQLRETURN iRet;
    //数据库连接
    iRet = SQLConnect(serverhdbc,(SQLCHAR*)"mysql-test",SQL_NTS,(SQLCHAR*)"root",
                     SQL_NTS,(SQLCHAR*)"admin",SQL_NTS);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    //分配执行语句句柄
    iRet = SQLAllocHandle(SQL_HANDLE_STMT,serverhdbc,&serverhstmt);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_DBC, serverhdbc, __LINE__);
        return EXEC_FAIL;
    }
    
    return EXEC_SUCCESS;
}

int FreeHandle()
{
    SQLRETURN iRet;
    //释放语句句柄
    iRet=SQLFreeHandle(SQL_HANDLE_STMT,serverhstmt);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }
 
    //断开数据库连接
    iRet=SQLDisconnect(serverhdbc);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }
 
    //释放连接句柄
    iRet=SQLFreeHandle(SQL_HANDLE_DBC,serverhdbc);
    if(!SQL_SUCCEEDED(iRet))
    {
        fprintf(stderr, "free hdbc error!\n");
        return EXEC_FAIL;
    }

    //释放环境句柄句柄
    iRet=SQLFreeHandle(SQL_HANDLE_ENV,serverhenv);
    if(!SQL_SUCCEEDED(iRet))
    {
        fprintf(stderr, "free henv error!\n");
        return EXEC_FAIL;
    }

    return EXEC_SUCCESS;
}

int main()
{

    SQLRETURN iRet;
	SQLINTEGER col1, col2;
    SQLLEN length;
 
    if (AllocHandle() != EXEC_SUCCESS)
        return -1;
    if (ConnectDB() != EXEC_SUCCESS)
        return -1;

	//insert data
	int ivalue = SQL_NULL_DATA;
	char *sqlstr = "insert into test1 values(NULL,?)";
	short int  v2 = 0;
	//iRet = SQLBindParameter(serverhstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ivalue, 0, &v2);
	//iRet = SQLBindParameter(serverhstmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, NULL, 0, &v2);

	short int ival = SQL_NULL_DATA;
	int va2 = SQL_NULL_DATA;

	iRet = SQLBindParameter(serverhstmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 4, 0, &va2, 0, &ival);

	/*
     * before execution is called
     */

	//ival = SQL_NULL_DATA;


	
	if(!SQL_SUCCEEDED(iRet))
	{
		fprintf(stdout, "error, line %d\n", __LINE__);
		exit(1);
	}
	v2 = SQL_NULL_DATA;
	iRet = SQLExecDirect(serverhstmt, sqlstr, SQL_NTS);
	if(!SQL_SUCCEEDED(iRet))
	{
		fprintf(stdout, "error, line %d\n", __LINE__);
		exit(1);
	}
    
    char *pSql = "select * from test1";
    fprintf(stdout, "sql : %s\n", pSql);
	//执行SQL语句
	iRet = SQLExecDirect(serverhstmt, (SQLCHAR *)pSql, SQL_NTS);
	if(SQL_SUCCEEDED(iRet))
	{
        //绑定数据
        SQLBindCol(serverhstmt,1, SQL_C_LONG, &col1, sizeof(col1), &length);
        SQLBindCol(serverhstmt,2, SQL_C_LONG, &col2, sizeof(col1), &length);
        while(SQL_NO_DATA != SQLFetch(serverhstmt))
        {
            fprintf(stdout, "result: %d, %d\n", col1, col2);
        }
    }

    FreeHandle();

}
 
