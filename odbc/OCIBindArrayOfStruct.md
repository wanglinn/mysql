OCI 绑定变量
OCIBindArrayOfStruct
OCIBindByPos
OCIBindByName


https://www.oschina.net/question/1160717_112781?sort=time

http://blog.itpub.net/81/viewspace-709486/

http://www.itpub.net/thread-783817-1-1.html

http://soft.zhiding.cn/software_zone/2007/0916/509767.shtml



7．准备SQL语句 
sword OCIStmtPrepare ( 
OCIStmt       *stmtp,//语句句柄   
                OCIError      *errhp, 
                      CONST text    *stmt,  //SQL语句 
                      ub4           stmt_len,   //语句长度 
                      ub4           language,  //语句的语法格式=OCI_NTV_SYNTAX 
ub4           mode //=OCI_DEFAULT 
);

eg: 
char sSQL[1024];

sprintf(sSQL, “select table_name from user_tables”);

swResult = OCIStmtPrepare(stmtp errhp,  (CONST OraText*)sSQL, strlen(sSQL), OCI_NTV_SYNTAX, OCI_DEFAULT); 
if(swResult != OCI_SUCCESS && swResult != OCI_SUCCESS_WITH_INFO) 
return FALSE;

 


8． 绑定输入参数

OCIBindArrayOfStruct() Set skip parameters for static array bind ，数组绑定，一般用于批量操作 
OCIBindByName() Bind by name  按名绑定 
OCIBindByPos() Bind by position  按位置绑定,建议一般按此方式绑定 
OCIBindDynamic() Sets additional attributes after bind with OCI_DATA_AT_EXEC mode   
OCIBindObject() Set additional attributes for bind of named data type

注： 
OCIBindArrayOfStruct必须先用OCIBindByPos初始化，然后在OCIBindArrayOfStruct中定义每个参数所跳过的字节数。 
如：

存储方式：

 

第一条记录第二条记录 N


 SkipPara(实际就是结构体长度，即本次所有列的长度和)

 

sword OCIBindByName ( 
OCIStmt       *stmtp, //语句句柄 
                OCIBind       **bindpp,//结合句柄,=NULL 
                OCIError      *errhp, 
                CONST text    *placeholder,//占位符名称 
                sb4           placeh_len, //占位符长度 
                dvoid         *valuep, //绑定的变量名 
                sb4           value_sz, //绑定的变量名长度 
                ub2           dty,  //绑定的类型 
                dvoid         *indp, //指示符变量指针(sb2类型),单条绑定时为NULL, 
                ub2           *alenp, //说明执行前后被结合的数组变量中各元素数据实际的长度，单条绑定时为NULL 
                ub2           *rcodep,//列级返回码数据指针，单条绑定时为NULL 
                ub4           maxarr_len, //最多的记录数,如果是单条绑定，则为0 
                ub4           *curelep, //实际的记录数,单条绑定则为NULL 
                ub4           mode //=OCI_DEFAULT 
);

sword OCIBindByPos ( OCIStmt      *stmtp, 
                    OCIBind      **bindpp, 
                    OCIError     *errhp, 
                    ub4          position,// 绑定的位置 
                    dvoid        *valuep, 
                    sb4          value_sz, 
                    ub2          dty, 
                    dvoid        *indp, 
                    ub2          *alenp, 
                    ub2          *rcodep, 
                    ub4          maxarr_len, 
                    ub4          *curelep, 
                    ub4          mode );

sword OCIBindArrayOfStruct ( 
OCIBind     *bindp,//绑定的结构句柄,由OCIBindByPos定义 
                OCIError    *errhp, 
                ub4         pvskip, //下一列跳过的字节数** 
                ub4         indskip,//下一个指示器或数组跳过的字节数 
                ub4         alskip, //下一个实际值跳过的字节数 
                ub4         rcskip //下一个列级返回值跳过的字节数 
);

例： 
sword     swResult; 
OCIBind*  hBind; 
Ub4 rec_num; 
Sql:  insert into student values (:p1,:p2)

