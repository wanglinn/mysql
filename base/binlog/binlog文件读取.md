# binlog 文件作用
mysql提供了binlog功能，任何对数据库的增删改操作及DDL操作都会记录在binlog中；数据库的主备复制当前（5.7、8.0版本）也是基于binlog进行的。  

# 提出问题
1. binlog可以理解为记录数据库操作语句的存储的文件，执行的语句有多种类别，是否记录也分为不同的类别进行记录？  
2. binlog 记录事件的格式分为哪几种，有什么区别？  
3. 记录事件时怎么来获取该事件对应的长度？是否有结束标志类似的信息还是有记录长度的信息？  
4. 如何根据binlog信息进行回滚操作？ 可以根据insert、update、delete、truncate 进行分别说明
5. binlog 写记录是在什么时候进行的？ binlog 、 undo 、 redo 、表数据文件 这几个顺序?

# binlog 涉及的开关及格式
log_bin                   -- binlog是否开启开关  
binlog_format             -- binlog格式：statement, row, mixed  
log_bin_basename          -- binlog名称前缀  
log_bin_index             -- 记录数据库binlog文件名称的文件  
max_binlog_cache_size     -- binlog可以使用最大缓存的大小  
max_binlog_size           -- binlog物理文件大小  
sync_binlog               -- 控制binlog缓存刷盘的频率：0 表示由文件系统控制刷盘  
                             1 表示每次事务提交都会刷盘，这样最安全不会丢失事务， 该参数很影响性能  
binlog_group_commit_sync_delay   -- 等待多长时间后才进行组提交，该参数可能影响性能  
还有很多binlog参数，不再列出。

# statement 格式下的binlog内容
读取binlog文件的内容可以通过以下两种方式：  
1. 通过mysqlbinlog 可执行文件进行读取  
2. 通过mysql提供的sql命令 show binlog events in 'mysql-bin.000117' 进行读取

mysqlbinlog  --no-defaults --help 查看使用方式  
主要使用的参数有：  
--skip-gtids  
--start-position=  
--stop-position=    
--verbose  
--include-gtids=name  
--exclude-gtids=name   

mysqlbinlog --no-defaults mysql-bin.000117 > 1.txt  

