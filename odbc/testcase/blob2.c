/* setup : 
 * drop table if exists blob2;
 * create table blob2(id1 int, id2 blob);
 *
 *
 */

#include <assert.h>
#include<sql.h>
#include <sqlext.h>
#include <stdlib.h>
#include<stdio.h>

#include "stringinfo.h"

typedef  unsigned char ub1;
typedef unsigned int sword;
typedef signed int sb4;
typedef unsigned int ub4;
typedef unsigned short int ub2;
typedef int boolean;

typedef sb4 (*OCICallbackLobRead)(void  *ctxp, const void  *bufp,  
                                             ub4 len, ub1 piece);

typedef sb4 (*OCICallbackLobWrite)(void  *ctxp, void  *bufp, 
                                          ub4 *lenp, ub1 *piece);

#define TRUE  1
#define FALSE 0
#define MAX_PATH_LENGTH 1024

/*--------------------------- FILE open modes -------------------------------*/
#define OCI_FILE_READONLY 1             /* readonly mode open for FILE types */
/*---------------------------------------------------------------------------*/
/*--------------------------- LOB open modes --------------------------------*/
#define OCI_LOB_READONLY 1              /* readonly mode open for ILOB types */
#define OCI_LOB_READWRITE 2                /* read write mode open for ILOBs */
#define OCI_LOB_WRITEONLY     3         /* Writeonly mode open for ILOB types*/ 
#define OCI_LOB_APPENDONLY    4       /* Appendonly mode open for ILOB types */
#define OCI_LOB_FULLOVERWRITE 5                 /* Completely overwrite ILOB */ 
#define OCI_LOB_FULLREAD      6                 /* Doing a Full Read of ILOB */ 

/*----------------------- LOB Buffering Flush Flags -------------------------*/
#define OCI_LOB_BUFFER_FREE   1 
#define OCI_LOB_BUFFER_NOFREE 2
/*---------------------------------------------------------------------------*/

/*---------------------------LOB Option Types -------------------------------*/
#define OCI_LOB_OPT_COMPRESS     1                    /* SECUREFILE Compress */
#define OCI_LOB_OPT_ENCRYPT      2                     /* SECUREFILE Encrypt */
#define OCI_LOB_OPT_DEDUPLICATE  4                 /* SECUREFILE Deduplicate */
#define OCI_LOB_OPT_ALLOCSIZE    8             /* SECUREFILE Allocation Size */
#define OCI_LOB_OPT_CONTENTTYPE 16                /* SECUREFILE Content Type */
#define OCI_LOB_OPT_MODTIME     32           /* SECUREFILE Modification Time */

/*------------------------   LOB Option Values ------------------------------*/
/* Compression */
#define OCI_LOB_COMPRESS_OFF  0                           /* Compression off */
#define OCI_LOB_COMPRESS_ON   1                            /* Compression on */
/* Encryption */
#define OCI_LOB_ENCRYPT_OFF   0                            /* Encryption Off */
#define OCI_LOB_ENCRYPT_ON    2                             /* Encryption On */
/* Deduplciate */
#define OCI_LOB_DEDUPLICATE_OFF 0                         /* Deduplicate Off */
#define OCI_LOB_DEDUPLICATE_ON  4                        /* Deduplicate Lobs */



SQLHENV serverhenv;
SQLHDBC serverhdbc;
SQLHSTMT serverhstmt;

#if 0
typedef enum
{
    EXEC_FAIL,
    EXEC_SUCCESS
}EXEC_STATUS;

#endif 

typedef struct OCILobLocator
{
    StringInfo strInfo;
    int iPostion;
    SQLHSTMT serverhstmt;
    boolean bOpen;
    boolean bNeedSend;
    char *pWriteSql;
    char *pReadSql;
    char dir_alias[MAX_PATH_LENGTH];
    int d_length;
    char filename[MAX_PATH_LENGTH];
    int f_length;
}OCILobLocator;

typedef struct OCISvcCtx
{
   int i;
}OCISvcCtx;

typedef struct OCIError
{
    int i;
}OCIError;



extern void SetBuf(char *pBuf, int iBufLen);
extern void PrintError(int handleType, void *handle, int line);
extern int AllocHandle();
extern int ConnectDB();
extern int ExecSQL(char *pSql);
extern int FreeHandle();
extern int CreateTable();
extern int InsertData();

sword OCILobFileOpen(OCISvcCtx *svchp,
                     OCIError *errhp,
                     OCILobLocator *filep,
                     ub1 mode);

sword OCILobRead (OCISvcCtx *svchp,
                    OCIError *errhp,
                    OCILobLocator *locp,
                    ub4 *amtp,
                    ub4 offset,
                    void *bufp,
                    ub4 bufl,
                    void *ctxp, 
                    OCICallbackLobRead cbfp,
                    ub2 csid,
                    ub1 csfrm);

sword OCILobWrite(OCISvcCtx *svchp,
                    OCIError *errhp,
                    OCILobLocator *locp,
                    ub4 *amtp,
                    ub4 offset,
                    void *bufp, 
                    ub4 buflen,
                    ub1 piece,
                    void *ctxp, 
                    OCICallbackLobWrite cbfp,
                    ub2 csid,
                    ub1 csfrm);