单条绑定： 
hBind = NULL; 
swResult = OCIBindByPos(stmtp &hBind, errhp,1,ststd.tname, 
sizeof(ststd.tname), SQLT_CHR, NULL, 
NULL,NULL,0, NULL, OCI_DEFAULT);


批量取数据，一次取100条 
Sql:  select username,age from student where username=:p1 and age=:p2

hBind = NULL; 
swResult = OCIBindByPos(stmtp &hBind, errhp,1,tstd[0].tname, 
sizeof(tstd[0].tname), SQLT_CHR, &tstdInd.sb2_usernmae[0], 
&tstdLen.ub2_username[0],&tstdRet.ub2_username[0],100, &rec_num, OCI_DEFAULT); 
swResult = OCIBindArrayOfStruct(hBind, errhp,sizeof(tstd [0]), sizeof(sb2), sizeof(ub2), sizeof(ub2));

9．执行SQL语句 
sword OCIStmtExecute ( 
OCISvcCtx           *svchp,  //服务环境句柄 
                      OCIStmt             *stmtp,  //语句句柄 
                      OCIError            *errhp, 
                      ub4                 iters, // ** 
                      ub4                 rowoff, //** 
                      CONST OCISnapshot   *snap_in, 
                      OCISnapshot         *snap_out, 
                      ub4                 mode //** 
); 
**注： 
1. iters:对于select语句，它说明一次执行读取到buffer中的记录行数，如果不能确定select语句所返回的行数，可将iters设置为0,而对于其他的语句，iters表示这些语句的执行次数，此时iters不能为0。 
2. rowoff:在多行执行时，该参数表示从所结合的数据变量中的第几条记录开始执行(即记录偏移量)。 
3. mode：=OCI_DEFAULT:default模式 
=OCI_DESCRIBE_ONLY：描述模式，只返回选择列表的描述信息，而不执行语句 
=OCI_COMMIT_ON_SUCCESS：自动提交模式，当执行成功后，自动提交。 
=OCI_EXACT_FETCH:精确提取模式。 
=OCI_BATCH_ERRORS：批错误执行模式：用于执行数组方式的操作，在此模式下，批量insert ,update,delete时，执行过程中任何一条记录错误不会导致整个insert ,update,delete失败，系统自动会收集错误信息，而在非批错误方式下，其中的任何一条记录错误，将会导致整个操作失败。 
Eg: 
执行一次 
swResult = OCIStmtExecute(svchp, stmtp,  errhp；, 
                     1, 0, NULL, NULL, OCI_DEFAULT); 
批量执行100次： 
swResult = OCIStmtExecute(svchp, stmtp,  errhp；, 
                     100, 0, NULL, NULL, OCI_DEFAULT);

10．定义输出变量 
  
OCIDefineArrayOfStruct()   Set additional attributes for static array define   
OCIDefineByPos()   Define an output variable association   
OCIDefineDynamic()   Sets additional attributes for define in OCI_DYNAMIC_FETCH mode   
OCIDefineObject()   Set additional attributes for define of named data type  

sword OCIDefineByPos ( 
OCIStmt     *stmtp, //语句句柄 
                      OCIDefine   **defnpp,//定义句柄―用于数组变量 
                      OCIError    *errhp, 
                      ub4         position,//位置序号(从1 开始) 
                      dvoid       *valuep, //输出的变量名 
                      sb4         value_sz, //变量长度 
                      ub2         dty,  //数据类型 
                      dvoid       *indp, //指示器变量/指示器变量数组，如果此字段可能存在空值，则要指示器变量，否则单条处理时为NULL 
                      ub2         *rlenp, //提取的数据长度 
                      ub2         *rcodep, //列级返回码数组指针 
ub4         mode //OCI_DEFAULT 
);

 


sword OCIDefineArrayOfStruct ( 
OCIDefine   *defnp,//由OCIDefineByPos定义的句柄 
                OCIError    *errhp, 
ub4         pvskip, //下一列跳过的字节数,一般就是结构的大小 
                ub4         indskip,//下一个指示器或结构跳过的字节数,=0 
                ub4         rlskip, //下一个实际值跳过的字节数,=0 
                ub4         rcskip //下一个列列级返回值跳过的字节数,=0 
);

