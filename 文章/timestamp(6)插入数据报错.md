# 1.timestamp(6) 插入数据报Incorrect datetime value
原因分析：
插入的数据不在数据类型 timestamp(6)范围内。  

**问题:timestamp(6)允许的范围是多少呢？和时区有没有关系？**

* 允许的范围：'1970-01-01 00:00:01' UTC 至 '2038-01-19 03:14:07' UTC  
  该数据范围是对时区0来说的，对东八区则数值加8hours

* 和时区有关系,当MySQL参数time_zone=system时，查询timestamp字段会调用系统时区做时区转换，  
而由于系统时区存在全局锁问题，在多并发大数据量访问时会导致线程上下文频繁切换，CPU使用率暴涨，  
系统响应变慢设置假死。  

查看时区  
```
[wl@host122 ~]$ timedatectl   
      Local time: Sat 2020-08-29 07:04:18 CST  
  Universal time: Fri 2020-08-28 23:04:18 UTC  
        RTC time: Fri 2020-08-28 23:04:17
       Time zone: Asia/Shanghai (CST, +0800)
     NTP enabled: no
NTP synchronized: yes
 RTC in local TZ: no
      DST active: n/a

mysql> insert into t3 values(1, '1970-01-01 08:00:00.0000');
ERROR 1292 (22007): Incorrect datetime value: '1970-01-01 08:00:00.0000' for column 'id2' at row 1
mysql> insert into t3 values(1, '1970-01-01 08:00:01.0000');
Query OK, 1 row affected (0.00 sec)

mysql> select * from t3;
+------+----------------------------+
| id1  | id2                        |
+------+----------------------------+
|    1 | 1970-01-01 08:00:01.000000 |
+------+----------------------------+
1 row in set (0.00 sec)

在线更改时区
mysql> show variables like '%time_zone%'; 
+------------------+--------+
| Variable_name    | Value  |
+------------------+--------+
| system_time_zone | CST    |
| time_zone        | SYSTEM |
+------------------+--------+
2 rows in set (0.01 sec)

mysql> set time_zone = '+0:00';
Query OK, 0 rows affected (0.00 sec)

mysql> insert into t3 values(1, '1970-01-01 00:00:00.0000');
ERROR 1292 (22007): Incorrect datetime value: '1970-01-01 00:00:00.0000' for column 'id2' at row 1
mysql> insert into t3 values(1, '1970-01-01 00:00:01.0000');
Query OK, 1 row affected (0.01 sec)

mysql> select * from t3;
+------+----------------------------+
| id1  | id2                        |
+------+----------------------------+
|    1 | 1970-01-01 00:00:01.000000 |
|    1 | 1970-01-01 00:00:01.000000 |
+------+----------------------------+
2 rows in set (0.00 sec)


mysql> select now();
+---------------------+
| now()               |
+---------------------+
| 2020-08-28 23:11:15 |
+---------------------+
1 row in set (0.00 sec)

```
mysql 报错非常不满意的是：  
很对报错提示太过笼统，比如上面的报错，可以提示时区及范围。  
即使在错误日志中也不提示出错的行数及在哪个文件中出错。  

# 2.源码分析报错原因

针对如下插入数据报错  
mysql> insert into t3 values(1, '1970-01-01 00:00:01.0000');  
ERROR 1292 (22007): Incorrect datetime value: '1970-01-01 00:00:01.0000' for column 'id2' at row 1  

报错函数为 my_system_gmt_sec  
该函数解释为：  
  Convert time in MYSQL_TIME representation in system time zone to its  
  my_time_t form (number of seconds in UTC since begginning of Unix Epoch).  
假设让设计个该函数思路为：
以 1970-01-01 00:00:00 为起点，计算给定的value 相对起点的数值并算上时区及本地时间的影响，若超过
最大值则报错。

