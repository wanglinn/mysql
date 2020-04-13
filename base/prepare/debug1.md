# 1 函数
```
void mysql_sql_stmt_prepare(THD *thd);  
void mysql_sql_stmt_execute(THD *thd);  

COM_STMT_PREPARE

send_prep_stmt
Prepared_statement::insert_params
Prepared_statement::emb_insert_params
Prepared_statement::insert_params_from_vars
Sql_cmd_insert_select::prepare
check_prepared_statement

mysqld_stmt_prepare
get_dynamic_sql_string
mysql_sql_stmt_prepare

reinit_stmt_before_use
mysqld_stmt_execute
mysql_sql_stmt_execute
Prepared_statement::Prepared_statement
Prepared_statement::setup_set_params
Prepared_statement::prepare
Prepared_statement::set_parameters
Prepared_statement::execute_server_runnable
Prepared_statement::reprepare()
Prepared_statement::execute

```

# 堆栈信息
```
(gdb) bt
#0  Prepared_statement::prepare (this=0x7fbf480064e0, query_str=0x7fbf4800c1f1 "select id1, id2 from t1 where id1 = ? for update;", query_length=49)
    at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:3211
#1  0x0000000001574398 in mysqld_stmt_prepare (thd=0x7fbf48001960, query=0x7fbf4800c1f1 "select id1, id2 from t1 where id1 = ? for update;", length=49)
    at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:2129
#2  0x000000000153778b in dispatch_command (thd=0x7fbf48001960, com_data=0x7fbf66648df0, command=COM_STMT_PREPARE)
    at /home/wl/software/mysql-5.7.26/sql/sql_parse.cc:1441
#3  0x0000000001536942 in do_command (thd=0x7fbf48001960) at /home/wl/software/mysql-5.7.26/sql/sql_parse.cc:1025
#4  0x0000000001666f42 in handle_connection (arg=0x35a38a0) at /home/wl/software/mysql-5.7.26/sql/conn_handler/connection_handler_per_thread.cc:306
#5  0x0000000001cf22f4 in pfs_spawn_thread (arg=0x3747160) at /home/wl/software/mysql-5.7.26/storage/perfschema/pfs.cc:2190
#6  0x00007fbf720bfdd5 in start_thread () from /lib64/libpthread.so.0
#7  0x00007fbf70d8502d in clone () from /lib64/libc.so.6





(gdb) bt
#0  Prepared_statement::execute (this=0x7fbf480064e0, expanded_query=0x7fbf66648490, open_cursor=false) at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:3820
#1  0x0000000001577a0b in Prepared_statement::execute_loop (this=0x7fbf480064e0, expanded_query=0x7fbf66648490, open_cursor=false, packet=0x7fbf4800c1fa "", 
    packet_end=0x7fbf4800c202 "") at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:3585
#2  0x00000000015753a4 in mysqld_stmt_execute (thd=0x7fbf48001960, stmt_id=1, flags=0, params=0x7fbf4800c1fa "", params_length=8)
    at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:2574
#3  0x00000000015376f3 in dispatch_command (thd=0x7fbf48001960, com_data=0x7fbf66648df0, command=COM_STMT_EXECUTE)
    at /home/wl/software/mysql-5.7.26/sql/sql_parse.cc:1421
#4  0x0000000001536942 in do_command (thd=0x7fbf48001960) at /home/wl/software/mysql-5.7.26/sql/sql_parse.cc:1025
#5  0x0000000001666f42 in handle_connection (arg=0x35a38a0) at /home/wl/software/mysql-5.7.26/sql/conn_handler/connection_handler_per_thread.cc:306
#6  0x0000000001cf22f4 in pfs_spawn_thread (arg=0x3747160) at /home/wl/software/mysql-5.7.26/storage/perfschema/pfs.cc:2190
#7  0x00007fbf720bfdd5 in start_thread () from /lib64/libpthread.so.0
#8  0x00007fbf70d8502d in clone () from /lib64/libc.so.6


```

# 断点函数
```
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000001539ddb in mysql_execute_command(THD*, bool) at /home/wl/software/mysql-5.7.26/sql/sql_parse.cc:2445
        breakpoint already hit 13 times
2       breakpoint     keep y   0x0000000001576c0f in Prepared_statement::prepare(char const*, unsigned long) at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:3211
        breakpoint already hit 1 time
3       breakpoint     keep y   0x00000000015741d6 in mysqld_stmt_prepare(THD*, char const*, unsigned int) at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:2096
4       breakpoint     keep y   0x00000000015785b0 in Prepared_statement::execute(String*, bool) at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:3817
        breakpoint already hit 2 times
5       breakpoint     keep y   0x0000000001575496 in mysql_sql_stmt_execute(THD*) at /home/wl/software/mysql-5.7.26/sql/sql_prepare.cc:2607

Prepared_statement::execute_loop  -- 用来设置参数

0x7fbf4401d31a: ""
0x7fbf4401d31b: ""
0x7fbf4401d31c: "\002"
0x7fbf4401d31e: ""
0x7fbf4401d31f: ""
0x7fbf4401d320: ""
0x7fbf4401d321: "\002t1\002t1\003id1\003id1\f?"
0x7fbf4401d332: "\v"
0x7fbf4401d334: ""
0x7fbf4401d335: ""
(gdb) p packet
$32 = (uchar *) 0x7fbf4401d31a ""
(gdb) p packet_end
$33 = (uchar *) 0x7fbf4401d320 ""

```