sword OCIDefineDynamic ( 
OCIDefine   *defnp, 
                        OCIError    *errhp, 
                        dvoid       *octxp, 
                        OCICallbackDefine       (ocbfp)(/*_ 
                                 dvoid          *octxp, 
                                 OCIDefine      *defnp, 
                                 ub4            iter, 
                                 dvoid          **bufpp, 
                                 ub4            **alenpp, 
                                 ub1            *piecep, 
                                 dvoid          **indpp, 
                                 ub2            **rcodep _*/)  );

sword OCIDefineObject ( OCIDefine       *defnp, 
                       OCIError        *errhp, 
                       CONST OCIType   *type, 
                       dvoid           **pgvpp, 
                       ub4             *pvszsp, 
                       dvoid           **indpp, 
                       ub4             *indszp );
					   
					   
					   
					   
					   
					   
https://www.hackhome.com/InfoView/Article_165465_20.html

例： 
sword     swResult; 
OCIBind*  hBind; 
Ub4 rec_num; 
Sql:  insert into student values (:p1,:p2) 

单条绑定： 
hBind = NULL; 
swResult = OCIBindByPos(stmtp &hBind, errhp,1,ststd.tname,  
sizeof(ststd.tname), SQLT_CHR, NULL,  
NULL,NULL,0, NULL, OCI_DEFAULT); 


批量取数据，一次取100条 
Sql:  select username,age from student where username=:p1 and age=:p2 

hBind = NULL; 
swResult = OCIBindByPos(stmtp &hBind, errhp,1,tstd[0].tname,  

sizeof(tstd[0].tname), SQLT_CHR, &tstdInd.sb2_usernmae[0],  
&tstdLen.ub2_username[0],&tstdRet.ub2_username[0],100, &rec_num, OCI_DEFAULT); 
swResult = OCIBindArrayOfStruct(hBind, errhp,sizeof(tstd [0]), sizeof(sb2), sizeof(ub2), sizeof(ub2)); 

---
sword OCIDefineArrayOfStruct (  
OCIDefine   *defnp,//由OCIDefineByPos定义的句柄 
                 OCIError    *errhp, 
ub4         pvskip, //下一列跳过的字节数,一般就是结构的大小 
                 ub4         indskip,//下一个指示器或结构跳过的字节数,=0  
                 ub4         rlskip, //下一个实际值跳过的字节数,=0 
                 ub4         rcskip //下一个列列级返回值跳过的字节数,=0 
); 


					   
					   
OciBindArrayofStruct函数的用法
OCI操作Oracle的时候，例如有个表TestData(xdata,ydata)，两个字段都是number(10,4)类型的，外部有两个double数组，怎么通过OciBindArrayofStruct这个函数来实现呢，哪位高手帮忙解决一下，谢谢了！

