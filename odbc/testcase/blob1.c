/* setup : 
 * drop table if exists blob1;
 * create table blob1(id1 int, id2 blob);
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

extern void SetBuf(char *pBuf, int iBufLen);
extern void PrintError(int handleType, void *handle, int line);
extern int AllocHandle();
extern int ConnectDB();
extern int ExecSQL(char *pSql);
extern int FreeHandle();
extern int CreateTable();
extern int InsertData();

/*
 * SetBuf
 */
void SetBuf(char *pBuf, int iBufLen)
{
    int i = 0;
    for(i=0; i<iBufLen; i++)
    {
        pBuf[i] = '0' + i%10;
    }
}

/*
 * PrintError
 */
void PrintError(int handleType, void *handle, int line)
{
    SQLCHAR SqlState[6], SQLStmt[100], Msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER    NativeError;
    SQLSMALLINT   MsgLen;  
    SQLGetDiagRec(handleType, handle, 1, SqlState, &NativeError,  
            Msg, sizeof(Msg), &MsgLen);
    fprintf(stderr, "line %d : %s - %d - %s\n", line, SqlState, NativeError, Msg);

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

/*
 * ExecSQL
 * 
 */
int ExecSQL(char *pSql)
{
    SQLRETURN iRet;
    iRet = SQLExecDirect(serverhstmt, pSql, SQL_NTS);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    return EXEC_SUCCESS;
}

/*
 * FreeHandle
 * 
 */

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
 * CreateTable
 * 
 */
int CreateTable()
{
    SQLRETURN iRet;
    /* 创建表 */
    char *pCreateSQL = "create table blob1(id1 int, id2 blob)";
    char *pDropSQL = "drop table if exists blob1";
    fprintf(stdout, "%s\n", pDropSQL);
    fprintf(stdout, "%s\n", pCreateSQL);
    iRet = ExecSQL(pDropSQL);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }
    iRet = ExecSQL(pCreateSQL);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    return EXEC_SUCCESS;
}

/*
 * InsertData
 * 
 */
int InsertData()
{
    SQLRETURN iRet;
    /* 插入数据 */
    char *pSql = "INSERT INTO blob1 VALUES (1, ?)";
    fprintf(stdout, "%s\n", pSql);
    iRet = SQLPrepare(serverhstmt, (SQLCHAR *)pSql,
                              SQL_NTS);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    SQLUINTEGER blob_size = 0; //不影响实际输入数据大小
    SQLLEN cbValue = SQL_DATA_AT_EXEC;
    iRet = SQLBindParameter(serverhstmt, SQL_PARAM_INPUT, 1, SQL_C_BINARY,
                            SQL_LONGVARBINARY, blob_size, 0, NULL,
                            0, &cbValue);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    /* 构造数据 */
    char *blobbuf;
    int blobbuf_size = 100;

    blobbuf = (SQLCHAR *)malloc(blobbuf_size);
    SetBuf(blobbuf, blobbuf_size);

    /* 执行 */
    iRet = SQLExecute(serverhstmt);
    if (iRet != SQL_NEED_DATA)
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    /* 发送数据 */
    SQLPOINTER token;
    iRet = SQLParamData(serverhstmt, &token);
    if (iRet != SQL_NEED_DATA)
    {
        fprintf(stderr, "获取要发送的列出错， line %d\n", __LINE__);
        return EXEC_FAIL;
    }
    iRet = SQLPutData(serverhstmt, blobbuf, blobbuf_size);
    if (!SQL_SUCCEEDED(iRet))
    {
        fprintf(stderr, "发送数据出错， line %d\n", __LINE__);
        return EXEC_FAIL;
    }

    /* 释放内存 */
    free(blobbuf);

    iRet = SQLParamData(serverhstmt, &token);
    if (!SQL_SUCCEEDED(iRet))
    {
        fprintf(stderr, "设置要发送的列出错, line %d\n", __LINE__);
        return EXEC_FAIL;
    }

    /* 关闭连接 */
    iRet = SQLFreeStmt(serverhstmt, SQL_CLOSE);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    return EXEC_SUCCESS;
}


int SelectData()
{
    SQLRETURN iRet;
    char *blobbuf;
    int blobbuf_size = 100;
    SQLLEN cbValue;

    blobbuf = (char *)malloc(blobbuf_size);
    memset(blobbuf, 0, blobbuf_size);

    /* 准备 sql */
    char *pSql = "SELECT id1, id2 FROM blob1 WHERE id1 = 1";
    fprintf(stdout, "%s\n", pSql);
    iRet =  SQLPrepare(serverhstmt,(SQLCHAR *)pSql,
                       SQL_NTS);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    /* 绑定参数 */
    int iCol1 = 0;
    SQLLEN length;
    iRet = SQLBindCol(serverhstmt,1, SQL_C_LONG, &iCol1, sizeof(iCol1), &length);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    iRet = SQLBindCol(serverhstmt,2, SQL_C_BINARY, blobbuf, 0, &length);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    /* 执行 */
    iRet = SQLExecute(serverhstmt);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    /* 获取数据 */
    iRet = SQLFetch(serverhstmt);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    iRet = SQLGetData(serverhstmt, 2, SQL_C_BINARY, blobbuf, blobbuf_size, &cbValue);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    /* 打印 */
    fprintf(stdout, "out: %s\n", blobbuf);

    /* 释放内存 */
    free(blobbuf);

    /* 关闭连接 */
    iRet = SQLFreeStmt(serverhstmt, SQL_CLOSE);
    if(!SQL_SUCCEEDED(iRet))
    {
        PrintError(SQL_HANDLE_STMT, serverhstmt, __LINE__);
        return EXEC_FAIL;
    }

    return EXEC_SUCCESS;
}



int main()
{

    SQLRETURN iRet;
 
    if (AllocHandle() != EXEC_SUCCESS)
        return -1;
    if (ConnectDB() != EXEC_SUCCESS)
        return -1;

    /* 创建表 */
    if (CreateTable() != EXEC_SUCCESS)
        return -1;

    /* 插入数据 */
    if (InsertData() != EXEC_SUCCESS)
        return -1;

    /* 读取数据 */
    if (SelectData() != EXEC_SUCCESS)
        return -1;

    FreeHandle();

}
