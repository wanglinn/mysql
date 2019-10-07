/* setup :
 * 获取date类型的变量，执行结果不对
 *
 *
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

    SQL_DATE_STRUCT a,b,c,d;
    SQLLEN len1;

    ok_sql(hstmt, "DROP TABLE IF EXISTS t_bug15773");
    ok_sql(hstmt, "CREATE TABLE t_bug15773("
            "`a` varchar(255) NOT NULL default ''," 
            "`b` datetime,"
            "`c` datetime)");

    ok_sql(hstmt, "INSERT INTO t_bug15773 VALUES ('a', '2005-12-24 00:00:00', '2008-05-12 00:00:00')");
    ok_sql(hstmt, "INSERT INTO t_bug15773 VALUES ('b', '2004-01-01 00:00:00', '2005-01-01 00:00:00')");
    ok_sql(hstmt, "INSERT INTO t_bug15773 VALUES ('c', '2004-12-12 00:00:00', '2005-12-12 00:00:00')");

#if 0
    ok_stmt(hstmt, SQLPrepare(hstmt, (SQLCHAR *)"SELECT * FROM t_bug15773"
                            " WHERE (?) BETWEEN b AND c", SQL_NTS));

    d.day= 15;
    d.month= 12;
    d.year = 2005;

    ok_stmt(hstmt, SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_TYPE_DATE,
                                    SQL_TYPE_DATE, 0, 0, &d, 0, NULL));

    ok_stmt(hstmt, SQLBindCol(hstmt, 1, SQL_C_CHAR, &a, 255, &len1));
    ok_stmt(hstmt, SQLBindCol(hstmt, 2, SQL_C_TYPE_DATE, &b, 0, &len1));
    ok_stmt(hstmt, SQLBindCol(hstmt, 3, SQL_C_TYPE_DATE, &c, 0, &len1));
#endif

    ok_stmt(hstmt, SQLPrepare(hstmt, (SQLCHAR *)"SELECT * FROM t_bug15773"
                         , SQL_NTS));

    ok_stmt(hstmt, SQLBindCol(hstmt, 1, SQL_C_CHAR, &a, 255, &len1));
    ok_stmt(hstmt, SQLBindCol(hstmt, 2, SQL_C_TYPE_DATE, &b, 0, &len1));
    ok_stmt(hstmt, SQLBindCol(hstmt, 3, SQL_C_TYPE_DATE, &c, 0, &len1));

    ok_stmt(hstmt, SQLExecute(hstmt));

    //expect_stmt(hstmt, SQLFetch(hstmt), SQL_NO_DATA_FOUND);

    fprintf(stdout, "year = %d, mm = %d, day = %d\n", b.year, b.month, b.day);

    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_UNBIND));
    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));

    //ok_sql(hstmt, "DROP TABLE IF EXISTS t_bug15773");


/************************************ end *************************************/

    FreeHandle();

}



/*

执行结果

[wl@host122 odbc]$ ./date1 
year = 5344, mm = 64, day = 0

查看表

mysql> select * from t_bug15773;
+---+---------------------+---------------------+
| a | b                   | c                   |
+---+---------------------+---------------------+
| a | 2005-12-24 00:00:00 | 2008-05-12 00:00:00 |
| b | 2004-01-01 00:00:00 | 2005-01-01 00:00:00 |
| c | 2004-12-12 00:00:00 | 2005-12-12 00:00:00 |
+---+---------------------+---------------------+
3 rows in set (0.00 sec)

*/
