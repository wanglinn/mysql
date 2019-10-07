/* setup :
 *
 *
 * 测试没发现有bug
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

SQLUINTEGER my_fetch_uint(SQLHSTMT hstmt, SQLUSMALLINT icol)
{
    SQLUINTEGER nData;
    SQLLEN len; 

    SQLGetData(hstmt, icol, SQL_C_ULONG, &nData, 0, &len);
    printMessage("my_fetch_int: %ld (%ld)", (long int)nData, len);
    return (len != SQL_NULL_DATA) ? nData : 0; 
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
    ok_sql(hstmt, "drop procedure if exists procedure_columns_test3");

    ok_sql(hstmt, "create procedure procedure_columns_test3(IN re_param_00 datetime,"\
                "OUT re_param_01 date, OUT re_param_02 time, INOUT re_param_03 timestamp,"\
                "re_param_04 year)" \
                "begin end;"
                );

    ok_stmt(hstmt, SQLProcedureColumns(hstmt, NULL, 0, NULL, 0,
                                        "procedure_columns_test3", SQL_NTS,
                                        "%", SQL_NTS));

    typedef struct 
    {
        char *c01_procedure_cat;
        char *c02_procedure_schem;
        char *c03_procedure_name;
        char *c04_column_name;
        short c05_column_type;
        short c06_data_type;
        char *c07_type_name;
        unsigned long c08_column_size;
        unsigned long c09_buffer_length;
        short c10_decimal_digits;
        short c11_num_prec_radix;
        short c12_nullable;
        char *c13_remarks;
        char *c14_column_def;
        short c15_sql_data_type;
        short c16_sql_datetime_sub;
        unsigned long c17_char_octet_length;
        int c18_ordinal_position;
        char *c19_is_nullable;
    }sqlproccol;


    sqlproccol data_to_check[] = {
    //第 1 列对应的结果
    /*cat    schem  proc_name                  col_name       col_type         data_type     type_name */
    {"db1", 0,     "procedure_columns_test3", "re_param_00", SQL_PARAM_INPUT, SQL_TYPE_TIMESTAMP, "datetime",
    /*size buf_len  dec radix  nullable      rem def sql_data_type  sub                  octet pos nullable*/
        19,  16,      0,  10,     SQL_NULLABLE, "", 0, SQL_DATETIME,  SQL_TYPE_TIMESTAMP,  0,    1,  "YES"},

    //第 2 列对应的结果
    /*cat    schem  proc_name                  col_name       col_type          data_type      type_name */
    {"db1", 0,     "procedure_columns_test3", "re_param_01", SQL_PARAM_OUTPUT, SQL_TYPE_DATE, "date",
    /*size buf_len  dec radix  nullable      rem def sql_data_type  sub octet pos nullable*/
        10,  6,       0,  0,     SQL_NULLABLE, "", 0,  SQL_DATETIME,  SQL_TYPE_DATE,  0,    2,  "YES"},

    //第 3 列对应的结果
    /*cat    schem  proc_name                  col_name       col_type          data_type      type_name */
    {"db1", 0,     "procedure_columns_test3", "re_param_02", SQL_PARAM_OUTPUT, SQL_TYPE_TIME, "time",
    /*size buf_len  dec radix  nullable      rem def sql_data_type  sub octet pos nullable*/
        8,   6,       0,  0,     SQL_NULLABLE, "", 0,  SQL_DATETIME,  SQL_TYPE_TIME,  0,    3,  "YES"},

    //第 4 列对应的结果
    /*cat    schem  proc_name                  col_name       col_type                data_type           type_name */
    {"db1", 0,     "procedure_columns_test3", "re_param_03", SQL_PARAM_INPUT_OUTPUT, SQL_TYPE_TIMESTAMP, "timestamp",
    /*size buf_len  dec radix  nullable      rem def sql_data_type sub                  octet pos nullable*/
        19,  16,      0,  0,     SQL_NULLABLE, "", 0,  SQL_DATETIME, SQL_TYPE_TIMESTAMP,  0,    4,  "YES"},

    //第 5 列对应的结果
    /*cat    schem  proc_name                  col_name       col_type         data_type     type_name */
    {"db1", 0,     "procedure_columns_test3", "re_param_04", SQL_PARAM_INPUT, SQL_SMALLINT, "year",
    /*size buf_len  dec radix  nullable      rem def sql_data_type sub octet pos nullable*/
        4,   1,       0,  10,    SQL_NULLABLE, "", 0,  SQL_SMALLINT, 0,  0,    5,  "YES"}
    };

    SQLRETURN rc= 0;
    SQLCHAR szName[255]= {0};
    int total_params= 0, iter= 0;

    while(SQLFetch(hstmt) == SQL_SUCCESS)
    {
        SQLCHAR buff[255] = {0}, *param_cat, *param_name;
        SQLUINTEGER col_size, buf_len, octet_len;

        param_cat= (SQLCHAR*)my_fetch_str(hstmt, buff, 1);
        is_str(param_cat, data_to_check[iter].c01_procedure_cat,
            strlen(data_to_check[iter].c01_procedure_cat) + 1);

        is_str(my_fetch_str(hstmt, buff, 3),
            data_to_check[iter].c03_procedure_name,
            strlen(data_to_check[iter].c03_procedure_name) + 1);

        param_name= (SQLCHAR*)my_fetch_str(hstmt, buff, 4);
        is_str(param_name, data_to_check[iter].c04_column_name,
            strlen(data_to_check[iter].c04_column_name) + 1);

        is_num(my_fetch_int(hstmt, 5), data_to_check[iter].c05_column_type);

        is_num(my_fetch_int(hstmt, 6), data_to_check[iter].c06_data_type);

        is_str(my_fetch_str(hstmt, buff, 7),
            data_to_check[iter].c07_type_name,
            strlen(data_to_check[iter].c07_type_name) + 1);

        col_size= my_fetch_uint(hstmt, 8);
        is_num(col_size, data_to_check[iter].c08_column_size);

        buf_len= my_fetch_uint(hstmt, 9);
        is_num(buf_len, data_to_check[iter].c09_buffer_length);

        is_num(my_fetch_int(hstmt, 10), data_to_check[iter].c10_decimal_digits);

        is_num(my_fetch_int(hstmt, 11), data_to_check[iter].c11_num_prec_radix);

        is_num(my_fetch_int(hstmt, 15), data_to_check[iter].c15_sql_data_type);

        is_num(my_fetch_int(hstmt, 16), data_to_check[iter].c16_sql_datetime_sub);

        octet_len= my_fetch_uint(hstmt, 17);
        is_num(octet_len, data_to_check[iter].c17_char_octet_length);

        is_num(my_fetch_int(hstmt, 18), data_to_check[iter].c18_ordinal_position);

        is_str(my_fetch_str(hstmt, buff, 19),
            data_to_check[iter].c19_is_nullable,
            strlen(data_to_check[iter].c19_is_nullable + 1));

        ++iter;
    }

    ok_stmt(hstmt, SQLFreeStmt(hstmt, SQL_CLOSE));




