/* setup :
 *
 * note: 数据库有bug, 对于存储过程，执行结果不对
 * 调试跟踪发现odbc这边有bug,值从mysql取出来是正确的，但是在odbc这边没赋值上去
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
                     SQL_NTS,(SQLCHAR*)"admin",SQL_NTS);
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
 
    if (AllocHandle() != EXEC_SUCCESS)
        return -1;
    if (ConnectDB() != EXEC_SUCCESS)
        return -1;

    void *hstmt = serverhstmt;

/*********************************** start ************************************/
    char sql_create_proc[256] = { 0 };
    char sql_drop_proc[100] = { 0 };
    char sql_select[100] = { 0 };
    SQLLEN iSize = SQL_NTS, iSize1 = SQL_NTS;
    char blobValue[200] = { 0 }, binValue[200] = { 0 }, expcValue[200] = { 0 };
    SQLCHAR       SqlState[6], Msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER    NativeError;
    SQLSMALLINT   i, MsgLen;

    // connection strings
    strcpy(blobValue, "test data");
    strcpy(binValue, "test data");
    strcpy(expcValue, "test databar");

    sprintf(sql_create_proc, "%s", "CREATE PROCEDURE inoutproc (INOUT param1 "\
            "LONGBLOB, INOUT param2 LONG VARBINARY)"\
            " BEGIN"\
            " SET param1 := CONCAT(param1, 'bar'); "\
            " SET param2 := CONCAT(param2, 'bar'); "\
            " END;; ");
    sprintf(sql_drop_proc, "%s", "DROP PROCEDURE if exists inoutproc");
    sprintf(sql_select, "%s", "call inoutproc(?, ?)");
    //drop proc if already exists
    ok_stmt(hstmt, SQLExecDirect(hstmt, (SQLCHAR*)sql_drop_proc, SQL_NTS));
    //create proc
    ok_stmt(hstmt, SQLExecDirect(hstmt, (SQLCHAR*)sql_create_proc, SQL_NTS));
    //prepare the call statement
    ok_stmt(hstmt, SQLPrepare(hstmt, (SQLCHAR*)sql_select, SQL_NTS));
    //bind the call statement 1
    ok_stmt(hstmt, SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT_OUTPUT, SQL_C_BINARY,
                                    SQL_LONGVARBINARY, 0, 0, blobValue, 200, &iSize));
    //bind the call statement 1
    ok_stmt(hstmt, SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT_OUTPUT, SQL_C_BINARY,
                                    SQL_LONGVARBINARY, 0, 0, binValue, 200, &iSize1));
    // execute
    ok_stmt(hstmt, SQLExecute(hstmt));

    ok_stmt(hstmt, SQLFetch(hstmt));

    if (memcmp(binValue, expcValue, 12) != 0)
    {
        printf("OuT parameter does not match : Retrieved Value : %s\n", binValue);
        return -1;
    }
    if (memcmp(blobValue, expcValue, 12) != 0)
    {
        printf("OuT parameter does not match : Retrieved Value : %s\n", blobValue);
        return -1;
    }


/************************************ end *************************************/

    FreeHandle();

}
 
