/*
* Copyright (c) OCI高级编程
* @File name: chap17_eg1.cpp
* @Author   : He Xiong
* @Content  : 读写LOB型数据
* 创建日期：2003-10
****/

/* setup : create table testblob (id int, vb blob);
 *
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oci.h>
#include "ocilib.h"
#include "ociap.h"

//获取错误诊断信息
//@param   : pErr,错误句柄
//           lStatus, 状态码信息
//@return  : 0: 失败，出错
//                       1: 成功返回
int CheckErr(OCIError *pErr, sword lStatus);

//数据库服务名
static text* dbname = (text*) "127.0.0.1:1521/dbsrv2";

//用户及密码
static text* username = (text*) "oci";
static text* password = (text*) "ociOCI1111";

static text* insert1 = (text*) "INSERT INTO testblob VALUES(1, empty_blob())";
static text* update1 = (text*) "SELECT vb FROM testblob WHERE id=1 FOR UPDATE";
static text* select1 = (text*) "SELECT vb FROM testblob WHERE id=1";
static char szblob[33]="我是中国人, 我爱中国";
static ub4 nLen = sizeof(szblob);

static char* szV = NULL;

int main()
{

        OCIEnv *envhp; // 环境句柄
        OCIServer *srvhp; //服务器句柄
        OCIError *errhp; //错误句柄
        OCISession *usrhp; //用户会话句柄
        OCISvcCtx *svchp; //服务上下文句柄
        OCIStmt* stmthp; //语句句柄
        OCILobLocator* blob; //LOB定位符

        ub4 status;
        ub4 len = 0;

        OCIBind* bndhp1 = NULL;
        OCIDefine* defhp1 = NULL;
        //使用线程和对象模式来创建环境句柄
        OCIEnvCreate(&envhp, OCI_THREADED|OCI_OBJECT, (dvoid *)0,
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

        //分配LOB定位符句柄
        OCIDescriptorAlloc(envhp, (void**)&blob, OCI_DTYPE_LOB, 0, (dvoid**)0);

        //分配语句句柄
        CheckErr(errhp, OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, 0));
        //准备第一个SQL语句
        CheckErr(errhp, OCIStmtPrepare(stmthp, errhp, insert1, strlen((char*)insert1),
                OCI_NTV_SYNTAX, OCI_DEFAULT));

        //插入第一条记录
        status = OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
                OCI_DEFAULT);

        if (status && status != OCI_SUCCESS_WITH_INFO)
        {
                OCIHandleFree((dvoid*)envhp, OCI_HTYPE_ENV);
                return -1;
        }
        printf("插入记录完成\n");
        printf("开始更新blob字段值\n");

        //准备更新blob
        CheckErr(errhp, OCIStmtPrepare(stmthp, errhp, update1, strlen((char*)update1),
                OCI_NTV_SYNTAX, OCI_DEFAULT));

        CheckErr(errhp, OCIDefineByPos(stmthp, &defhp1, errhp, 1, &blob, -1,
                SQLT_BLOB, 0,0, 0, OCI_DEFAULT));
        //准备写入数据
        status = OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
                OCI_DEFAULT);
        if (status && status != OCI_SUCCESS_WITH_INFO)
        {
                OCIHandleFree((dvoid*)envhp, OCI_HTYPE_ENV);
                return -1;
        }
        OCILobOpen(svchp, errhp, blob, OCI_LOB_READWRITE);
        OCILobGetLength(svchp, errhp, blob, &len);
        printf("写入前, blob长度=%d\n", len);
        CheckErr(errhp, OCILobWrite(svchp, errhp, blob, &nLen, 1, szblob, sizeof(szblob), 
                OCI_ONE_PIECE, 0, 0, 0, SQLCS_IMPLICIT));
        OCILobGetLength(svchp, errhp, blob, &len);
        printf("写入后, blob长度=%d\n", len);
        OCILobClose(svchp, errhp, blob);

        //准备读取blob字段的值
        printf("开始读取blob字段内容\n");
        CheckErr(errhp, OCIStmtPrepare(stmthp, errhp, select1, strlen((char*)select1),
                OCI_NTV_SYNTAX, OCI_DEFAULT));
        CheckErr(errhp, OCIDefineByPos(stmthp, &defhp1, errhp, 1, &blob, -1,
                SQLT_BLOB, 0,0, 0, OCI_DEFAULT));
        //准备读出数据
        status = OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
                OCI_DEFAULT);
        if (status && status != OCI_SUCCESS_WITH_INFO)
        {
                OCIHandleFree((dvoid*)envhp, OCI_HTYPE_ENV);
                return -1;
        }
        OCILobOpen(svchp, errhp, blob, OCI_LOB_READONLY);
        OCILobGetLength(svchp, errhp, blob, &len);
        if (len > 0) szV = new char[len];
        CheckErr(errhp, OCILobRead(svchp, errhp, blob, &nLen, 1, szV, len, 
                OCI_ONE_PIECE, 0, 0, SQLCS_IMPLICIT));
        printf("blob内容为: %s\n", szV);
        OCILobClose(svchp, errhp, blob);
        if (szV) delete[] szV;
        //释放定位符空间
        OCIDescriptorFree(blob, OCI_DTYPE_LOB);
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
    
    sb4         m_s_nErrCode = 0;
        char    m_s_szErr[512];
    
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
        exit(-1);  //确实有错误
    } 
    else 
    {
        return 1;  //没有检查到错误
    }
}

执行结果：
[oracle@host122 testcase]$ ./blob1

已经成功连上数据库orcl
成功建立用户会话!
插入记录完成
开始更新blob字段值
写入前, blob长度=33
写入后, blob长度=33
开始读取blob字段内容
blob内容为: 我是中国人, 我爱中国