sword OCILobIsOpen(OCISvcCtx *svchp,
                   OCIError *errhp, 
                   OCILobLocator *locp, 
                   boolean *flag);

sword OCILobClose(OCISvcCtx *svchp,
                  OCIError *errhp, 
                  OCILobLocator *locp);

sword OCILobFileSetName(OCIEnv *envhp,
                        OCIError *errhp,
                        OCILobLocator **filepp,
                        const OraText *dir_alias,
                        ub2 d_length, 
                        const OraText *filename,
                        ub2 f_length);

/*
 * OCILobFileOpen
 *
 */
sword OCILobFileOpen(OCISvcCtx *svchp,
                     OCIError *errhp,
                     OCILobLocator *filep,
                     ub1 mode)
{
    /* get all data to local */
    SQLRETURN iRet;
    char *blobbuf;
    int blobbuf_size = 128;
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

    /* 初始化 StringInfo */
    filep->strInfo = makeStringInfo();

    while(1)
    {
        cbValue = 0;
        iRet = SQLGetData(serverhstmt, 2, SQL_C_BINARY, blobbuf, blobbuf_size,
                          &cbValue);
        if(!SQL_SUCCEEDED(iRet))
        {
            break;
        }
        if (appendBinaryStringInfo(filep->strInfo, blobbuf, cbValue) 
            != EXEC_SUCCESS)
        {
            return EXEC_FAIL;
        }
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

    filep->bOpen = TRUE;

    return EXEC_SUCCESS;
}


/*
 * OCILobFileOpen
 *
 */
sword OCILobRead (OCISvcCtx *svchp,
                    OCIError *errhp,
                    OCILobLocator *locp,
                    ub4 *amtp,
                    ub4 offset,
                    void *bufp,
                    ub4 bufl,
                    void *ctxp, 
                    OCICallbackLobRead cbfp,
                    ub2 csid,
                    ub1 csfrm)
{
    if (NULL == locp || NULL == locp->strInfo
        || FALSE == locp->bOpen)
    {
        return EXEC_FAIL;
    }

    if (offset > locp->strInfo->len)
    {
        return EXEC_FAIL;
    }

    int iLen = 0;
    if (offset + bufl > locp->strInfo->len)
    {
        if (bufl > locp->strInfo->len - offset)
        {
            iLen = locp->strInfo->len - offset + 1;
        }
        else
        {
            iLen = locp->strInfo->len - offset;
        }
    }
    else
    {
        iLen = bufl;
    }
    *amtp = iLen;
    memcpy((char *)bufp, locp->strInfo->data + offset - 1, iLen);

    char *pReadSql = "select id2 from blob2 where id1 = 1";
    locp->pReadSql = (char *)malloc(strlen(pReadSql) + 1);
    memset(locp->pReadSql, 0 , strlen(pReadSql) + 1);
    strncpy(locp->pReadSql, pReadSql, strlen(pReadSql));

    return EXEC_SUCCESS;
}

/* OCILobWrite
 *
 *
 */
sword OCILobWrite(OCISvcCtx *svchp,
                    OCIError *errhp,
                    OCILobLocator *locp,
                    ub4 *amtp,
                    ub4 offset,
                    void *bufp, 
                    ub4 buflen,
                    ub1 piece,
                    void *ctxp, 
                    OCICallbackLobWrite cbfp,
                    ub2 csid,
                    ub1 csfrm)
{
    if (NULL == amtp || NULL == bufp || NULL == locp)
    {
        return EXEC_FAIL;
    }

    if (locp->bOpen == FALSE)
    {
        return EXEC_FAIL;
    }

    /* 将数据写入 */
    if (offset > locp->strInfo->len + 1)
    {
        return EXEC_FAIL;
    }

    int iCopyLen = 0;
    int iAppendLen = buflen;
    /* 先复制表数据 */
    if (offset < locp->strInfo->len)
    {
        if (buflen + offset <= locp->strInfo->len + 1)
        {
            iCopyLen = buflen;
        }
        else
        {
            iCopyLen = locp->strInfo->len - offset + 1;
        }
        iAppendLen = buflen - iCopyLen;
        char *pData = locp->strInfo->data + offset  - 1;
        strncpy(pData, bufp, iCopyLen);
    }

    char *pData = bufp + iCopyLen;
    if (appendBinaryStringInfo(locp->strInfo, pData, iAppendLen)
                != EXEC_SUCCESS)
    {
        return EXEC_FAIL;
    }

    *amtp = buflen;
    locp->bNeedSend = TRUE;
    char *pWriteSql = "update blob2 set id2 = ? where id1=1";
    locp->pWriteSql = (char *)malloc(strlen(pWriteSql) + 1);
    memset(locp->pWriteSql, 0, strlen(pWriteSql) + 1);
    strncpy(locp->pWriteSql, pWriteSql, strlen(pWriteSql) + 1);

    return EXEC_SUCCESS;
}

/*
 * OCILobIsOpen
 *
 */
sword OCILobIsOpen(OCISvcCtx *svchp,
                   OCIError *errhp, 
                   OCILobLocator *locp, 
                   boolean *flag)
{
    if (NULL == locp)
    {
        return EXEC_FAIL;
    }

    *flag = locp->bOpen;

    return EXEC_SUCCESS;
}

/*
 * OCILobClose
 *
 */
sword OCILobClose(OCISvcCtx *svchp,
                  OCIError *errhp, 
                  OCILobLocator *locp)
{
    if (NULL == locp || locp->bOpen == FALSE)
    {
        return EXEC_FAIL;
    }

    if (!locp->bNeedSend)
    {
        return EXEC_SUCCESS;
    }
    /* 将locp中数据写入数据库 */
    SQLRETURN iRet;
    /* 插入数据 */
    char *pSql = locp->pWriteSql;
    fprintf(stdout, "%s\n", pSql);
    iRet = SQLPrepare(serverhstmt, (SQLCHAR *)pSql, SQL_NTS);
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
    /* 每次发送指定大小数据 */
    char *pData = locp->strInfo->data;
    int iSendUnit = 128;
    int iSendLen = iSendUnit;
    int iPos = 0;
    while(iPos < locp->strInfo->len)
    {
        if (locp->strInfo->len - iPos < iSendUnit)
        {
            iSendLen = locp->strInfo->len - iPos;
        }
        else
        {
            iSendLen = iSendUnit;
        }
        iRet = SQLPutData(serverhstmt, pData, iSendLen);
        if (!SQL_SUCCEEDED(iRet))
        {
            fprintf(stderr, "发送数据出错， line %d\n", __LINE__);
            return EXEC_FAIL;
        }
        iPos = iPos + iSendLen;
        pData = pData + iPos;
    }

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

/*
 * OCILobFileSetName
 *
 */

sword OCILobFileSetName(OCIEnv *envhp,
                        OCIError *errhp,
                        OCILobLocator **filepp,
                        const OraText *dir_alias,
                        ub2 d_length, 
                        const OraText *filename,
                        ub2 f_length)
 {
    if (NULL == filepp || NULL == dir_alias || NULL == filename)
    {
        return EXEC_FAIL;
    }
    OCILobLocator *locp = *filepp;
    memset(locp->dir_alias, 0, MAX_PATH_LENGTH);
    strncpy(locp->dir_alias, dir_alias, d_length);
    locp->d_length = d_length;
    memset(locp->filename, 0, MAX_PATH_LENGTH);
    strncpy(locp->filename, filename, f_length);
    locp->f_length = f_length;  

    return EXEC_SUCCESS;
 }


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
    char *pCreateSQL = "create table blob2(id1 int, id2 blob)";
    char *pDropSQL = "drop table if exists blob2";
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
    char *pSql = "INSERT INTO blob2 VALUES (1, ?)";
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
    int blobbuf_size = 128;

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
    char *pSql = "SELECT id1, id2 FROM blob2 WHERE id1 = 1";
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

/*
 * FreeLob
 *
 */

int FreeLob(OCILobLocator *locp)
{
    if (NULL == locp)
    {
        return EXEC_SUCCESS;
    }

    if (NULL != locp->strInfo)
    {
        freeStringInfo(locp->strInfo);
    }

    if (NULL != locp->pReadSql)
    {
        free(locp->pReadSql);
        locp->pReadSql = NULL;
    }

    if (NULL != locp->pWriteSql)
    {
        free(locp->pWriteSql);
        locp->pWriteSql = NULL;
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

    OCISvcCtx *svchp;
    OCIError *errhp;
    OCILobLocator *filep;

    filep = (OCILobLocator *)malloc(sizeof(OCILobLocator));
    memset(filep, 0, sizeof(OCILobLocator));

    /* LOB open */
    if (OCILobFileOpen(svchp, errhp, filep, OCI_LOB_READONLY) != EXEC_SUCCESS)
        return -1;

    /* LOB read */
    char acReadBuf[200] = {0};
    int iReadLen = 112;
    int offset = 55;

    if (OCILobRead(svchp, errhp, filep, &iReadLen, offset, acReadBuf, iReadLen,
                   NULL, NULL, 0, 0) != EXEC_SUCCESS)
    {
        fprintf(stderr, "lob read error\n");
        return -1;
    }
    fprintf(stdout, "read data : %s\n", acReadBuf);

    /* lob write */
    char acWriteBuf[200] = {0};
    memset(acWriteBuf, 'A', 200);
    int iWriteLen = 118;
    offset = 3;
    if (OCILobWrite(svchp, errhp, filep, &iWriteLen, offset, acWriteBuf, iWriteLen,
                    0, NULL, NULL, 0, 0) != EXEC_SUCCESS)
    {
        fprintf(stderr, "lob write error\n");
        return -1;
    }


    /* lob close */
    if (OCILobClose(svchp, errhp, filep) != EXEC_SUCCESS)
    {
        fprintf(stderr, "lob close error\n");
        return -1;
    }

    if (FreeLob(filep) != EXEC_SUCCESS)
    {
        fprintf(stderr, "lob free error\n");
        return -1;
    }

    FreeHandle();

}
