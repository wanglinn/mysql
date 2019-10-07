/*

setup 
 create or replace procedure myDemo05(name out varchar,age in int)
as
begin
       dbms_output.put_line('age='||age);
  select 'xiaoming' into name from dual;
end;
/
 
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
static text* username = (text*) "oci";
static text* password = (text*) "ociOCI1111";

//DML(INSERT)操作的SQL语句
static text* selectstr = (text*) "SELECT * FROM student";

static char  colname[64] = "";

#define PrintErr(errhp) \
{ \
    int errcode; \
    char errBuf[200] = {0}; \
    OCIErrorGet((dvoid *)errhp, 1, NULL, &errcode,(text*)errBuf,  \
                (ub4)sizeof(errBuf), (ub4)OCI_HTYPE_ERROR); \
    fprintf(stderr, "%d, %s. line %d\n", errcode, errBuf, __LINE__); \
} \

#define RESULT_SUCCESS(status) (status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO)


int main()
{
	
	OCIEnv *envhp; // 环境句柄
	OCIServer *srvhp; //服务器句柄
	OCIError *errhp; //错误句柄
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
	sb4 iRet; //状态码


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
	//分配语句句柄
	CheckErr(errhp, OCIHandleAlloc(envhp, (void**)&stmthp, OCI_HTYPE_STMT, 0, 0));
	//准备SQL语句
	CheckErr(errhp, OCIStmtPrepare(stmthp, errhp, selectstr, strlen((char*)selectstr),
		OCI_NTV_SYNTAX, OCI_DEFAULT));

    int i = 0, j = 0;
    text objptr[] = "myDemo05"; /* the name of a procedure to be described */
    ub4 objp_len = (ub4)strlen((char *)objptr);
    ub2 numargs = 0, numargs1, pos, level;
    text *name, *name1;
    ub4 namelen, namelen1;
    OCIParam *parmh = (OCIParam *) 0; /* parameter handle */
    OCIParam *arglst = (OCIParam *) 0; /* list of args */
    OCIParam *arg = (OCIParam *) 0; /* argument handle */
    OCIParam *arglst1 = (OCIParam *) 0; /* list of args */
    OCIParam *arg1 = (OCIParam *) 0; /* argument handle */
    OCIDescribe *dschp = (OCIDescribe *)0; /* describe handle */


    iRet = OCIHandleAlloc((void *)envhp, (void **)&dschp,
                    (ub4)OCI_HTYPE_DESCRIBE, (size_t)0, (void **)0);
    if (!RESULT_SUCCESS(iRet))
    {
        fprintf(stderr, "error , line %d\n", __LINE__);
        return -1;
    }


    /* get the describe handle for the procedure */
    if (OCIDescribeAny(svchp, errhp, (void *)objptr, objp_len, OCI_OTYPE_NAME, 0,
                        OCI_PTYPE_PROC, dschp))
    {
        fprintf(stderr, "error , line %d\n", __LINE__);
        return OCI_ERROR;
    }

    /* get the parameter handle */
    if (OCIAttrGet((void *)dschp, OCI_HTYPE_DESCRIBE, (void *)&parmh, (ub4 *)0,
                    OCI_ATTR_PARAM, errhp))
    {
        fprintf(stderr, "error , line %d\n", __LINE__);
        return OCI_ERROR;
    }
    /* Get the number of arguments and the arg list */
    if (OCIAttrGet((void *)parmh, OCI_DTYPE_PARAM, (void *)&arglst,
                   (ub4 *)0, OCI_ATTR_LIST_ARGUMENTS, errhp))
    {
        fprintf(stderr, "error , line %d\n", __LINE__);
        return OCI_ERROR;
    }

    if (OCIAttrGet((void *)arglst, OCI_DTYPE_PARAM, (void *)&numargs, (ub4 *)0,
                    OCI_ATTR_NUM_PARAMS, errhp))
    {
        fprintf(stderr, "error , line %d\n", __LINE__);
        return OCI_ERROR;
    }
    
    fprintf(stdout, "numargs = %d\n", numargs);

    /* For a procedure, we begin with i = 1; for a
    function, we begin with i = 0. */
    for (i = 1; i <= numargs; i++)
    {
        OCIParamGet ((void *)arglst, OCI_DTYPE_PARAM, errhp, (void **)&arg, (ub4)i);
        namelen = 0;
        OCIAttrGet((void *)arg, OCI_DTYPE_PARAM, (void *)&name, (ub4 *)&namelen,
                    OCI_ATTR_NAME, errhp);

        fprintf(stdout, "name = %s, namelen = %d\n", name, namelen);

        /* to print the attributes of the argument of type record
        (arguments at the next level), traverse the argument list */
        OCIAttrGet((void *)arg, OCI_DTYPE_PARAM, (void *)&arglst1, (ub4 *)0,
                    OCI_ATTR_LIST_ARGUMENTS, errhp);
        /* check if the current argument is a record. For arg1 in the procedure
        arglst1 is NULL. */
        if (arglst1)
        {
            numargs1 = 0;
            OCIAttrGet((void *)arglst1, OCI_DTYPE_PARAM, (void *)&numargs1, (ub4 *)0,
                        OCI_ATTR_NUM_PARAMS, errhp);

            fprintf(stdout, "numargs1 = %d\n", numargs1);

            /* Note that for both functions and procedures,the next higher level
            arguments start from index 1. For arg2 in the procedure, the number of
            arguments at the level 1 would be 2 */
            for (j = 1; j <= numargs1; j++) 
            {
                OCIParamGet((void *)arglst1, OCI_DTYPE_PARAM, errhp, (void **)&arg1,
                            (ub4)j);
                namelen1 = 0;
                OCIAttrGet((void *)arg1, OCI_DTYPE_PARAM, (void *)&name1, 
                            (ub4 *)&namelen1, OCI_ATTR_NAME, errhp);

                fprintf(stdout, "name1 = %s, namelen1 = %d\n", name1, namelen1);
            }
        }
    }

    if (dschp)
        OCIHandleFree((void *) dschp, OCI_HTYPE_DESCRIBE);

#if 0
	//获取选择表的元信息
	printf("获取选择表的元信息\n");
	CheckErr(errhp, OCIStmtExecute(svchp, stmthp, errhp, 1, 0, NULL, NULL,
		OCI_DESCRIBE_ONLY));

    //获取执行语句对应的列数
    int parmcnt = 0;
    iRet = OCIAttrGet ((void *)stmthp, (ub4)OCI_HTYPE_STMT, (void *)
                      &parmcnt, (ub4 *) 0, (ub4)OCI_ATTR_PARAM_COUNT, errhp);
    fprintf(stdout, "get the sql column number = %d\n", parmcnt);

	counter = 1;
	iRet = OCIParamGet(stmthp, OCI_HTYPE_STMT, errhp, (void**)&paramhp,
		(ub4) counter);
	memset(szCol, 0, sizeof(szCol));
	//循环获取
	while (iRet==OCI_SUCCESS) {
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
		iRet = OCIParamGet(stmthp, OCI_HTYPE_STMT, errhp, (dvoid**)&paramhp,
			(ub4) counter);
	}

	printf("共得到选择列表列数为:%d\n", counter-1);
#endif

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

/*

执行结果

[oracle@host122 testcase]$ ./procedure1 

已经成功连上数据库orcl
成功建立用户会话!
numargs = 2
name = NAME, namelen = 4
name = AGE, namelen = 3
结束会话和数据库连接!




*/
