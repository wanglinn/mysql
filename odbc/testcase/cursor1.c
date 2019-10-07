/* setup : null
 *
 *
 */

#include <assert.h>
#include <sql.h>
#include <sqlext.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

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
    SQLGetDiagRec(handleType, handle, 1, SqlState, &NativeError,  
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
                     SQL_NTS,(SQLCHAR*)"password",SQL_NTS);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_DBC, serverhdbc, __LINE__);
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

/*
 * ExecSQL
 *
 */
int ExecSQL(void *hstmt, char *pSql)
{
    SQLRETURN iRet;
    iRet = SQLExecDirect(hstmt, pSql, SQL_NTS);
    return iRet;
}

#define ok_sql(hstmt, pSql) \
do \
{ \
    if (!SQL_SUCCEEDED(SQLExecDirect(hstmt, pSql, SQL_NTS))) \
    { \
        PrintError(SQL_HANDLE_STMT, hstmt, __LINE__); \
        exit(-1); \
    } \
}while(0)

#define ok_stmt(hstmt, iRet) \
do{ \
    if (!SQL_SUCCEEDED(iRet)) \
    { \
        PrintError(SQL_HANDLE_STMT, hstmt, __LINE__); \
        exit(-1); \
    }   \
}while(0)

void printMessage(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stdout, "# ");
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    va_end(ap);
}


int main()
{

    SQLRETURN iRet;
	SQLINTEGER col1, col2;
    SQLLEN length;
    SQLPOINTER nData = 0;
    char szData[20] = {0};
    SQLLEN nlen = 0;
    SQLULEN pcrow = 0;
    SQLUSMALLINT rgfRowStatus = 0;
 
    if (AllocHandle() != EXEC_SUCCESS)
        return -1;
    if (ConnectDB() != EXEC_SUCCESS)
        return -1;

    void *hstmt = serverhstmt;
    ok_sql(hstmt, "DROP TABLE IF EXISTS t_alias_setpos_del");
    ok_sql(hstmt, "CREATE TABLE t_alias_setpos_del (col1 INT, col2 VARCHAR(30))");

    ok_sql(hstmt, "INSERT INTO t_alias_setpos_del VALUES (100, 'MySQL1')");
    ok_sql(hstmt, "INSERT INTO t_alias_setpos_del VALUES (200, 'MySQL2')");
    ok_sql(hstmt, "INSERT INTO t_alias_setpos_del VALUES (300, 'MySQL3')");
    ok_sql(hstmt, "INSERT INTO t_alias_setpos_del VALUES (400, 'MySQL4')");


    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));
    ok_stmt(hstmt, SQLSetStmtAttr(hstmt, SQL_ATTR_CURSOR_TYPE,
                                (SQLPOINTER)SQL_CURSOR_STATIC, 0));

    ok_stmt(hstmt, SQLSetCursorName(hstmt, (SQLCHAR *)"venu", SQL_NTS));

    ok_sql(hstmt,"SELECT col1 AS id, col2 AS name FROM t_alias_setpos_del");

    ok_stmt(hstmt, SQLBindCol(hstmt, 1, SQL_C_LONG, &nData, 0, NULL));
    ok_stmt(hstmt, SQLBindCol(hstmt, 2, SQL_C_CHAR, szData, sizeof(szData),
                                &nlen));

    ok_stmt(hstmt, SQLExtendedFetch(hstmt, SQL_FETCH_ABSOLUTE, 1,
                                    &pcrow, &rgfRowStatus));

    printMessage("pcrow:%d, rgfRowStatus:%d", pcrow, rgfRowStatus);
    printMessage(" row1:%d, %s", nData, szData);

    ok_stmt(hstmt, SQLSetPos(hstmt, 1, SQL_POSITION, SQL_LOCK_NO_CHANGE));
    ok_stmt(hstmt, SQLSetPos(hstmt, 1, SQL_DELETE, SQL_LOCK_NO_CHANGE));

    ok_stmt(hstmt, SQLRowCount(hstmt, &nlen));

    printMessage("rows affected:%d",nlen);

    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_UNBIND));
    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));

    ok_sql(hstmt, "SELECT * FROM t_alias_setpos_del");

    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));

    ok_sql(hstmt, "DROP TABLE IF EXISTS t_alias_setpos_del");


    FreeHandle();

}
 
