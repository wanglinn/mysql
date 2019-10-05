/*
* Copyright (c) OCI高级编程
* @File name: chap15_eg3.cpp
* @Author   : He Xiong
* @Content  : 隐式描述一个选择列表
* 创建日期：2003-10
****/

/*
 * setup : create table student(id int, name char(20), age int);
 *
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


//获取类型nType对应的Oracle内部类型表示
//@param   : nType, 列类型的码值
//@return  : 列类型
char* GetType(ub2 nType);


//数据库服务名
static text* dbname = (text*) "127.0.0.1:1521/dbsrv2";

//用户及密码
static text* username = (text*) "user";
static text* password = (text*) "password";

//DML(INSERT)操作的SQL语句
static text* selectstr = (text*) "SELECT * FROM student";

static char  colname[64] = "";

int main()
{
	
	OCIEnv *envhp; // 环境句柄
	OCIServer *srvhp; //服务器句柄
	OCIError *errhp; //错误句柄
	OCIError *errhp1; //错误句柄
	OCIError *errhp2; //错误句柄
	OCISession *usrhp; //用户会话句柄
	OCISvcCtx *svchp; //服务上下文句柄
	OCIStmt* stmthp; //语句句柄

	OCIBind* bndhp1 = NULL;
	OCIBind* bndhp2 = NULL;


	sb4 status = 0;


	OCIParam *paramhp; //参数句柄指针
	ub4 counter;
	ub2 dtype; //数据类型
	char szCol[128]; //列名缓存
	text *col_name; //列名
	ub4 col_name_len; //列长度
	sb4 parm_status; //状态码


	//使用线程和对象模式来创建环境句柄
	OCIEnvCreate(&envhp, OCI_THREADED|OCI_OBJECT, (dvoid *)0,
		0, 0, 0, (size_t) 0, (dvoid **)0);
	//分配服务器句柄
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&srvhp,
		OCI_HTYPE_SERVER, 0, (dvoid **) 0);
	//分配错误句柄
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&errhp,
		OCI_HTYPE_ERROR, 0, (dvoid **) 0);
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&errhp1,
		OCI_HTYPE_ERROR, 0, (dvoid **) 0);
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&errhp2,
		OCI_HTYPE_ERROR, 0, (dvoid **) 0);

	//创建服务器上下文句柄,"orcl"为建立连接的数据库名
	if (OCIServerAttach (srvhp, errhp, (text *)dbname,
		strlen ((char*)dbname), OCI_DEFAULT) == OCI_SUCCESS)
		printf("\n已经成功连上数据库orcl\n");
	else //终止程序
	{
		printf("\n数据库名字不对，连接数据库失败!\n");
		return -1;
	}
	//分配服务器上下文句柄
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&svchp,
		OCI_HTYPE_SVCCTX, 0, (dvoid **) 0);
	//设置服务器上下文句柄的服务器句柄属性
	OCIAttrSet ((dvoid *)svchp, OCI_HTYPE_SVCCTX,
		(dvoid *)srvhp, (ub4) 0, OCI_ATTR_SERVER, errhp);
	//分配用户会话句柄
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&usrhp,
		OCI_HTYPE_SESSION, 0, (dvoid **) 0);
	//为用户会话句柄设置用户名和密码属性
	OCIAttrSet ((dvoid *)usrhp, OCI_HTYPE_SESSION,
		(dvoid *)username, (ub4)strlen((char*)username),
		OCI_ATTR_USERNAME, errhp);
	OCIAttrSet ((dvoid *)usrhp, OCI_HTYPE_SESSION,
		(dvoid *)password, (ub4)strlen((char*)password),
		OCI_ATTR_PASSWORD, errhp);
	if (OCISessionBegin ( svchp, errhp, usrhp,
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
	OCIAttrSet ( (dvoid *)svchp, OCI_HTYPE_SVCCTX,
		(dvoid *)usrhp, (ub4) 0, OCI_ATTR_SESSION, errhp);
	//分配语句句柄
	CheckErr(errhp, OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, 0));
	//准备SQL语句
	CheckErr(errhp, OCIStmtPrepare(stmthp, errhp, selectstr, strlen((char*)selectstr),
		OCI_NTV_SYNTAX, OCI_DEFAULT));

	//获取选择表的元信息
	printf("获取选择表的元信息\n");
	CheckErr(errhp, OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
		OCI_DESCRIBE_ONLY));

	counter = 1;
	parm_status = OCIParamGet(stmthp, OCI_HTYPE_STMT, errhp, (void**)&paramhp,
		(ub4) counter);
	memset(szCol, 0, sizeof(szCol));
	//循环获取
	while (parm_status==OCI_SUCCESS) {
		//获取数据类型属性
		CheckErr(errhp, OCIAttrGet((dvoid*) paramhp, (ub4) OCI_DTYPE_PARAM,
			(dvoid*) &dtype,(ub4 *) 0, (ub4) OCI_ATTR_DATA_TYPE,(OCIError *) errhp ));
		//获取列名属性
		CheckErr(errhp, OCIAttrGet((dvoid*) paramhp, (ub4) OCI_DTYPE_PARAM,
			(dvoid**) &col_name,(ub4 *) &col_name_len, (ub4) OCI_ATTR_NAME,
			(OCIError *) errhp ));
		memcpy(szCol, col_name, col_name_len);
		printf("%d\t列名:%s\t\t数据类型:%s\n", counter, szCol, GetType(dtype));
		memset(szCol, 0, sizeof(szCol));
		counter++;
		parm_status = OCIParamGet(stmthp, OCI_HTYPE_STMT, errhp, (dvoid**)&paramhp,
			(ub4) counter);
	}

	printf("共得到选择列表列数为:%d\n", counter-1);


	printf("结束会话和数据库连接!\n");
	//结束会话
	OCISessionEnd(svchp, errhp, usrhp, OCI_DEFAULT);
	//断开连接
	OCIServerDetach(srvhp, errhp, OCI_DEFAULT);
	//释放环境句柄
	OCIHandleFree((void*)envhp, OCI_HTYPE_ENV);
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


char* GetType(ub2 nType)
{
	char* str = colname;
	switch(nType)
	{
	case SQLT_AFC:
	case SQLT_VCS:
	case SQLT_CHR:
		strcpy(str, "VARCHAR2");
		break;
	case SQLT_DAT:
		strcpy(str, "DATE");
		break;
	case SQLT_INT:
		strcpy(str, "INTEGER");
		break;
	case SQLT_UIN:
		strcpy(str, "INTEGER");
		break;
	case SQLT_FLT:
		strcpy(str, "FLOAT");
		break;
	case SQLT_BLOB:
	case SQLT_BIN:
		strcpy(str, "BLOB");
		break;
	case SQLT_NUM:
		strcpy(str, "NUMBER");
		break;
	default:
		strcpy(str, "UNSUPPORTED");
		break;
	}
	return str;
}

执行结果：
[oracle@host122 testcase]$ ./column2

已经成功连上数据库orcl
成功建立用户会话!
获取选择表的元信息
1       列名:ID         数据类型:NUMBER
2       列名:NAME               数据类型:VARCHAR2
3       列名:AGE                数据类型:NUMBER
共得到选择列表列数为:3
结束会话和数据库连接!
