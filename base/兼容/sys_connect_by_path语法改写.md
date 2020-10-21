# 1. 语法改写 connect by prior 

```
create table x1(id int, father_id int, name char(10), id4 int);
insert into x1 values(101, 100, 'n1', 1);
insert into x1 values(102, 101, 'n2', 2);
insert into x1 values(112, 101, 'n3', 3);
insert into x1 values(103, 102, 'n4', 4);
insert into x1 values(113, 112, 'n5', 5);
insert into x1 values(114, 113, 'n6', 6);
insert into x1 values(201, 200, 'n7', 7);

```

## 1.1 语法改写 connect by prior id = father_id start with father_id = 100

**测试语句**   
select id, father_id, level from x1 connect by prior id = father_id   
   start with father_id = 100;  

改写为mysql 支持的语句如下：  
```
select id, father_id, level, pathlevel from 
(
    select id, 
           father_id,
            @le:= if (father_id = 100 , 1, if( locate( concat('|',father_id,':'), @pathlevel) > 0,
                substring_index( substring_index(@pathlevel,concat('|',father_id,':'),-1),'|',1) +1, -1) ) level,
            @pathlevel:= concat(@pathlevel,'|',id,':', @le ,'|') pathlevel
    from (select id, father_id from db2.x1  order by 1,2)temp1,
    (
        select @le := 1,
               @pathlevel := ''
    )temp2
)xxx
where level > 0;

```
**执行测试**   
```
oracle 执行结果
SQL> select id, father_id, level from x1 connect by prior id = father_id   
  2     start with father_id = 100;  

        ID  FATHER_ID      LEVEL
---------- ---------- ----------
       101        100          1
       102        101          2
       103        102          3
       112        101          2
       113        112          3
       114        113          4

6 rows selected.

mysql 执行结果
+------+-----------+-------+--------------------------------------------+
| id   | father_id | level | pathlevel                                  |
+------+-----------+-------+--------------------------------------------+
|  101 |       100 |     1 | |101:1|                                    |
|  102 |       101 |     2 | |101:1||102:2|                             |
|  103 |       102 |     3 | |101:1||102:2||103:3|                      |
|  112 |       101 |     2 | |101:1||102:2||103:3||112:2|               |
|  113 |       112 |     3 | |101:1||102:2||103:3||112:2||113:3|        |
|  114 |       113 |     4 | |101:1||102:2||103:3||112:2||113:3||114:4| |
+------+-----------+-------+--------------------------------------------+
6 rows in set (0.01 sec)

```

## 1.2 语法改写 connect by prior id = father_id start with  id = 101

**测试语句**   
select id, father_id, level from x1 connect by prior id = father_id   
   start with  id = 101; 

改写为mysql 支持的语句如下：  
```
select id, father_id, 1 as level, concat('|', id, ':0|') as pathlevel from db2.x1 where id = 101
union
select id, father_id, level, pathlevel from 
(
    select id, 
           father_id,
            @le:= if (father_id = 101 , 2, if( locate( concat('|',father_id,':'), @pathlevel) > 0,
                substring_index( substring_index(@pathlevel,concat('|',father_id,':'),-1),'|',1) +1, -1) ) level,
            @pathlevel:= if(@le = -1, @pathlevel, concat(@pathlevel,'|',id,':', @le ,'|')) pathlevel
    from (select id, father_id from db2.x1  where id != 101 order by 1,2)temp1,
    (
        select @le := 2,
               @pathlevel := '|101:0|'
    )temp2
)xxx
where level > 0;


```

**执行测试**   

```
oracle 执行结果
SQL> select id, father_id, level from x1 connect by prior id = father_id    start with  id=101;

        ID  FATHER_ID      LEVEL
---------- ---------- ----------
       101        100          1
       102        101          2
       103        102          3
       112        101          2
       113        112          3
       114        113          4

6 rows selected.

mysql 执行结果
+------+-----------+-------+--------------------------------------------+
| id   | father_id | level | pathlevel                                  |
+------+-----------+-------+--------------------------------------------+
|  101 |       100 |     1 | |101:0|                                    |
|  102 |       101 |     2 | |101:0||102:2|                             |
|  103 |       102 |     3 | |101:0||102:2||103:3|                      |
|  112 |       101 |     2 | |101:0||102:2||103:3||112:2|               |
|  113 |       112 |     3 | |101:0||102:2||103:3||112:2||113:3|        |
|  114 |       113 |     4 | |101:0||102:2||103:3||112:2||113:3||114:4| |
+------+-----------+-------+--------------------------------------------+
6 rows in set (0.00 sec)
```

# 2. sys_connect_by_path

## 2.1 sys_connect_by_path ... start with father_id = x

