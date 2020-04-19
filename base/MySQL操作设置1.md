# 1 设置允许远端连接  
grant all on *.* to admin@'ip' identified by 'password' with grant option;   
允许任意用户对应密码password通过ip连接  
**示例**    
grant all on *.* to admin@'%' identified by 'password' with grant option;   
flush privileges;

允许任何ip地址(%表示允许任何ip地址)的电脑用admin帐户和密码(password)来访问这个mysql server。  
use mysql;   
select user, host from user;  

# 2 更新密码
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password';  

my.cnf 文件 bind-address 设置多个ip （用空格隔开）

## 3 debug
源码 -DWITH_DEBUG=1  
环境变量   export MYSQL_DEBUG=d:t:O,/home/wl/mysqlcluster/client.trace  
或者 mysql 连接数据库时加上 --debug 默认路径/tmp/client.trace  

https://dev.mysql.com/doc/refman/5.7/en/dbug-package.html  














