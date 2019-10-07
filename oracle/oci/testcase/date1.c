/*
* Copyright (c) OCI高级编程
* @File name: oci_eg2.cpp
* @Author   : He Xiong
* @Content  : OCI批错误处理模式的使用
* 创建日期：2003-10
****/

/*
 setup :
 create table date1(id1 int, id2 date);
 
 查看：
 select to_char(id2, 'yyyy-mm-dd') from date1;

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oci.h>
#include "ocilib.h"


//获取错误诊断信息
//@param   : pErr,错误句柄
//           lStatus, 状态码信息
//@return  : 0: 失败，出错
//			 1: 成功返回
int CheckErr(OCIError *pErr, sword lStatus);

//数据库服务名
static text* dbname = (text*) "127.0.0.1:1521/dbsrv2";

//用户及密码
static text* username = (text*) "oci";
static text* password = (text*) "ociOCI1111";

//DML(INSERT)操作的SQL语句
static text* insertstr = (text*) "INSERT INTO date1  VALUES(1, :id2)";
static text* selectstr = (text*) "select id2 from date1 where id1=1";


void PrintErr(void *errhp)
{
    int errcode;
    char errBuf[200] = {0};
    OCIErrorGet((dvoid *)errhp, 1, NULL, &errcode,(text*)errBuf, 
                (ub4)sizeof(errBuf), (ub4)OCI_HTYPE_ERROR);
    fprintf(stderr, "%d, %s\n", errcode, errBuf);
}

int main()
{
	
	OCIEnv *myenvhp; // 环境句柄
	OCIServer *mysrvhp; //服务器句柄
	OCIError *myerrhp; //错误句柄
	OCISession *myusrhp; //用户会话句柄
	OCISvcCtx *mysvchp; //服务上下文句柄
	OCIStmt* stmthp; //语句句柄

	OCIBind* bndhp1 = NULL;
    OCIDefine *defhp1 = NULL;

	sb4 status = 0;
    OCIDate start_date;
    OCIDate get_date;
    OCIDateSetDate(&start_date, 1990,10,5);
    sb2 year;
    ub1 mm;
    ub1 dd;


	//使用线程和对象模式来创建环境句柄
	OCIEnvCreate(&myenvhp, OCI_THREADED|OCI_OBJECT, (dvoid *)0,
		0, 0, 0, (size_t) 0, (dvoid **)0);
	//分配服务器句柄
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&mysrvhp,
		OCI_HTYPE_SERVER, 0, (dvoid **) 0);
	//分配错误句柄
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myerrhp,
		OCI_HTYPE_ERROR, 0, (dvoid **) 0);

	//创建服务器上下文句柄,"orcl"为建立连接的数据库名
	if (OCIServerAttach (mysrvhp, myerrhp, (text *)dbname,
		strlen ((char*)dbname), OCI_DEFAULT) == OCI_SUCCESS)
		printf("\n已经成功连上数据库orcl\n");
	else //终止程序
	{
		printf("\n数据库名字不对，连接数据库失败!\n");
		return -1;
	}
	//分配服务器上下文句柄
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&mysvchp,
		OCI_HTYPE_SVCCTX, 0, (dvoid **) 0);
	//设置服务器上下文句柄的服务器句柄属性
	OCIAttrSet ((dvoid *)mysvchp, OCI_HTYPE_SVCCTX,
		(dvoid *)mysrvhp, (ub4) 0, OCI_ATTR_SERVER, myerrhp);
	//分配用户会话句柄
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myusrhp,
		OCI_HTYPE_SESSION, 0, (dvoid **) 0);
	//为用户会话句柄设置用户名和密码属性
	OCIAttrSet ((dvoid *)myusrhp, OCI_HTYPE_SESSION,
		(dvoid *)username, (ub4)strlen((char*)username),
		OCI_ATTR_USERNAME, myerrhp);
	OCIAttrSet ((dvoid *)myusrhp, OCI_HTYPE_SESSION,
		(dvoid *)password, (ub4)strlen((char*)password),
		OCI_ATTR_PASSWORD, myerrhp);
	if (OCISessionBegin ( mysvchp, myerrhp, myusrhp,
		OCI_CRED_RDBMS, OCI_DEFAULT) == OCI_SUCCESS)
	{
		printf("成功建立用户会话!\n");
	}
	else
	{
		printf("建立用户会话失败!\n");
		return -1;
	}
	//在服务器上下文环境中设置用户会话属性
	OCIAttrSet ( (dvoid *)mysvchp, OCI_HTYPE_SVCCTX,
		(dvoid *)myusrhp, (ub4) 0, OCI_ATTR_SESSION, myerrhp);
	//分配语句句柄
	CheckErr(myerrhp, OCIHandleAlloc(myenvhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, 0));

    //插入数据

	//准备SQL语句
	CheckErr(myerrhp, OCIStmtPrepare(stmthp, myerrhp, insertstr, strlen((char*)insertstr),
		OCI_NTV_SYNTAX, OCI_DEFAULT));

	//位置绑定
	OCIBindByPos(stmthp, &bndhp1, myerrhp, 1, &start_date, 
		sizeof(start_date), SQLT_ODT, 0, 0, 0, 0, 0, OCI_DEFAULT);


	//执行
	status = OCIStmtExecute(mysvchp, stmthp, myerrhp, 1, 0, NULL, NULL,
		OCI_BATCH_ERRORS | OCI_COMMIT_ON_SUCCESS);

    CheckErr(myerrhp, status);
#if 0
	if (status && status != OCI_SUCCESS_WITH_INFO)
	{
        PrintErr(myerrhp);
		OCIHandleFree((dvoid*)myenvhp, OCI_HTYPE_ENV);
		exit(-1);
	}
#endif

	int	rows_processed = 0;
	int err_row = 0; //错误行
	//得到已经成功处理的记录行数
	status = OCIAttrGet(stmthp, OCI_HTYPE_STMT, &rows_processed, 0, OCI_ATTR_ROW_COUNT, myerrhp);
	CheckErr(myerrhp, status);
	printf("已经成功处理记录%d行\n", rows_processed);
 
    //查看数据

	//准备SQL语句
	CheckErr(myerrhp, OCIStmtPrepare(stmthp, myerrhp, selectstr, strlen((char*)selectstr),
		OCI_NTV_SYNTAX, OCI_DEFAULT));

	//位置绑定
    status = OCIDefineByPos(stmthp, &defhp1, myerrhp, 1, &get_date, sizeof(get_date),
                            SQLT_ODT, 0, 0, 0, OCI_DEFAULT);
	CheckErr(myerrhp, status);


	//执行
	status = OCIStmtExecute(mysvchp, stmthp, myerrhp, 1, 0, NULL, NULL,
		OCI_BATCH_ERRORS | OCI_COMMIT_ON_SUCCESS);
	CheckErr(myerrhp, status);

    OCIDateGetDate(&get_date, &year, &mm, &dd);
    fprintf(stdout, "year=%d, month=%d, day=%d\n", year, mm, dd);



	printf("结束会话和数据库连接!\n");
	//结束会话
	OCISessionEnd(mysvchp, myerrhp, myusrhp, OCI_DEFAULT);
	//断开连接
	OCIServerDetach(mysrvhp, myerrhp, OCI_DEFAULT);
	//释放环境句柄
	OCIHandleFree((void*)myenvhp, OCI_HTYPE_ENV);
	return 0;
}

int CheckErr(OCIError *pErr, sword lStatus)
{
    
    sb4		m_s_nErrCode = 0;
	char	m_s_szErr[512];
    
    switch (lStatus)
    {
    case OCI_SUCCESS:
        strcpy(m_s_szErr,"OCI_SUCCESS");    
        break;
    case OCI_SUCCESS_WITH_INFO:
        strcpy(m_s_szErr, "OCI_SUCCESS_WITH_INFO");
		printf("OCI Error: %s\n", m_s_szErr);
        break;
    case OCI_ERROR:
        OCIErrorGet((dvoid *)pErr, (ub4)1, (text *)NULL, &m_s_nErrCode, 
            (unsigned char*)m_s_szErr, (ub4)sizeof(m_s_szErr), OCI_HTYPE_ERROR);
		printf("OCI Error: %s\n", m_s_szErr);
        break;
    case OCI_NEED_DATA:
        strcpy(m_s_szErr, "OCI_NEED_DATA");
		printf("OCI Error: %s\n", m_s_szErr);
        break;
    case OCI_NO_DATA:
        strcpy(m_s_szErr, "OCI_NO_DATA");
		printf("OCI Error: %s\n", m_s_szErr);
        break;
    case OCI_INVALID_HANDLE:
        strcpy(m_s_szErr, "OCI_INVALID_HANDLE");
		printf("OCI Error: %s\n", m_s_szErr);
        break;
    case OCI_STILL_EXECUTING:
        strcpy(m_s_szErr, "OCI_STILL_EXECUTING");
		printf("OCI Error: %s\n", m_s_szErr);
        break;
    case OCI_CONTINUE:
        strcpy(m_s_szErr, "OCI_CONTINUE");
		printf("OCI Error: %s\n", m_s_szErr);
        break;
    default:
        break;
    }
    if (lStatus != OCI_SUCCESS && lStatus != OCI_SUCCESS_WITH_INFO)
    {
        return 0;  //确实有错误
    } 
    else 
    {
        return 1;  //没有检查到错误
    }
}


/*
执行结果

[oracle@host122 testcase]$ ./date1 

已经成功连上数据库orcl
成功建立用户会话!
已经成功处理记录1行
year=1990, month=10, day=5
结束会话和数据库连接!


*/