```
mysql> show variables like '%binlog_format%';
+---------------+-----------+
| Variable_name | Value     |
+---------------+-----------+
| binlog_format | STATEMENT |
+---------------+-----------+
1 row in set (0.01 sec)


[wl@host122 data]$ cat 1.txt 4
/*!50530 SET @@SESSION.PSEUDO_SLAVE_MODE=1*/;
/*!50003 SET @OLD_COMPLETION_TYPE=@@COMPLETION_TYPE,COMPLETION_TYPE=0*/;
DELIMITER /*!*/;
# at 4
#210204 20:47:39 server id 1  end_log_pos 123 CRC32 0x9f0743fa  Start: binlog v 4, server v 5.7.26-debug-log created 210204 20:47:39
# Warning: this binlog is either in use or was not closed properly.
BINLOG '
6+wbYA8BAAAAdwAAAHsAAAABAAQANS43LjI2LWRlYnVnLWxvZwAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAEzgNAAgAEgAEBAQEEgAAXwAEGggAAAAICAgCAAAACgoKKioAEjQA
AfpDB58=
'/*!*/;
# at 123
#210204 20:47:39 server id 1  end_log_pos 154 CRC32 0x7e9b0f9d  Previous-GTIDs
# [empty]
# at 154
#210204 20:47:43 server id 1  end_log_pos 219 CRC32 0x9b46cfba  Anonymous_GTID  last_committed=0        sequence_number=1       rbr_only=no
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 219
#210204 20:47:43 server id 1  end_log_pos 297 CRC32 0x12de1cce  Query   thread_id=2     exec_time=0     error_code=0
use `db1`/*!*/;
SET TIMESTAMP=1612442863/*!*/;
SET @@session.pseudo_thread_id=2/*!*/;
SET @@session.foreign_key_checks=1, @@session.sql_auto_is_null=0, @@session.unique_checks=1, @@session.autocommit=1/*!*/;
SET @@session.sql_mode=1436549152/*!*/;
SET @@session.auto_increment_increment=1, @@session.auto_increment_offset=1/*!*/;
/*!\C utf8mb4 *//*!*/;
SET @@session.character_set_client=45,@@session.collation_connection=45,@@session.collation_server=45/*!*/;
SET @@session.lc_time_names=0/*!*/;
SET @@session.collation_database=DEFAULT/*!*/;
flush tables
/*!*/;
# at 297
#210215 16:00:32 server id 1  end_log_pos 362 CRC32 0x3aca370c  Anonymous_GTID  last_committed=1        sequence_number=2       rbr_only=no
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 362
#210215 16:00:32 server id 1  end_log_pos 439 CRC32 0x9c93e331  Query   thread_id=2859  exec_time=0     error_code=0
SET TIMESTAMP=1613376032/*!*/;
BEGIN
/*!*/;
# at 439
#210215 16:00:32 server id 1  end_log_pos 541 CRC32 0x998b521c  Query   thread_id=2859  exec_time=0     error_code=0
SET TIMESTAMP=1613376032/*!*/;
insert into t1 values(101,101)
/*!*/;
# at 541
#210215 16:00:32 server id 1  end_log_pos 572 CRC32 0xf92d923c  Xid = 66
COMMIT/*!*/;
# at 572
#210215 16:05:59 server id 1  end_log_pos 637 CRC32 0x70da93f9  Anonymous_GTID  last_committed=2        sequence_number=3       rbr_only=no
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 637
#210215 16:05:59 server id 1  end_log_pos 711 CRC32 0x8ad74716  Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376359/*!*/;
BEGIN
/*!*/;
# at 711
#210215 16:05:59 server id 1  end_log_pos 814 CRC32 0x9d541cfa  Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376359/*!*/;
insert into db1.t1 values(102,102)
/*!*/;
# at 814
#210215 16:05:59 server id 1  end_log_pos 845 CRC32 0x7e646a22  Xid = 84
COMMIT/*!*/;
# at 845
#210215 16:07:13 server id 1  end_log_pos 910 CRC32 0xbf90742e  Anonymous_GTID  last_committed=3        sequence_number=4       rbr_only=no
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 910
#210215 16:06:47 server id 1  end_log_pos 984 CRC32 0x76738025  Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376407/*!*/;
BEGIN
/*!*/;
# at 984
#210215 16:06:47 server id 1  end_log_pos 1087 CRC32 0x58d9408a         Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376407/*!*/;
insert into db1.t1 values(103,103)
/*!*/;
# at 1087
#210215 16:06:50 server id 1  end_log_pos 1165 CRC32 0x5ad4a866         Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376410/*!*/;
SAVEPOINT `sp1`
/*!*/;
# at 1165
#210215 16:07:13 server id 1  end_log_pos 1196 CRC32 0xc181e7c8         Xid = 87
COMMIT/*!*/;
# at 1196
#210215 16:08:05 server id 1  end_log_pos 1261 CRC32 0x8ef0d74c         Anonymous_GTID  last_committed=4        sequence_number=5       rbr_only=no
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 1261
#210215 16:07:35 server id 1  end_log_pos 1335 CRC32 0xcab1422f         Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376455/*!*/;
BEGIN
/*!*/;
# at 1335
#210215 16:07:35 server id 1  end_log_pos 1438 CRC32 0x9eb2f1aa         Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376455/*!*/;
insert into db1.t1 values(104,104)
/*!*/;
# at 1438
#210215 16:07:41 server id 1  end_log_pos 1516 CRC32 0x5b0bc6b5         Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376461/*!*/;
SAVEPOINT `sp1`
/*!*/;
# at 1516
#210215 16:07:45 server id 1  end_log_pos 1619 CRC32 0x5b277cec         Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376465/*!*/;
insert into db1.t1 values(105,105)
/*!*/;
# at 1619
#210215 16:07:50 server id 1  end_log_pos 1697 CRC32 0x5e9e2a85         Query   thread_id=2861  exec_time=0     error_code=0
SET TIMESTAMP=1613376470/*!*/;
SAVEPOINT `sp2`
/*!*/;
# at 1697
#210215 16:08:05 server id 1  end_log_pos 1728 CRC32 0x156b7de6         Xid = 96
COMMIT/*!*/;
# at 1728
#210215 17:20:11 server id 1  end_log_pos 1793 CRC32 0x8f56255a         Anonymous_GTID  last_committed=5        sequence_number=6       rbr_only=no
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 1793
#210215 17:20:11 server id 1  end_log_pos 1870 CRC32 0xa26c7f2b         Query   thread_id=2863  exec_time=0     error_code=0
SET TIMESTAMP=1613380811/*!*/;
BEGIN
/*!*/;
# at 1870
#210215 17:20:11 server id 1  end_log_pos 1972 CRC32 0xf89a1241         Query   thread_id=2863  exec_time=0     error_code=0
use `db1`/*!*/;
SET TIMESTAMP=1613380811/*!*/;
insert into t1 values(120,120)
/*!*/;
# at 1972
#210215 17:20:11 server id 1  end_log_pos 2003 CRC32 0xfe127e27         Xid = 137
COMMIT/*!*/;
SET @@SESSION.GTID_NEXT= 'AUTOMATIC' /* added by mysqlbinlog */ /*!*/;
DELIMITER ;
# End of log file
/*!50003 SET COMPLETION_TYPE=@OLD_COMPLETION_TYPE*/;
/*!50530 SET @@SESSION.PSEUDO_SLAVE_MODE=0*/;

```
通过show binglog events 查看  

