# 1. connect by prior 介绍

**语法**  
```
{ CONNECT BY [ NOCYCLE ] condition [AND condition]... [ START WITH condition ]
| START WITH condition CONNECT BY [ NOCYCLE ] condition [AND condition]...}

解释:

start with: 指定起始节点的条件

connect by: 指定父子行的条件关系

prior: 查询父行的限定符，格式: prior column1 = column2 or column1 = prior column2 and ... ，

nocycle: 若数据表中存在循环行，那么不添加此关键字会报错，添加关键字后，便不会报错，但循环的两行只会显示其中的第一条

循环行: 该行只有一个子行，而且子行又是该行的祖先行

connect_by_iscycle: 前置条件:在使用了nocycle之后才能使用此关键字，用于表示是否是循环行，0表示否，1 表示是

connect_by_isleaf: 是否是叶子节点，0表示否，1 表示是

level: level伪列,表示层级，值越小层级越高，level=1为层级最高节点


```
**个人理解：**  
connect by prior 是oracle 提供的一个查询表数据的树形结构的功能。  
connect by prior 用法模式介绍：  
connect by prior id = father_id start with id = x;  
通过start with id=x  或者 start with father_id=x 获取第一行数据；  
然后再通过connect by prior id = father_id 获取第二行及以后的数据，这里prior 表示之前  
的意思，prior id = father_id 表示要取的father_id 的值来自上次取的id的值。

connect by prior id = father_id start with id = x0 可以这样递推：  
根据id=x0 获取 对应的行1，  
根据father_id = x0 获取对应的行2， 获取对应的id=x2;  
根据father_id=x2 获取对应的行3，获取对应的id=x3;  
...

示例：  
```
create table t1(id int, father_id int);
insert into t1 values(1,0);
insert into t1 values(2,1);
insert into t1 values(12,1);
insert into t1 values(3,2);
insert into t1 values(102,101);

SQL> select id, father_id, level from t1 connect by prior id = father_id start with id=1;

        ID  FATHER_ID      LEVEL
---------- ---------- ----------
         1          0          1
         2          1          2
         3          2          3
        12          1          2

SQL> select id, father_id, level from t1 connect by  id =  prior father_id start with id=2;

        ID  FATHER_ID      LEVEL
---------- ---------- ----------
         2          1          1
         1          0          2

增加一行，使出现循环
SQL> insert into t1 values(1,1);

1 row created.

SQL> select id, father_id, level from t1 connect by prior id = father_id start with id=1;
ERROR:
ORA-01436: CONNECT BY loop in user data


SQL> select id, father_id, level from t1 connect by  nocycle prior id = father_id start with id=1;

        ID  FATHER_ID      LEVEL
---------- ---------- ----------
         1          0          1
         2          1          2
         3          2          3
        12          1          2
         1          1          1
         2          1          2
         3          2          3
        12          1          2
：：如果循环id=father_id 出现开头，如上，会出现2份。

如果是中间出现的则忽略该行

SQL> select * from t1 order by 1,2;   

        ID  FATHER_ID
---------- ----------
         1          0
         2          1
         2          2
         3          2
         4          3
        12          1
       102        101

7 rows selected.

SQL> select id, father_id, level from t1 connect by  nocycle prior id = father_id start with id=1;

        ID  FATHER_ID      LEVEL
---------- ---------- ----------
         1          0          1
         2          1          2
         3          2          3
         4          3          4
        12          1          2

```

# 2. 功能实现
初步实现connect by prior id=father_id start with id=x  
```
#include <iostream>
#include <list>
using namespace std;

typedef struct
{
    int id;
    int father_id;
}id_pid;

/*
 * show list node
 */
void show_nodes(list<id_pid>&list_data)
{
    list<id_pid>::iterator it;
    cout<<"result:"<<endl;
    for(it=list_data.begin(); it!=list_data.end(); it++)
    {
        cout<<it->id<<'\t'<<it->father_id<<endl;
    }
}

/*
 * add node
 */
void add_nodes(list<id_pid> &list_data)
{
    int i = 0;
    int total = 3;
    id_pid data_value;
    while(i++ < total)
    {
        data_value.father_id = i;
        data_value.id = i+1;
        list_data.push_back(data_value);
    }
#if 1
    while(i++ < total+10)
    {
        data_value.father_id = i;
        data_value.id=i+i%2;
        list_data.push_back(data_value);
    }
#endif
}

/*
 * get the first node
 */
bool get_first_value(list<id_pid> &list_task1, list<id_pid>&list_data, int father_id)
{
    list<id_pid>::iterator it;
    for(it=list_data.begin(); it!=list_data.end(); it++)
    {
        if(father_id == it->father_id)
        {
          id_pid data_value;
          data_value.id = it->id;
          data_value.father_id = it->father_id;

          list_task1.push_back(data_value);
          it=list_data.erase(it);
        }
    }

    return list_data.size() == 0 ? false:true;
}

/*
 * connect by prior
 */
void get_connect_by_prior(list<id_pid>&list_task1, list<id_pid> &list_data  
                          ,list<id_pid> &list_res)
{
    list<id_pid>::iterator it;
    int father_id;
    id_pid data_value;
    list<id_pid> list_task2;

    while(list_task1.size()>0)
    {
        for(it=list_task1.begin(); it!=list_task1.end(); it++)
        {
            data_value.id = it->id;
            data_value.father_id = it->father_id;
            list_res.push_back(data_value);

            father_id = it->id;
            get_first_value(list_task2, list_data, father_id);
        }
        list_task1 = list_task2;
        list_task2.clear();
    }

}

int main()
{
    list<id_pid> list_data;
    list<id_pid> list_task1;
    list<id_pid> list_res;

    add_nodes(list_data);
    show_nodes(list_data);

    get_first_value(list_task1, list_data, 1);
    get_connect_by_prior(list_task1, list_data, list_res);
    show_nodes(list_res);

}

```
测试结果：  
```
[wl@host122 cpp]$ ./connect1 
result:
2       1
3       2
4       3
6       5
6       6
8       7
8       8
10      9
10      10
12      11
12      12
14      13
result:
2       1
3       2
4       3
```