测试语句
select id, father_id, level,  sys_connect_by_path(id, '/') as paths from x1   
   connect by prior id = father_id   
   start with father_id = 100;  

等价写法：  

```
 
select id, father_id, level, pathnodes from 
(
    select id, 
           father_id,
            @le:= if (father_id = 100 , 1, if( locate( concat('|',father_id,':'), @pathlevel) > 0,
                substring_index( substring_index(@pathlevel,concat('|',father_id,':'),-1),'|',1) +1, -1) ) level,
            @pathlevel:= if(@le = -1, @pathlevel, concat(@pathlevel,'|',id,':', @le ,'|')) pathlevel,
            @pathnodes:= if( father_id = 100, concat('/', id), concat_ws('/',if( locate( concat('|',father_id,':'),
               @pathall) > 0 ,  substring_index( substring_index(@pathall,concat('|', father_id, ':'),-1),'|',1),
                 @pathnodes ) ,id ) )pathnodes,
            @pathall:=concat(@pathall,'|',id,':', @pathnodes ,'|') pathall 

    from (select id, father_id from db2.x1    order by 1,2)temp1,
    (
        select @le := 1,
               @pathlevel := '',
               @pathall:='',
               @pathnodes:=''
    )temp2
)xxx
where level > 0;

```

测试结果  
```
oracle 执行结果
       101        100          1  /101
       102        101          2  /101/102
       103        102          3  /101/102/103
       112        101          2  /101/112
       113        112          3  /101/112/113
       114        113          4  /101/112/113/114

6 rows selected.

mysql 执行结果

+------+-----------+-------+------------------+
| id   | father_id | level | pathnodes        |
+------+-----------+-------+------------------+
|  101 |       100 |     1 | /101             |
|  102 |       101 |     2 | /101/102         |
|  103 |       102 |     3 | /101/102/103     |
|  112 |       101 |     2 | /101/112         |
|  113 |       112 |     3 | /101/112/113     |
|  114 |       113 |     4 | /101/112/113/114 |
+------+-----------+-------+------------------+
6 rows in set (0.01 sec)

```

## 2.1 sys_connect_by_path ... start with id = x

测试语句
select id, father_id, level,  sys_connect_by_path(id, '/') as paths from x1   
   connect by prior id = father_id   
   start with  id = 101;  

等价写法：  

```
select id, father_id, 1 as level, concat('|', id, ':1') as pathlevel,  concat('/', id) as pathnodes
 from db2.x1 where id = 101
 union
select id, father_id, level, pathlevel, pathnodes from 
(
    select id, 
           father_id,
            @le:= if (father_id = 101 , 2, if( locate( concat('|',father_id,':'), @pathlevel) > 0,
                substring_index( substring_index(@pathlevel,concat('|',father_id,':'),-1),'|',1) +1, -1) ) level,
            @pathlevel:= if(@le = -1, @pathlevel, concat(@pathlevel,'|',id,':', @le ,'|')) pathlevel,
            @pathnodes:= if( father_id = 101, concat('/101/', id), concat_ws('/',if( locate( concat('|',father_id,':'),
               @pathall) > 0 ,  substring_index( substring_index(@pathall,concat('|', father_id, ':'),-1),'|',1),
                 @pathnodes ) ,id ) )pathnodes,
            @pathall:=concat(@pathall,'|',id,':', @pathnodes ,'|') pathall 

    from (select id, father_id from db2.x1  where id != 101  order by 1,2)temp1,
    (
        select @le := 1,
               @pathlevel := '|101:1',
               @pathall:='',
               @pathnodes:=''
    )temp2
)xxx
where level > 0;

```

**测试结果** 
```
oracle
       101        100          1 /101
       102        101          2 /101/102
       103        102          3 /101/102/103
       112        101          2 /101/112
       113        112          3 /101/112/113
       114        113          4 /101/112/113/114

6 rows selected.


mysql 执行结果
+------+-----------+-------+-------------------------------------------+------------------+
| id   | father_id | level | pathlevel                                 | pathnodes        |
+------+-----------+-------+-------------------------------------------+------------------+
|  101 |       100 |     1 | |101:1                                    | /101             |
|  102 |       101 |     2 | |101:1|102:2|                             | /101/102         |
|  103 |       102 |     3 | |101:1|102:2||103:3|                      | /101/102/103     |
|  112 |       101 |     2 | |101:1|102:2||103:3||112:2|               | /101/112         |
|  113 |       112 |     3 | |101:1|102:2||103:3||112:2||113:3|        | /101/112/113     |
|  114 |       113 |     4 | |101:1|102:2||103:3||112:2||113:3||114:4| | /101/112/113/114 |
+------+-----------+-------+-------------------------------------------+------------------+
6 rows in set (0.01 sec)


```