```
mysql> show binlog events in 'mysql-bin.000117';
+------------------+------+----------------+-----------+-------------+---------------------------------------------+
| Log_name         | Pos  | Event_type     | Server_id | End_log_pos | Info                                        |
+------------------+------+----------------+-----------+-------------+---------------------------------------------+
| mysql-bin.000117 |    4 | Format_desc    |         1 |         123 | Server ver: 5.7.26-debug-log, Binlog ver: 4 |
| mysql-bin.000117 |  123 | Previous_gtids |         1 |         154 |                                             |
| mysql-bin.000117 |  154 | Anonymous_Gtid |         1 |         219 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000117 |  219 | Query          |         1 |         297 | use `db1`; flush tables                     |
| mysql-bin.000117 |  297 | Anonymous_Gtid |         1 |         362 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000117 |  362 | Query          |         1 |         439 | BEGIN                                       |
| mysql-bin.000117 |  439 | Query          |         1 |         541 | use `db1`; insert into t1 values(101,101)   |
| mysql-bin.000117 |  541 | Xid            |         1 |         572 | COMMIT /* xid=66 */                         |
| mysql-bin.000117 |  572 | Anonymous_Gtid |         1 |         637 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000117 |  637 | Query          |         1 |         711 | BEGIN                                       |
| mysql-bin.000117 |  711 | Query          |         1 |         814 | insert into db1.t1 values(102,102)          |
| mysql-bin.000117 |  814 | Xid            |         1 |         845 | COMMIT /* xid=84 */                         |
| mysql-bin.000117 |  845 | Anonymous_Gtid |         1 |         910 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000117 |  910 | Query          |         1 |         984 | BEGIN                                       |
| mysql-bin.000117 |  984 | Query          |         1 |        1087 | insert into db1.t1 values(103,103)          |
| mysql-bin.000117 | 1087 | Query          |         1 |        1165 | SAVEPOINT `sp1`                             |
| mysql-bin.000117 | 1165 | Xid            |         1 |        1196 | COMMIT /* xid=87 */                         |
| mysql-bin.000117 | 1196 | Anonymous_Gtid |         1 |        1261 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000117 | 1261 | Query          |         1 |        1335 | BEGIN                                       |
| mysql-bin.000117 | 1335 | Query          |         1 |        1438 | insert into db1.t1 values(104,104)          |
| mysql-bin.000117 | 1438 | Query          |         1 |        1516 | SAVEPOINT `sp1`                             |
| mysql-bin.000117 | 1516 | Query          |         1 |        1619 | insert into db1.t1 values(105,105)          |
| mysql-bin.000117 | 1619 | Query          |         1 |        1697 | SAVEPOINT `sp2`                             |
| mysql-bin.000117 | 1697 | Xid            |         1 |        1728 | COMMIT /* xid=96 */                         |
| mysql-bin.000117 | 1728 | Anonymous_Gtid |         1 |        1793 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000117 | 1793 | Query          |         1 |        1870 | BEGIN                                       |
| mysql-bin.000117 | 1870 | Query          |         1 |        1972 | use `db1`; insert into t1 values(120,120)   |
| mysql-bin.000117 | 1972 | Xid            |         1 |        2003 | COMMIT /* xid=137 */                        |
+------------------+------+----------------+-----------+-------------+---------------------------------------------+
28 rows in set (0.01 sec)
```
# binlog 格式为row

