# ODBC 可移植使用方法
1. 对 odbc.ini 及 odbcinst.ini 处理  
	touch /etc/odbc.ini  
	chmod 777 /etc/odbc.ini  

	touch /etc/odbcinst.ini  
	chmod 777 /etc/odbcinst.ini  
1. linux odbc源码编译  

	../../unixODBC-2.3.7/configure --prefix=/home/wl/install/unixodbc --includedir=/home/wl/install/unixodbc/include --libdir=/home/wl/install/unixodbc/lib -bindir=/home/wl/install/unixodbc/bin --sysconfdir=/etc  

1. 内容填充  
	/etc/odbc.ini 内容： 
	```
	[MySQL-test]  
	Description = MySQL test database  
	Trace       = Off  
	TraceFile   = stderr  
	Driver      = MySQL  
	SERVER      = 127.0.0.1  
	USER        = root  
	PASSWORD    = password  
	PORT        = 3306  
	DATABASE    = db1  
	```
	/etc/odbcinst.ini 内容：  
	```
	[MYSQL]  
	Driver = /home/user1/work/DCI/lib/libmyodbc5w.so  
	SETUP = /home/user1/work/DCI/lib/libmyodbc5w.so  
	UsageCount = 1
	```

1. 用户user1操作  
	[user1@host122 DCI]$ ll lib/  
	total 11904  
	-rwxr-xr-x 1 user1 user1 10628992 Aug 12 06:49 libmyodbc5w.so  
	-rwxr-xr-x 1 user1 user1  1558792 Aug 12 06:48 libodbc.so  
	[user1@host122 DCI]$ pwd  
	/home/user1/work/DCI  

1. 查看 /etc/odbc.ini及/etc/odbcinst.ini内容  
	按上面内容填充