堆栈信息  
```
#0  validate_timestamp_range (t=0x7fb0f685baa0) at /home/user1/software/mysql-5.7.26/include/my_time.h:184
#1  0x00000000016607a0 in my_system_gmt_sec (t_src=0x7fb0f685bcb0, my_timezone=0x7fb0f685bb38, in_dst_time_gap=0x7fb0f685bbbf "")
    at /home/user1/software/mysql-5.7.26/sql-common/my_time.c:1032
#2  0x0000000001641937 in Time_zone_system::TIME_to_gmt_sec (this=0x2d3ebe8 <tz_SYSTEM>, t=0x7fb0f685bcb0, in_dst_time_gap=0x7fb0f685bbbf "")
    at /home/user1/software/mysql-5.7.26/sql/tztime.cc:1068
#3  0x00000000015dacbf in TIME_to_timestamp (thd=0x7fb0d8001960, t=0x7fb0f685bcb0, in_dst_time_gap=0x7fb0f685bbbf "")
    at /home/user1/software/mysql-5.7.26/sql/sql_time.cc:625
#4  0x00000000015dad97 in datetime_with_no_zero_in_date_to_timeval (thd=0x7fb0d8001960, ltime=0x7fb0f685bcb0, tm=0x7fb0f685bc30, warnings=0x7fb0f685bce0)
    at /home/user1/software/mysql-5.7.26/sql/sql_time.cc:686
#5  0x0000000000eff4ba in Field_temporal_with_date_and_time::convert_TIME_to_timestamp (this=0x7fb0d801ec60, thd=0x7fb0d8001960, ltime=0x7fb0f685bcb0, 
    tm=0x7fb0f685bc30, warnings=0x7fb0f685bce0) at /home/user1/software/mysql-5.7.26/sql/field.cc:5591
#6  0x0000000000f00015 in Field_timestampf::store_internal (this=0x7fb0d801ec60, ltime=0x7fb0f685bcb0, warnings=0x7fb0f685bce0)
    at /home/user1/software/mysql-5.7.26/sql/field.cc:5942
#7  0x0000000000eff1af in Field_temporal_with_date::store_internal_with_round (this=0x7fb0d801ec60, ltime=0x7fb0f685bcb0, warnings=0x7fb0f685bce0)
    at /home/user1/software/mysql-5.7.26/sql/field.cc:5527
#8  0x0000000000efe28d in Field_temporal::store (this=0x7fb0d801ec60, str=0x7fb0d8011540 "1900-01-01 00:00:01 00000", len=25, 
    cs=0x2d070e0 <my_charset_utf8mb4_general_ci>) at /home/user1/software/mysql-5.7.26/sql/field.cc:5273
#9  0x0000000000f48e09 in Item::save_str_value_in_field (this=0x7fb0d8011560, field=0x7fb0d801ec60, result=0x7fb0d8011570)
    at /home/user1/software/mysql-5.7.26/sql/item.cc:561
#10 0x0000000000f5aee2 in Item_string::save_in_field_inner (this=0x7fb0d8011560, field=0x7fb0d801ec60, no_conversions=false)
    at /home/user1/software/mysql-5.7.26/sql/item.cc:6900
#11 0x0000000000f5a48c in Item::save_in_field (this=0x7fb0d8011560, field=0x7fb0d801ec60, no_conversions=false) at /home/user1/software/mysql-5.7.26/sql/item.cc:6757
#12 0x00000000014c31ee in fill_record (thd=0x7fb0d8001960, table=0x7fb0d801d600, ptr=0x7fb0d801eb70, values=..., bitmap=0x0, insert_into_fields_bitmap=0x0)
    at /home/user1/software/mysql-5.7.26/sql/sql_base.cc:9653
#13 0x00000000014c34c3 in fill_record_n_invoke_before_triggers (thd=0x7fb0d8001960, ptr=0x7fb0d801eb60, values=..., table=0x7fb0d801d600, event=TRG_EVENT_INSERT, 
    num_fields=2) at /home/user1/software/mysql-5.7.26/sql/sql_base.cc:9735
#14 0x000000000175dec4 in Sql_cmd_insert::mysql_insert (this=0x7fb0d8011cc8, thd=0x7fb0d8001960, table_list=0x7fb0d80116e8)
    at /home/user1/software/mysql-5.7.26/sql/sql_base.cc:9653
#13 0x00000000014c34c3 in fill_record_n_invoke_before_triggers (thd=0x7fb0d8001960, ptr=0x7fb0d801eb60, values=..., table=0x7fb0d801d600, event=TRG_EVENT_INSERT, 
    num_fields=2) at /home/user1/software/mysql-5.7.26/sql/sql_base.cc:9735
#14 0x000000000175dec4 in Sql_cmd_insert::mysql_insert (this=0x7fb0d8011cc8, thd=0x7fb0d8001960, table_list=0x7fb0d80116e8)
    at /home/user1/software/mysql-5.7.26/sql/sql_insert.cc:751
#15 0x000000000176484f in Sql_cmd_insert::execute (this=0x7fb0d8011cc8, thd=0x7fb0d8001960) at /home/user1/software/mysql-5.7.26/sql/sql_insert.cc:3118
#16 0x000000000153c891 in mysql_execute_command (thd=0x7fb0d8001960, first_level=true) at /home/user1/software/mysql-5.7.26/sql/sql_parse.cc:3596
#17 0x0000000001541fc3 in mysql_parse (thd=0x7fb0d8001960, parser_state=0x7fb0f685d690) at /home/user1/software/mysql-5.7.26/sql/sql_parse.cc:5577
#18 0x0000000001537a0e in dispatch_command (thd=0x7fb0d8001960, com_data=0x7fb0f685ddf0, command=COM_QUERY) at /home/user1/software/mysql-5.7.26/sql/sql_parse.cc:1484
#19 0x0000000001536942 in do_command (thd=0x7fb0d8001960) at /home/user1/software/mysql-5.7.26/sql/sql_parse.cc:1025
#20 0x0000000001666f42 in handle_connection (arg=0x4ed5940) at /home/user1/software/mysql-5.7.26/sql/conn_handler/connection_handler_per_thread.cc:306
#21 0x0000000001cf22f4 in pfs_spawn_thread (arg=0x5049810) at /home/user1/software/mysql-5.7.26/storage/perfschema/pfs.cc:2190
#22 0x00007fb1024f4dd5 in start_thread () from /lib64/libpthread.so.0
#23 0x00007fb1011ba02d in clone () from /lib64/libc.so.6

(gdb) p *t
$124 = {
  year = 1900, 
  month = 1, 
  day = 1, 
  hour = 0, 
  minute = 0, 
  second = 1, 
  second_part = 0, 
  neg = 0 '\000', 
  time_type = MYSQL_TIMESTAMP_DATETIME
```