解决方案：
对两个字段及其对应的数组调用分别调用一次下面的代码(fieldname和doublearrayname替换成实际的名称)先调用OCIBindByName：fErr = OCIBindByName(hStmt, &hBind, hErr, (text *)fieldname,strlen(fieldname), (dvoid *)(doubleArrayname),(sb4)sizeof(double), SQLT_FLT, (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);然后调用 OCIBindArrayOfStruct：
fErr = OCIBindArrayOfStruct ( hBind, hErr,(ub4) sizeof(double),0,0,0);
  
  
  
  
 https://www.hackhome.com/InfoView/Article_165465_28.html
 eg: 
单条查询 
sql: select username,age from student  where username=:p1; 
 如果此字段有可能有空值，则 
hDefine = NULL; 
swResult = OCIDefineByPos(stmtp &hDefine, errhp, 1, tstd.username, sizeof(tstd.username), SQLT_CHR, & sb2aInd[0], NULL, NULL, OCI_DEFAULT); 
如果此字段没有空值，则 
hDefine = NULL; 
swResult = OCIDefineByPos(stmtp &hDefine, errhp, 1, tstd.username, sizeof(tstd.username), SQLT_CHR, NULL, NULL, NULL, OCI_DEFAULT);   
    批量查询 
select username,age from student  where age>30; 
hDefine = NULL; 
swResult = OCIDefineByPos(stmtp, &hDefine, errhp, 1, &tstd[0].username,  
sizeof(tstd[0].usenmae), SQLT_CHR, NULL, NULL, NULL, OCI_DEFAULT); 

swResult = OCIDefineArrayOfStruct(hDefine, errhp, sizeof(tstd[0]), 0, 0, 0); 



  

假设已分配了相关的句柄，且已连接数据库，并开始了回话

...

{

OCISvcCtx *svr;

OCIStmt *stmt;

OCIError *err;

char sql[]="BEGIN \

UPDATE_TEST_OCI.ADD_TEST_OCI(:n,:id,:val); \

END;";

/*分析没有问题*/

OCIStmtPrepare(stmt,err,(text*)sql,(ub4)strlen(sql),OCI_NTV_SYNTAX,OCI_DEFAULT);

{

OCIBind *nbd;

int n;

ub2 n_rlen=sizeof(int);

sb2 n_ind=0;

ub2 n_rcode=0;


OCIBind *idbd;

int id[3]={1,2,3};

ub2 id_rlen[3]={sizeof(int),sizeof(int),sizeof(int)};

sb2        id_ind[3]={0,0,0};

ub2 id_rcode[3]={0,0,0};

ub4 id_cur=3;


OCIBind *valbd;

char val[3][20]={"test1","test2","test3"};

ub2 val_rlen[3]={sizeof(val[0]),sizeof(val[1]),sizeof(val[2])};

sb2        val_ind[3]={0,0,0};

ub2 val_rcode[3]={0,0,0};

ub4 val_cur=3;


/*绑定也没问题*/

OCIBindByName(stmt,&nbd,err,":n",strlen(":n"),&n,sizeof(int),SQLT_INT,&n_ind,&n_rlen,&n_rcode,0,0,OCI_DEFAULT);

OCIBindByName(stmt,&idbd,err,":id",strlen(":id"),&id,sizeof(int),SQLT_INT,id_ind,id_rlen,id_rcode,id_cur,&id_cur,OCI_DEFAULT);

OCIBindArrayOfStruct(idbd,err,sizeof(int),sizeof(sb2),sizeof(ub2),sizeof(ub2));

OCIBindByName(stmt,&valbd,err,":val",strlen(":val"),&val,sizeof(val[0]),SQLT_STR,val_ind,val_rlen,val_rcode,id_cur,&id_cur,OCI_DEFAULT);

OCIBindArrayOfStruct(valbd,err,sizeof(val[0]),sizeof(sb2),sizeof(ub2),sizeof(ub2));

}

/*执行没有任何反应*/

OCIStmtExecute(svr,stmt,err,1,0,0,0,OCI_DEFAULT);

}


/*如果将分析的SQL改变，却可以成功*/

...

{


OCISvcCtx *svr;

OCIStmt *stmt;

OCIError *err;

char sql[]="BEGIN \

UPDATE_TEST_OCI.ADD_TEST_OCI(3,:id,:val); \

END;";

/*分析没有问题*/

OCIStmtPrepare(stmt,err,(text*)sql,(ub4)strlen(sql),OCI_NTV_SYNTAX,OCI_DEFAULT);

{

OCIBind *idbd;

int id[3]={1,2,3};

ub2 id_rlen[3]={sizeof(int),sizeof(int),sizeof(int)};

sb2        id_ind[3]={0,0,0};

ub2 id_rcode[3]={0,0,0};

ub4 id_cur=3;



OCIBind *valbd;

char val[3][20]={"test1","test2","test3"};

ub2 val_rlen[3]={sizeof(val[0]),sizeof(val[1]),sizeof(val[2])};

sb2        val_ind[3]={0,0,0};

ub2 val_rcode[3]={0,0,0};

ub4 val_cur=3;



/*绑定没问题*/

OCIBindByName(stmt,&idbd,err,":id",strlen(":id"),&id,sizeof(int),SQLT_INT,id_ind,id_rlen,id_rcode,id_cur,&id_cur,OCI_DEFAULT);

OCIBindArrayOfStruct(idbd,err,sizeof(int),sizeof(sb2),sizeof(ub2),sizeof(ub2));

OCIBindByName(stmt,&valbd,err,":val",strlen(":val"),&val,sizeof(val[0]),SQLT_STR,val_ind,val_rlen,val_rcode,id_cur,&id_cur,OCI_DEFAULT);

OCIBindArrayOfStruct(valbd,err,sizeof(val[0]),sizeof(sb2),sizeof(ub2),sizeof(ub2));

}

/*执行成功*/


OCIStmtExecute(svr,stmt,err,1,0,0,0,OCI_DEFAULT);

}



---------------

oci中处理null，必须通过 Indicator 来完成。

使用的Oracle 版本 Oracle Database 11g Enterprise Edition Release 11.1.0.6.0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include\oci.h"
#include "include\ociextp.h"

 

#define ERRGOTO(Recode) do    \
{          \
 if (Recode!=0)      \
 {         \
 goto ERR;          \
 }         \
} while (0)

 

void main()
{

int a = 0 ;
 OCIDefine* defhp1 = 0;
 OCIDefine* defhp2 = 0;
 int blength = 40;
 char b[40];
 sb2 indicator;

 text *select_sql = (text *)"select a,b from teststmt2";

 dvoid     *tmp;
 OCIEnv *envhp;
 OCIServer *srvhp;
 OCIError *errhp;
 OCISvcCtx *svchp;
 OCISession *usrhp;
 OCIStmt *stmthp;


 char serName1[30] ="10.0.4.161:1521/orcl";
 char userName1[30] = "cxy";
 char pwd1[30] = "cxy";

 //!如果没有数据可以使用这个测试用例创建数据表。
 //test_BindArrayOfStruct();

 memset(b, 0, blength);

 //!初始化换环境句柄
 ERRGOTO(OCIEnvCreate(&(envhp), OCI_DEFAULT,
  NULL, NULL, NULL, NULL, 0, NULL));

 //!初始化错误句柄
 ERRGOTO(OCIHandleAlloc((dvoid *)envhp,(dvoid **)&errhp,OCI_HTYPE_ERROR,64,(dvoid **) &tmp));

 //!分配服务上下文句柄和服务句柄
 ERRGOTO(OCIHandleAlloc((dvoid *)envhp,(dvoid **)&srvhp,OCI_HTYPE_SERVER, 64,(dvoid **) &tmp));
 ERRGOTO(OCIHandleAlloc((dvoid *)envhp,(dvoid **)&svchp,OCI_HTYPE_SVCCTX, 64,(dvoid **) &tmp));

 //!初始化服务器句柄
 ERRGOTO(OCIServerAttach( srvhp, errhp, (text *)serName1, (sb4) strlen(serName1), (ub4) OCI_DEFAULT));

 //!/将服务器句柄连接到服务上下文句柄
 ERRGOTO(OCIAttrSet ((dvoid *)svchp, OCI_HTYPE_SVCCTX,(dvoid *)srvhp, (ub4) 0, OCI_ATTR_SERVER, errhp));

 //!分配设置会话句柄，并向里填充用户名和密码
 ERRGOTO(OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&usrhp,OCI_HTYPE_SESSION, 0, (dvoid **) 0));
 ERRGOTO(OCIAttrSet ((dvoid *)usrhp, OCI_HTYPE_SESSION,(dvoid *)userName1, (ub4)strlen(userName1),OCI_ATTR_USERNAME, errhp));
 ERRGOTO(OCIAttrSet ((dvoid *)usrhp, OCI_HTYPE_SESSION,(dvoid *)pwd1, (ub4)strlen(pwd1),OCI_ATTR_PASSWORD, errhp));

 //!建立会话
 ERRGOTO(OCISessionBegin (svchp, errhp, usrhp,OCI_CRED_RDBMS, OCI_DEFAULT));

 //!将会话句柄连接到服务上下文句柄
 ERRGOTO( OCIAttrSet (  (dvoid *)svchp, OCI_HTYPE_SVCCTX,(dvoid *)usrhp, (ub4) 0, OCI_ATTR_SESSION, errhp));

 //!分配语句句柄
 ERRGOTO(OCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, 0, 0));

 


 //!开始查询并获得结果
 text *delete_table = (text *)"delete from teststmt2";
 ERRGOTO(OCIStmtPrepare(stmthp,errhp,(text *)delete_table,strlen((char *)delete_table),OCI_NTV_SYNTAX,OCI_DEFAULT));
 ERRGOTO(OCIStmtExecute(svchp,stmthp,errhp,1,0,0,0,OCI_DEFAULT));


 int aa = 8;
 char bb[] = "bbbbbb";
 indicator = -1;
 text *insert_sql = (text *)"INSERT INTO teststmt2(a,b) values(:1,:2)"; 
 OCIBind* bindhp = 0;
 //!准备数据，绑定参数
 ERRGOTO(OCIStmtPrepare(stmthp,errhp,(text *)insert_sql,strlen((char *)insert_sql),OCI_NTV_SYNTAX,OCI_DEFAULT));
 ERRGOTO(OCIBindByPos(stmthp,&bindhp,errhp,1, (dvoid *)&aa,4, SQLT_INT, (void*)&indicator, NULL, NULL,0,0,0));
 //OCIBindArrayOfStruct(bindhp,errhp,sizeof(int),0,0,0);

 ERRGOTO(OCIBindByPos(stmthp,&bindhp,errhp,2,(dvoid *)&bb, 6, SQLT_CHR,NULL,NULL,NULL,0,0,0));
 //OCIBindArrayOfStruct(bindhp,errhp,2,0,0,0);

 //！执行语句
 ERRGOTO(OCIStmtExecute(svchp,stmthp,errhp,1,0,0,0,0));
 
 indicator = -1;

 //!开始查询并获得结果
 ERRGOTO(OCIStmtPrepare(stmthp,errhp,(text *)select_sql,strlen((char *)select_sql),OCI_NTV_SYNTAX,OCI_DEFAULT));
 //!绑定以一个列
 ERRGOTO(OCIDefineByPos(stmthp,&defhp1,errhp,1,&a,sizeof(a),SQLT_INT, (void*)&indicator,0,0,OCI_DEFAULT));
 //ERRGOTO(OCIDefineArrayOfStruct(defhp1,errhp,sizeof(int),0,4,0));
 //!绑定以二个列

 indicator = -2;
 ERRGOTO(OCIDefineByPos(stmthp,&defhp2,errhp,2,&b, 20, SQLT_CHR, (void*)&indicator, 0, 0, OCI_DEFAULT));
 //ERRGOTO(OCIDefineArrayOfStruct(defhp2,errhp,2,0,2,0));
 //!执行语句
 ERRGOTO(OCIStmtExecute(svchp,stmthp,errhp,1,0,0,0,OCI_DEFAULT));
 //ERRGOTO(OCIStmtFetch(stmthp,errhp,1,OCI_FETCH_NEXT,0));

 printf("%d, %s, %d", a, b, indicator);

 ERRGOTO(OCITransCommit(svchp,errhp,0));

 
 //!释放各个资源
 
 //ERRGOTO(OCIHandleFree(srvhp,OCI_HTYPE_SERVER));
 //ERRGOTO(OCIHandleFree(errhp,OCI_HTYPE_ERROR));
 //ERRGOTO(OCIHandleFree(envhp,OCI_HTYPE_ENV));

 

 return ;
