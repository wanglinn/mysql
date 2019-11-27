# SQLColumns 获取表的列的属性

1. 函数定义
```
SQLRETURN SQL_API
SQLColumns(SQLHSTMT hstmt, SQLCHAR *catalog, SQLSMALLINT catalog_len,
           SQLCHAR *schema, SQLSMALLINT schema_len,
           SQLCHAR *table, SQLSMALLINT table_len,
           SQLCHAR *column, SQLSMALLINT column_len)
```
hstmt -- 语句句柄  
catalog -- database名称  
schema -- 未使用  
table -- 表名  
column -- 列名  

2. SQLColumns -> MySQLColumns  
查看函数 MySQLColumns 定义  

```
SQLRETURN SQL_API
MySQLColumns(SQLHSTMT hstmt, SQLCHAR *catalog_name, SQLSMALLINT catalog_len,
             SQLCHAR *schema_name __attribute__((unused)),
             SQLSMALLINT schema_len __attribute__((unused)),
             SQLCHAR *table_name, SQLSMALLINT table_len,
             SQLCHAR *column_name, SQLSMALLINT column_len)
```

3. MySQLColumns -> columns_no_i_s
查看函数 columns_no_i_s 定义  

```
SQLRETURN
columns_no_i_s(STMT * stmt, SQLCHAR *szCatalog, SQLSMALLINT cbCatalog,
               SQLCHAR *szSchema __attribute__((unused)),
               SQLSMALLINT cbSchema __attribute__((unused)),
               SQLCHAR *szTable, SQLSMALLINT cbTable,
               SQLCHAR *szColumn, SQLSMALLINT cbColumn)
```


表明、库名、列名 长度 NAME_LEN 64*3  

获取表的状态先上锁  
myodbc_mutex_lock(&stmt->dbc->lock);  -- 实际对应 pthread_mutex_lock  

3.1 查看表状态函数 table_status  
table_status -> table_status_i_s (拼装查询语句) -> exec_stmt_query  
(set @@sql_select_limit=%lu 或 set @@sql_select_limit=DEFAULT  -- 是否有limit限制)  
-> odbc_stmt (myodbc_mutex_lock(&dbc->lock); -- 加锁) -> mysql_real_query (数据库端API)
-> mysql_store_result (数据库端API)  


mysql_errno(MYSQL *mysql) (数据库端API)  

获取表状态查询结果  
MYSQL_ROW table_row;  
根据表名获取其对应的列属性  

```
  while ((table_row= mysql_fetch_row(res)))
  {
    MYSQL_FIELD *field;
    MYSQL_RES *table_res;
    int count= 0;

    /* Get list of columns matching szColumn for each table. */
    lengths= mysql_fetch_lengths(res);
    table_res= server_list_dbcolumns(stmt, szCatalog, cbCatalog,
                                     (SQLCHAR *)table_row[0],
                                     (SQLSMALLINT)lengths[0],
                                     szColumn, cbColumn);
    ...

```
mysql_fetch_row (数据库端API)  
mysql_fetch_lengths (数据库端API)  
mysql_num_fields (数据库端API)  

server_list_dbcolumns -> mysql_select_db (数据库端API)  -- 获取表的列属性  
mysql_fetch_field (数据库端API)  
mysql_free_result(数据库端API)  



# 参考
mysql_com.h 头文件 （内容应该是来自数据库端，只是一个说明）  
myutil.h 头文件  
mysql.h 头文件  

