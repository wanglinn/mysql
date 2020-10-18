# 1. \h xx 信息来源
```
./scripts/fill_help_tables.sql
对应表 mysql.help_topic 

比如 \h crate table   
可以从表中查找   
select * from mysql.help_topic where name like 'CREATE TABLE' \G  

# 2.help帮助信息命令执行过程流程