ERR:
 sb4 errcod=0;
 OraText msgerr[200]="";
 OraText msgstat[200]="";
 OCIErrorGet(errhp,1,msgstat,&errcod,msgerr,200,OCI_HTYPE_ERROR);
 printf((char *)msgerr);
 return;

 

}

https://www.linuxidc.com/Linux/2011-02/32564.htm



http://www.mchz.com.cn/bbs/showtopic-2875.aspx

OCI使用结构体方式进行批量操作

/*

* 使用结构体进行批量插入

*/

#include <oci.h>

#include <errno.h>

#include <string>

#include <stdio.h>

#include <iostream>

using namespace std;


class dept{

public:

int deptno;

char dname;

char loc;


dept(){deptno=0;}


~dept(){}


dept(const dept &dept){

deptno = dept.deptno;

dname = dept.dname;

loc = dept.loc;

}


dept& operator=(const dept &dept){

deptno = dept.deptno;

dname = dept.dname;

loc = dept.loc;

return *this;

}

};


typedef struct{int deptno;char dname[6];char loc[12];}dept_T;


int main(){

sword status = 0;

dept_T deptarry[10];

for(int i=0;i<10;i++){

deptarry.deptno = 60+i;

strcpy(deptarry.dname, "test0");

strcpy(deptarry.loc, "bulkinsert0");

}


text *sqlstmt=(text *)"insert into dept(deptno,dname,loc) values (:deptno,:dname,:loc)";


OCIEnv *envhp;

OCIServer *srvhp;

OCIError *errhp;

OCISession *usrhp;

OCISvcCtx *svchp;

OCIStmt *stmthp;


OCIBind *bndp = NULL;

OCIBind *bnd1p = NULL;

OCIBind *bnd2p = NULL;

OCIBind *bnd3p = NULL;


OCIEnvCreate(&envhp,OCI_THREADED,(dvoid *)0,0,0,0,(size_t)0,(dvoid **)0);

OCIHandleAlloc((dvoid *)envhp,(dvoid **)&srvhp,OCI_HTYPE_SERVER,0,(dvoid **)0);

OCIHandleAlloc((dvoid *)envhp,(dvoid **)&errhp,OCI_HTYPE_ERROR,0,(dvoid **)0);

OCIServerAttach(srvhp,errhp,(text *)"",strlen(""),OCI_DEFAULT);

OCIHandleAlloc((dvoid *)envhp,(dvoid **)&svchp,OCI_HTYPE_SVCCTX,0,(dvoid **)0);

OCIHandleAlloc((dvoid *)envhp,(dvoid **)&usrhp,OCI_HTYPE_SESSION,0,(dvoid **)0);

OCIHandleAlloc((dvoid *)envhp,(dvoid **)&stmthp,OCI_HTYPE_STMT,0,(dvoid **)0);

OCIAttrSet((dvoid *)svchp,OCI_HTYPE_SVCCTX,(dvoid *)srvhp,(ub4)0,OCI_ATTR_SERVER,errhp);

OCIAttrSet((dvoid *)usrhp,OCI_HTYPE_SESSION,(dvoid *)"scott",(ub4)strlen("scott"),OCI_ATTR_USERNAME,errhp);

OCIAttrSet((dvoid *)usrhp,OCI_HTYPE_SESSION,(dvoid *)"scott",(ub4)strlen("scott"),OCI_ATTR_PASSWORD,errhp);

OCISessionBegin(svchp,errhp,usrhp,OCI_CRED_RDBMS,OCI_DEFAULT);

OCIAttrSet((dvoid *)svchp,OCI_HTYPE_SVCCTX,(dvoid *)usrhp,(ub4)0,OCI_ATTR_SESSION,errhp);


OCIStmtPrepare(stmthp,errhp,(text *)sqlstmt,(ub4)strlen((char *)sqlstmt),(ub4)OCI_NTV_SYNTAX,(ub4)OCI_DEFAULT);


OCIBindByPos(stmthp,&bndp,errhp,1,&deptarry[0].deptno,4,SQLT_INT,0, 0, 0,0,0,OCI_DEFAULT);

OCIBindArrayOfStruct(bndp,errhp,sizeof(deptarry[0]),0,0,0);//需要为每一个绑定变量设置OCIBindArrayOfStruct

OCIBindByPos(stmthp,&bndp,errhp,2,&deptarry[0].dname,6,SQLT_CHR,0, 0, 0,0,0,OCI_DEFAULT);

OCIBindArrayOfStruct(bndp,errhp,sizeof(deptarry[0]),0,0,0);

OCIBindByPos(stmthp,&bndp,errhp,3,&deptarry[0].loc,12,SQLT_CHR,0, 0, 0,3,0,OCI_DEFAULT);

OCIBindArrayOfStruct(bndp,errhp,sizeof(deptarry[0]),0,0,0);


status = OCIStmtExecute(svchp,stmthp,errhp,10,0,(OCISnapshot *)0,(OCISnapshot *)0,OCI_DEFAULT);


system("pause");

}

