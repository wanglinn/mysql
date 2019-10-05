/*
* Copyright (c) OCI高级编程
* @File name: oci_eg2.cpp
* @Author   : He Xiong
* @Content  : OCI批错误处理模式的使用
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

//数据库服务名
static text* dbname = (text*) "127.0.0.1:1521/dbsrv2";

//用户及密码
static text* username = (text*) "user";
static text* password = (text*) "password";

//DML(INSERT)操作的SQL语句
static text* insert = (text*) "INSERT INTO STUDENT(id, name, age)"
			" VALUES(:1, :name, 22)";

int main()
{
	
	OCIEnv *myenvhp; // 环境句柄
	OCIServer *mysrvhp; //服务器句柄
	OCIError *myerrhp; //错误句柄
	OCIError *myerrhp1; //错误句柄
	OCIError *myerrhp2; //错误句柄
	OCISession *myusrhp; //用户会话句柄
	OCISvcCtx *mysvchp; //服务上下文句柄
	OCIStmt* stmthp; //语句句柄

	OCIBind* bndhp1 = NULL;
	OCIBind* bndhp2 = NULL;

	int	nSID[5] = {1, 2, 1, 1, 3}; //学号
	char szName[5][32] = {"Bob", "Ying", "HeXiong", "Jenny", "Mary" }; //姓名
	sb4 nNameLen = 32;
	sb4 status = 0;


	//使用线程和对象模式来创建环境句柄
	OCIEnvCreate(&myenvhp, OCI_THREADED|OCI_OBJECT, (dvoid *)0,
		0, 0, 0, (size_t) 0, (dvoid **)0);
	//分配服务器句柄
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&mysrvhp,
		OCI_HTYPE_SERVER, 0, (dvoid **) 0);
	//分配错误句柄
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myerrhp,
		OCI_HTYPE_ERROR, 0, (dvoid **) 0);
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myerrhp1,
		OCI_HTYPE_ERROR, 0, (dvoid **) 0);
	OCIHandleAlloc ((dvoid *)myenvhp, (dvoid **)&myerrhp2,
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
	//准备SQL语句
	CheckErr(myerrhp, OCIStmtPrepare(stmthp, myerrhp, insert, strlen((char*)insert),
		OCI_NTV_SYNTAX, OCI_DEFAULT));

	//位置绑定
	OCIBindByPos(stmthp, &bndhp1, myerrhp, 1, &nSID, 
		sizeof(nSID[0]), SQLT_INT, 0, 0, 0, 0, 0, OCI_DEFAULT);

	//名字绑定
	OCIBindByName(stmthp, &bndhp2, myerrhp, (text*)":name", -1, &szName, 
		sizeof(szName[0]), SQLT_STR, 0, 0, 0, 0, 0, OCI_DEFAULT);

	//执行批量插入
	status = OCIStmtExecute(mysvchp, stmthp, myerrhp, 5, 0, NULL, NULL,
		OCI_BATCH_ERRORS | OCI_COMMIT_ON_SUCCESS);

	if (status && status != OCI_SUCCESS_WITH_INFO)
	{
		OCIHandleFree((dvoid*)myenvhp, OCI_HTYPE_ENV);
		exit(-1);
	}

	int	rows_processed = 0;
	int err_row = 0; //错误行
	//得到已经成功处理的记录行数
	OCIAttrGet(stmthp, OCI_HTYPE_STMT, &rows_processed, 0, OCI_ATTR_ROW_COUNT, myerrhp);
	
	printf("已经成功处理记录%d行\n", rows_processed);
	//如果还有记录未被插入
	if (5-rows_processed != 0)
	{
		for (int i=0; i<5-rows_processed; i++)
		{
			//获取错误信息
			OCIParamGet(myerrhp, OCI_HTYPE_ERROR, myerrhp2, (void**)&myerrhp1, i);
			//获取具体错误的记录行
			OCIAttrGet(myerrhp1, OCI_HTYPE_ERROR, &err_row, 0, OCI_ATTR_DML_ROW_OFFSET, myerrhp);
			//显示错误信息
			printf("第%d行记录插入发生错误,具体错误为:\n", err_row+1);
			CheckErr(myerrhp1, OCI_ERROR);

		}
		//提交成功处理的记录行
		if (OCITransCommit(mysvchp, myerrhp2, 0) != 0)
			CheckErr(myerrhp2, status);
	}


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
