# 设置允许远端连接
grant all on *.* to admin@'ip' identified by 'password' with grant option;  
允许任意用户对应密码password通过ip连接  
示例：  
grant all on *.* to admin@'%' identified by 'password' with grant option;   
flush privileges;

允许任何ip地址(%表示允许任何ip地址)的电脑用admin帐户和密码(password)来访问这个mysql server。  
use mysql;   
select user, host from user;  

# 更新密码
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'password';  

