```
mysql> show variables like '%binlog_format';
+---------------+-------+
| Variable_name | Value |
+---------------+-------+
| binlog_format | ROW   |
+---------------+-------+
1 row in set (0.00 sec)

mysql> use  db1;
Reading table information for completion of table and column names
You can turn off this feature to get a quicker startup with -A

Database changed
mysql> truncate t1;
Query OK, 0 rows affected (0.02 sec)

mysql> insert into t1 values(1,1);
Query OK, 1 row affected (0.01 sec)

mysql> show binlog events in 'mysql-bin.000118';
+------------------+-----+----------------+-----------+-------------+---------------------------------------------+
| Log_name         | Pos | Event_type     | Server_id | End_log_pos | Info                                        |
+------------------+-----+----------------+-----------+-------------+---------------------------------------------+
| mysql-bin.000118 |   4 | Format_desc    |         1 |         123 | Server ver: 5.7.26-debug-log, Binlog ver: 4 |
| mysql-bin.000118 | 123 | Previous_gtids |         1 |         154 |                                             |
| mysql-bin.000118 | 154 | Anonymous_Gtid |         1 |         219 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000118 | 219 | Query          |         1 |         296 | use `db1`; truncate t1                      |
| mysql-bin.000118 | 296 | Anonymous_Gtid |         1 |         361 | SET @@SESSION.GTID_NEXT= 'ANONYMOUS'        |
| mysql-bin.000118 | 361 | Query          |         1 |         432 | BEGIN                                       |
| mysql-bin.000118 | 432 | Table_map      |         1 |         477 | table_id: 114 (db1.t1)                      |
| mysql-bin.000118 | 477 | Write_rows     |         1 |         521 | table_id: 114 flags: STMT_END_F             |
| mysql-bin.000118 | 521 | Xid            |         1 |         552 | COMMIT /* xid=15 */                         |
+------------------+-----+----------------+-----------+-------------+---------------------------------------------+
9 rows in set (0.00 sec)

后面又执行了一些操作...

[wl@host122 data]$ mysqlbinlog --no-defaults --base64-output=decode-rows  mysql-bin.000118 -v
/*!50530 SET @@SESSION.PSEUDO_SLAVE_MODE=1*/;
/*!50003 SET @OLD_COMPLETION_TYPE=@@COMPLETION_TYPE,COMPLETION_TYPE=0*/;
DELIMITER /*!*/;
# at 4
#210216 12:46:45 server id 1  end_log_pos 123 CRC32 0x2056db8d  Start: binlog v 4, server v 5.7.26-debug-log created 210216 12:46:45 at startup
# Warning: this binlog is either in use or was not closed properly.
ROLLBACK/*!*/;
# at 123
#210216 12:46:45 server id 1  end_log_pos 154 CRC32 0x42c28e90  Previous-GTIDs
# [empty]
# at 154
#210216 12:47:03 server id 1  end_log_pos 219 CRC32 0x355f8700  Anonymous_GTID  last_committed=0        sequence_number=1       rbr_only=no
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 219
#210216 12:47:03 server id 1  end_log_pos 296 CRC32 0x1fcc0cdb  Query   thread_id=2     exec_time=0     error_code=0
use `db1`/*!*/;
SET TIMESTAMP=1613450823/*!*/;
SET @@session.pseudo_thread_id=2/*!*/;
SET @@session.foreign_key_checks=1, @@session.sql_auto_is_null=0, @@session.unique_checks=1, @@session.autocommit=1/*!*/;
SET @@session.sql_mode=1436549152/*!*/;
SET @@session.auto_increment_increment=1, @@session.auto_increment_offset=1/*!*/;
/*!\C utf8mb4 *//*!*/;
SET @@session.character_set_client=45,@@session.collation_connection=45,@@session.collation_server=45/*!*/;
SET @@session.lc_time_names=0/*!*/;
SET @@session.collation_database=DEFAULT/*!*/;
truncate t1
/*!*/;
# at 296
#210216 12:47:09 server id 1  end_log_pos 361 CRC32 0x6a88fd11  Anonymous_GTID  last_committed=1        sequence_number=2       rbr_only=yes
/*!50718 SET TRANSACTION ISOLATION LEVEL READ COMMITTED*//*!*/;
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 361
#210216 12:47:09 server id 1  end_log_pos 432 CRC32 0x4ade8664  Query   thread_id=2     exec_time=0     error_code=0
SET TIMESTAMP=1613450829/*!*/;
BEGIN
/*!*/;
# at 432
#210216 12:47:09 server id 1  end_log_pos 477 CRC32 0xd2c00cf0  Table_map: `db1`.`t1` mapped to number 114
# at 477
#210216 12:47:09 server id 1  end_log_pos 521 CRC32 0x32810d99  Write_rows: table id 114 flags: STMT_END_F
### INSERT INTO `db1`.`t1`
### SET
###   @1=1
###   @2=1
# at 521
#210216 12:47:09 server id 1  end_log_pos 552 CRC32 0x23be7c04  Xid = 15
COMMIT/*!*/;
# at 552
#210216 12:54:02 server id 1  end_log_pos 617 CRC32 0x6520e86f  Anonymous_GTID  last_committed=2        sequence_number=3       rbr_only=yes
/*!50718 SET TRANSACTION ISOLATION LEVEL READ COMMITTED*//*!*/;
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 617
#210216 12:54:02 server id 1  end_log_pos 688 CRC32 0xc9b3241a  Query   thread_id=3     exec_time=0     error_code=0
SET TIMESTAMP=1613451242/*!*/;
BEGIN
/*!*/;
# at 688
#210216 12:54:02 server id 1  end_log_pos 733 CRC32 0x341df0e3  Table_map: `db1`.`t1` mapped to number 114
# at 733
#210216 12:54:02 server id 1  end_log_pos 787 CRC32 0x54890e68  Update_rows: table id 114 flags: STMT_END_F
### UPDATE `db1`.`t1`
### WHERE
###   @1=1
###   @2=1
### SET
###   @1=2
###   @2=1
# at 787
#210216 12:54:02 server id 1  end_log_pos 818 CRC32 0xedab9413  Xid = 30
COMMIT/*!*/;
# at 818
#210216 12:54:37 server id 1  end_log_pos 883 CRC32 0x58740890  Anonymous_GTID  last_committed=3        sequence_number=4       rbr_only=yes
/*!50718 SET TRANSACTION ISOLATION LEVEL READ COMMITTED*//*!*/;
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 883
#210216 12:54:37 server id 1  end_log_pos 954 CRC32 0xb5d5754b  Query   thread_id=4     exec_time=0     error_code=0
SET TIMESTAMP=1613451277/*!*/;
BEGIN
/*!*/;
# at 954
#210216 12:54:37 server id 1  end_log_pos 999 CRC32 0x47a45fe0  Table_map: `db1`.`t1` mapped to number 114
# at 999
#210216 12:54:37 server id 1  end_log_pos 1070 CRC32 0x376bc03f         Write_rows: table id 114 flags: STMT_END_F
### INSERT INTO `db1`.`t1`
### SET
###   @1=2
###   @2=2
### INSERT INTO `db1`.`t1`
### SET
###   @1=3
###   @2=3
### INSERT INTO `db1`.`t1`
### SET
###   @1=4
###   @2=4
### INSERT INTO `db1`.`t1`
### SET
###   @1=5
###   @2=5
# at 1070
#210216 12:54:37 server id 1  end_log_pos 1101 CRC32 0x9eda2748         Xid = 44
COMMIT/*!*/;
# at 1101
#210216 12:54:53 server id 1  end_log_pos 1166 CRC32 0xe32275e5         Anonymous_GTID  last_committed=4        sequence_number=5       rbr_only=yes
/*!50718 SET TRANSACTION ISOLATION LEVEL READ COMMITTED*//*!*/;
SET @@SESSION.GTID_NEXT= 'ANONYMOUS'/*!*/;
# at 1166
#210216 12:54:53 server id 1  end_log_pos 1237 CRC32 0xfb0640f2         Query   thread_id=4     exec_time=0     error_code=0
SET TIMESTAMP=1613451293/*!*/;
BEGIN
/*!*/;
# at 1237
#210216 12:54:53 server id 1  end_log_pos 1282 CRC32 0x27505936         Table_map: `db1`.`t1` mapped to number 114
# at 1282
#210216 12:54:53 server id 1  end_log_pos 1390 CRC32 0x93ed8f4c         Update_rows: table id 114 flags: STMT_END_F
### UPDATE `db1`.`t1`
### WHERE
###   @1=2
###   @2=2
### SET
###   @1=3
###   @2=2
### UPDATE `db1`.`t1`
### WHERE
###   @1=3
###   @2=3
### SET
###   @1=4
###   @2=3
### UPDATE `db1`.`t1`
### WHERE
###   @1=4
###   @2=4
### SET
###   @1=5
###   @2=4
### UPDATE `db1`.`t1`
### WHERE
###   @1=5
###   @2=5
### SET
###   @1=6
###   @2=5
# at 1390
#210216 12:54:53 server id 1  end_log_pos 1421 CRC32 0x13584d15         Xid = 45
COMMIT/*!*/;
SET @@SESSION.GTID_NEXT= 'AUTOMATIC' /* added by mysqlbinlog */ /*!*/;
DELIMITER ;
# End of log file
/*!50003 SET COMPLETION_TYPE=@OLD_COMPLETION_TYPE*/;
/*!50530 SET @@SESSION.PSEUDO_SLAVE_MODE=0*/;

这个本来时update t1 set id1 = id2+1 可以看出，上面对应了多个update语句。

```