/************************************ end *************************************/

fprintf(stdout, "finish\n");
FreeHandle();

}



/*

执行结果

[wl@host122 odbc]$ 
[wl@host122 odbc]$ ./procedure3
#  my_fetch_str: db1(3)
#  my_fetch_str: procedure_columns_test3(23)
#  my_fetch_str: re_param_00(11)
# my_fetch_int: 1 (4)
# my_fetch_int: 93 (4)
#  my_fetch_str: datetime(8)
# my_fetch_int: 19 (4)
# my_fetch_int: 16 (4)
# my_fetch_int: 0 (4)
# my_fetch_int: 10 (4)
# my_fetch_int: 9 (4)
# my_fetch_int: 93 (4)
# my_fetch_int: 93 (-1)
# my_fetch_int: 1 (4)
#  my_fetch_str: YES(3)
#  my_fetch_str: db1(3)
#  my_fetch_str: procedure_columns_test3(23)
#  my_fetch_str: re_param_01(11)
# my_fetch_int: 4 (4)
# my_fetch_int: 91 (4)
#  my_fetch_str: date(4)
# my_fetch_int: 10 (4)
# my_fetch_int: 6 (4)
# my_fetch_int: NULL
# my_fetch_int: NULL
# my_fetch_int: 9 (4)
# my_fetch_int: 91 (4)
# my_fetch_int: 91 (-1)
# my_fetch_int: 2 (4)
#  my_fetch_str: YES(3)
#  my_fetch_str: db1(3)
#  my_fetch_str: procedure_columns_test3(23)
#  my_fetch_str: re_param_02(11)
# my_fetch_int: 4 (4)
# my_fetch_int: 92 (4)
#  my_fetch_str: time(4)
# my_fetch_int: 8 (4)
# my_fetch_int: 6 (4)
# my_fetch_int: NULL
# my_fetch_int: NULL
# my_fetch_int: 9 (4)
# my_fetch_int: 92 (4)
# my_fetch_int: 92 (-1)
# my_fetch_int: 3 (4)
#  my_fetch_str: YES(3)
#  my_fetch_str: db1(3)
#  my_fetch_str: procedure_columns_test3(23)
#  my_fetch_str: re_param_03(11)
# my_fetch_int: 2 (4)
# my_fetch_int: 93 (4)
#  my_fetch_str: timestamp(9)
# my_fetch_int: 19 (4)
# my_fetch_int: 16 (4)
# my_fetch_int: NULL
# my_fetch_int: NULL
# my_fetch_int: 9 (4)
# my_fetch_int: 93 (4)
# my_fetch_int: 93 (-1)
# my_fetch_int: 4 (4)
#  my_fetch_str: YES(3)
#  my_fetch_str: db1(3)
#  my_fetch_str: procedure_columns_test3(23)
#  my_fetch_str: re_param_04(11)
# my_fetch_int: 1 (4)
# my_fetch_int: 5 (4)
#  my_fetch_str: year(4)
# my_fetch_int: 4 (4)
# my_fetch_int: 1 (4)
# my_fetch_int: 0 (4)
# my_fetch_int: 10 (4)
# my_fetch_int: 5 (4)
# my_fetch_int: NULL
# my_fetch_int: 5 (-1)
# my_fetch_int: 5 (4)
#  my_fetch_str: YES(3)
finish


*/
