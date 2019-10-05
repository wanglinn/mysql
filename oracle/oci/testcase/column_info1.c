/*
* Copyright (c) OCI高级编程
* @File name: chap16_eg2.cpp
* @Author   : He Xiong
* @Content  : 通过表名获取关系表的表结构信息
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

//表名
static text* tablename = (text*) "student";

static char  colname[64] = "";

int main()
{
	
	OCIEnv *envhp;			// 环境句柄
	OCIServer *srvhp;		//服务器句柄
	OCIError *errhp;		//错误句柄
	OCISession *usrhp;		//用户会话句柄
	OCISvcCtx *svchp;		//服务上下文句柄
	

	OCIDescribe* dschp;		//描述句柄
	OCIParam	*parmhp = NULL;	//参数句柄
    OCIParam	*collsthp = NULL;	//列列句柄
	OCIParam	*colhp = NULL;		//列句柄

	OCIBind* bndhp1 = NULL;
	OCIBind* bndhp2 = NULL;


	sb4 status = 0;


	
	
	ub4 numcols = 0; //列个数
	ub2 dtype; //数据类型
	ub2 colsize = (ub2)0;  //列长度
	sb1 colscale = (sb1)0; //列小数位数
	ub1 colprecision = (ub1)0; //列精度
	char szCol[128]; //列名缓存
	text *col_name; //列名
	ub4 col_name_len; //列长度
	

	ub1 ind; //列是否允许为空


	//使用对象模式来创建环境句柄
	OCIEnvCreate(&envhp, OCI_OBJECT, (dvoid *)0,
		0, 0, 0, (size_t) 0, (dvoid **)0);
	//分配服务器句柄
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&srvhp,
		OCI_HTYPE_SERVER, 0, (dvoid **) 0);
	//分配错误句柄
	OCIHandleAlloc ((dvoid *)envhp, (dvoid **)&errhp,
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

	//申请描述句柄
	OCIHandleAlloc((dvoid*)envhp, (dvoid**)&dschp, OCI_HTYPE_DESCRIBE, 0, (void**)0);

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
	
	
	//获取关系表的表结构信息
	printf("获取关系表ocitest.student的表结构信息\n");

	CheckErr(errhp, OCIDescribeAny(svchp, errhp, (dvoid*)tablename, 
		(ub4)strlen((char*)tablename), OCI_OTYPE_NAME, OCI_DEFAULT, 
		OCI_PTYPE_TABLE, dschp));

	//获取参数描述
	CheckErr(errhp, OCIAttrGet(dschp, OCI_HTYPE_DESCRIBE, &parmhp, 
		0, OCI_ATTR_PARAM, errhp));


	//获取表的列(字段)个数
	CheckErr(errhp, OCIAttrGet(parmhp, OCI_DTYPE_PARAM, &numcols,
		0, OCI_ATTR_NUM_COLS, errhp));

	//获取列列表
	CheckErr(errhp, OCIAttrGet(parmhp, OCI_DTYPE_PARAM, &collsthp,
		0, OCI_ATTR_LIST_COLUMNS, errhp));

	printf("名称             空?      类型             列长度   列精度   小数位数\n");
	printf("---------------- -------- ---------------- -------- -------- --------\n");
	//列枚举,注意,它是以1为序
	for (ub4 i=1; i<=numcols; i++)
	{
		//获取一个字段的描述参数
		CheckErr(errhp, OCIParamGet(collsthp, OCI_DTYPE_PARAM, errhp, (void**)&colhp, i));

		//分别获取列名、是否为空、列数据类型、列长度、列精度、列小数点位数
		CheckErr(errhp, OCIAttrGet(colhp, OCI_DTYPE_PARAM, &col_name, &col_name_len, 
			OCI_ATTR_NAME, errhp));
		memcpy(szCol, col_name, col_name_len);
		szCol[col_name_len] = '\0'; //串强制以'\0'结束

		CheckErr(errhp, OCIAttrGet(colhp, OCI_DTYPE_PARAM, &ind, 0, 
			OCI_ATTR_IS_NULL, errhp));

		CheckErr(errhp, OCIAttrGet(colhp, OCI_DTYPE_PARAM, &dtype, 0, 
			OCI_ATTR_DATA_TYPE, errhp));

		CheckErr(errhp, OCIAttrGet(colhp, OCI_DTYPE_PARAM, &colsize, 0, 
			OCI_ATTR_DATA_SIZE, errhp));

		CheckErr(errhp, OCIAttrGet(colhp, OCI_DTYPE_PARAM, &colprecision, 0, 
			OCI_ATTR_PRECISION, errhp));

		CheckErr(errhp, OCIAttrGet(colhp, OCI_DTYPE_PARAM, &colscale, 0, 
			OCI_ATTR_SCALE, errhp));
		
		//输出相关信息
		printf("%-17s", szCol);
		if (ind == 0) { printf("NOT NULL "); }
		else { printf("         "); }
		printf("%-17s", GetType(dtype));
		printf("%-9d", colsize);
		printf("%-9d", colprecision);
		printf("%-9d\n", colscale);
	}

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
        //return 0;  //确实有错误
		exit(-1);
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