# mysqlbinlog 源码解读
涉及的源码文件： mysqlbinlog.cc  
涉及的主要函数：  
dump_multiple_logs ->   
dump_single_log ->   
dump_local_log_entries ->   
process_event   

## 函数 dump_multiple_logs
非raw_mode 模式下，输出内容对应sql语句的间隔符为 /*!*/  
然后主要调用 dump_single_log  

## 函数 dump_single_log
根据 opt_remote_proto 调用不同的函数进行处理

static enum enum_remote_proto {  
  BINLOG_DUMP_NON_GTID = 0,  
  BINLOG_DUMP_GTID = 1,  
  BINLOG_LOCAL = 2  
} opt_remote_proto = BINLOG_LOCAL;  

```
    case BINLOG_LOCAL:
      rc = dump_local_log_entries(print_event_info, logname);
      break;
    case BINLOG_DUMP_NON_GTID:
    case BINLOG_DUMP_GTID:
      rc = dump_remote_log_entries(print_event_info, logname);
      break;
    default:
      DBUG_ASSERT(0);
```
## 函数 dump_local_log_entries
根据起始点及结束点进行读取内容，主要调用了
函数 Log_event::read_log_event  调用 read_log_event  
函数 process_event  

## 函数 process_event
根据 case ev_type 对不同的类型进行进行处理。  
如 binary_log::QUERY_EVENT  
   binary_log::ANONYMOUS_GTID_LOG_EVENT  
问题：读取一个事件时，怎么判断是否结束了？  
：：根据Log_event::read_log_event 函数中事件类型，可以看出事件实际存储的长度大小。  
在 ulong data_len = uint4korr(head + EVENT_LEN_OFFSET); 会记录当前事件的长度  

需要了解的类：  
Format_description_log_event  
Format_description_event  
Log_event  
print_event_info  



# 参开
MYSQL中BINLOG_FORMAT的三种模式  
https://www.cnblogs.com/xingyunfashi/p/8431780.html  

MySQL闪回技术——binlog2sql工具  
https://www.cnblogs.com/ziroro/p/9620175.html  
