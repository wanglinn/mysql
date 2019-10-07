/* setup :
 *
 *
 *
 * create table simp (id int, name varchar(20));
 * 该程序myql驱动说有bug,测试没发现有bug
 */

#include <assert.h>
#include <sql.h>
#include <sqlext.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_ROW_DATA_LEN 1000


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

/**
  return string data, by fetching it
*/
const char *my_fetch_str(SQLHSTMT hstmt, SQLCHAR *szData,SQLUSMALLINT icol)
{
    SQLLEN nLen;

    SQLGetData(hstmt,icol,SQL_CHAR,szData,MAX_ROW_DATA_LEN+1,&nLen);
    /* If Null value - putting down smth meaningful. also that allows caller to
       better/(in more easy way) test the value */
    if (nLen < 0) 
    {    
      strcpy(szData, "(Null)");
    }    
    printMessage(" my_fetch_str: %s(%ld)", szData, nLen);
    return((const char *)szData);
}

SQLINTEGER my_fetch_int(SQLHSTMT hstmt, SQLUSMALLINT icol)
{
    SQLINTEGER nData;
    SQLLEN len; 

    SQLGetData(hstmt, icol, SQL_INTEGER, &nData, 0, &len);
    if (len == SQL_NULL_DATA)
    {    
      printMessage("my_fetch_int: NULL");
    }    
    else 
    {    
      printMessage("my_fetch_int: %ld (%ld)", (long int)nData, len);
      return nData;
    }    
    return 0;
}


static void print_diag(SQLRETURN rc, SQLSMALLINT htype, SQLHANDLE handle,
                       const char *text, const char *file, int line)
{
  if (rc != SQL_SUCCESS)
  {
    SQLCHAR     sqlstate[6], message[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER  native_error;
    SQLSMALLINT length;
    SQLRETURN   drc; 

    /** @todo map rc to SQL_SUCCESS_WITH_INFO, etc */
    printf("# %s = %d\n", text, rc); 

    /** @todo Handle multiple diagnostic records. */
    drc= SQLGetDiagRec(htype, handle, 1, sqlstate, &native_error,
                       message, SQL_MAX_MESSAGE_LENGTH - 1, &length);

    if (SQL_SUCCEEDED(drc))
      printf("# [%6s] %*s in %s on line %d\n",
             sqlstate, length, message, file, line);
    else 
      printf("# Did not get expected diagnostics from SQLGetDiagRec() = %d"
             " in file %s on line %d\n", drc, file, line);
  }
}


#define is_num(a, b) \
do { \
  long long a1= (a), a2= (b); \
  if (a1 != a2) { \
    printf("# %s (%lld) != %lld in %s on line %d\n", \
           #a, a1, a2, __FILE__, __LINE__); \
    return EXEC_FAIL; \
  } \
} while (0)

#define expect_stmt(statement, call, expect) \
  expect_odbc((statement), SQL_HANDLE_STMT, (call), (expect))


#define expect_odbc(hnd, type, call, expect) \
do { \
  SQLRETURN rc= (call); \
  if (rc != (expect)) \
  { \
    print_diag(rc, (type), (hnd), #call, __FILE__, __LINE__); \
    printf("# Expected %d, but got %d in %s on line %d\n", (expect), rc, \
           __FILE__, __LINE__); \
    return EXEC_FAIL; \
  } \
} while (0)

#define is_str(a, b, c) \ 
do { \
  char *val_a= (char *)(a), *val_b= (char *)(b); \
  int val_len= (int)(c); \
  if (strncmp(val_a, val_b, val_len) != 0) { \
    printf("# %s ('%*s') != '%*s' in %s on line %d\n", \
           #a, val_len, val_a, val_len, val_b, __FILE__, __LINE__); \
    return EXEC_FAIL; \
  } \  
} while (0); 



int main()
{

    SQLRETURN iRet;
 
    if (AllocHandle() != EXEC_SUCCESS)
        return -1;
    if (ConnectDB() != EXEC_SUCCESS)
        return -1;

    void *hstmt = serverhstmt;

/*********************************** start ************************************/

// 测试是没问题的
    SQLLEN nRowCount;
    SQLCHAR buff[24];

    ok_sql(hstmt, "drop procedure if exists bug57182");
    ok_sql(hstmt, "CREATE  PROCEDURE `bug57182`(in id int, in name varchar(20)) "
    "BEGIN"
    "  insert into simp values (id, name);"
    "END");

    ok_stmt(hstmt, SQLProcedureColumns(hstmt, NULL, 0, NULL, 0,
    "bug57182", SQL_NTS, 
    NULL, 0)); 

    ok_stmt(hstmt, SQLRowCount(hstmt, &nRowCount));

    is_num(2, nRowCount);
    fprintf(stdout, "rowcount = %d\n", nRowCount);

    ok_stmt(hstmt, SQLFetch(hstmt));

    is_str(my_fetch_str(hstmt, buff, 3), "bug57182", 9);
    is_str(my_fetch_str(hstmt, buff, 4), "id", 3);
    is_str(my_fetch_str(hstmt, buff, 7), "int", 4);

    ok_stmt(hstmt, SQLFetch(hstmt));

    is_str(my_fetch_str(hstmt, buff, 3), "bug57182", 9);
    is_str(my_fetch_str(hstmt, buff, 4), "name", 5);
    is_str(my_fetch_str(hstmt, buff, 7), "varchar", 8);
    is_num(my_fetch_int(hstmt, 8), 20); 

    expect_stmt(hstmt, SQLFetch(hstmt), SQL_NO_DATA);

    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));

    /* Almost the same thing but with column specified */
    ok_stmt(hstmt, SQLProcedureColumns(hstmt, NULL, 0, NULL, 0,

    "bug57182", SQL_NTS,
    "id", SQL_NTS));

    ok_stmt(hstmt, SQLRowCount(hstmt, &nRowCount));
    is_num(1, nRowCount);

    ok_stmt(hstmt, SQLFetch(hstmt));

    is_str(my_fetch_str(hstmt, buff, 3), "bug57182", 9);
    is_str(my_fetch_str(hstmt, buff, 4), "id", 3);
    is_str(my_fetch_str(hstmt, buff, 7), "int", 4);

    expect_stmt(hstmt, SQLFetch(hstmt), SQL_NO_DATA);

    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));

    /* And testing impossible column condition - expecting to get no rows */
    ok_stmt(hstmt, SQLProcedureColumns(hstmt,  NULL, 0, NULL, 0,
    "bug57182", SQL_NTS,
    "non_existing_column%", SQL_NTS));

    ok_stmt(hstmt, SQLRowCount(hstmt, &nRowCount));
    is_num(0, nRowCount);

    expect_stmt(hstmt, SQLFetch(hstmt), SQL_NO_DATA);

    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));

    ok_sql(hstmt, "drop procedure if exists bug57182");
    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));



/************************************ end *************************************/

    FreeHandle();

}



// 执行结果

[wl@host122 odbc]$ ./procedure2
rowcount = 2
#  my_fetch_str: bug57182(8)
#  my_fetch_str: id(2)
#  my_fetch_str: int(3)
#  my_fetch_str: bug57182(8)
#  my_fetch_str: name(4)
#  my_fetch_str: varchar(7)
# my_fetch_int: 20 (4)
#  my_fetch_str: bug57182(8)
#  my_fetch_str: id(2)
