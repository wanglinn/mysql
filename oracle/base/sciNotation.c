#include <stdio.h>
#include <string.h>

//https://docs.oracle.com/cd/E57185_01/HIRUG/ch02s03s02.html

typedef int bool;

#define   true      1
#define   false     0

typedef struct StNumber2SciInfo
{
    int flagLen;
    int integerLen;
    int decimalLen;
    int numwidth;
    int strLen;
    bool isDecimalNum;
    bool isNeedRound;
}Number2SciInfo;

//判断是否需要使用科学计数法
bool isUseSciNotationFormat(Number2SciInfo* sciInfo)
{
    if (sciInfo->integerLen > sciInfo->numwidth) {
        return true;
    }
    return false;
}

//获取字符串符号位、整数长度、小数长度
void getIntegerLenDecimalLenFlagLen(const char* sourceStr, Number2SciInfo* sciInfo)
{
    int i = 0;
    char decimalPoint = '.';

    sciInfo->numwidth = (sourceStr[0] == '-' ? 1 : 0);
    while (sourceStr[i] != decimalPoint && sourceStr[i] != '\0') {
        i++;
    }
    sciInfo->integerLen = i - sciInfo->flagLen;
    sciInfo->decimalLen = sciInfo->strLen - sciInfo->flagLen - sciInfo->integerLen;
    sciInfo->isDecimalNum = (sciInfo->integerLen == 0 || (sciInfo->integerLen == 1 && sourceStr[sciInfo->flagLen] == '0'));
}

//非科学计数法格式，是否需要进行进位处理
void isNumberNeedRoundForNoSciFormat(const char *sourceStr, Number2SciInfo *sciInfo)
{
    if (sciInfo->strLen <= sciInfo->numwidth) {
        sciInfo->isNeedRound = false;
    }
    if (sourceStr[sciInfo->numwidth] > '5' ||
        (sourceStr[sciInfo->numwidth] == '.' && sourceStr[sciInfo->numwidth + 1] > '5')) {
        sciInfo->isNeedRound = true;
    }

    sciInfo->isNeedRound = false;
}

//需要进位，用非科学计数法显示
void transStringRoundNoSciFormat(const char *sourceStr, char *destStr, Number2SciInfo *sciInfo)
{
    char pStrTemp[100] = {0};
    int i = 0;
    int j = 100-1;
    int carry = 1;
    pStrTemp[100-1] = '\0';
    i = sciInfo->numwidth - 1;
    j = 100 - 2;

    while (i >= sciInfo->flagLen)
    {
        if (sourceStr[i] != '.') {
            pStrTemp[j] = sourceStr[i] + carry;
        } else {
            pStrTemp[j] = sourceStr[i];
            i--;
            j--;
            continue;
        }
        carry = 0;
        if (pStrTemp[j] > '9') {
            pStrTemp[j] = pStrTemp[j] - 10;
            carry = 1;
        }
        i--;
        j--;
        }
        if (carry > 0) {
            pStrTemp[j--] = '0' + carry;
        }
        if (sciInfo->flagLen > 0) {
            pStrTemp[j--] = '-';
        }
    }

    if (pStrTemp[100-2] == '.') {
        pStrTemp[100-2] = '\0'
    }
    strncpy(destStr, pStrTemp + j, sciInfo->numwidth);
}

void transStringNoRoundNoSciFormat(const char *sourceStr, char *destStr, Number2SciInfo *sciInfo)
{
    strncpy(destStr, sourceStr, sciInfo->numwidth);
}

void transStringUseSciFormat(const char *sourceStr, char *destStr, Number2SciInfo *sciInfo)
{
    int expLen = 0;
    int exponent = 0;
    int baseIntLen = 1;
    int basePointLen = 1;
    int baseDecimalLen = 0;
    int expSpecialCharLen = 2;
    int expLen = 2;
    int checkPos = sciInfo->numwidth - 4;

    //移动小数点的长度
    int i = 0;
    int j = 0;
    char strTemp1[100] = {0};
    if (sciInfo->flagLen == 1) {
        strTemp1[j++] = sourceStr[i++];
    }
    if (sourceStr[i] == '0' && sourceStr[i+1] == '.')
    {
        strTemp1[j++] = '.';
        i = i + 1;
    }

    while (i < sciInfo->numwidth){
        strTemp1[j++] = sourceStr[i++];
    }

    int exp = 0;
    int integerActiveLen = sciInfo->integerLen;
    if (sciInfo->integerLen == 1 && sourceStr[sciInfo->flagLen] == '0') {
        integerActiveLen = 0;
    }
    if (!sciInfo->isDecimalNum)
    {
        exp = integerActiveLen;
    }

        int startPos = 0;
    int endPos = 0;
    i = sciInfo->strLen - 1;
    while(i >= flagLen) {

    }

        if (!sciInfo->isDecimalNum)
        {
            if (sciInfo->integerLen >= 100)
            {
                expLen = 3;
                checkPos = sciInfo->numwidth - 5;
            }
            else if (sciInfo->integerLen == 99)
            {
            if (sourceStr[sciInof->numwidth -4] > '5' || 
                (sourceStr[sciInof->numwidth -4] == '.' && sourceStr[sciInof->numwidth -5] > '5') {
                expLen = 3;
            }
        }
    } else {

    }


}

void transString2SciNotation(char *sourceStr, char *destStr, int numwidth)
{
    Number2SciInfo sciInfo;

    sciInfo.numwidth = numwidth;
    sciInfo.strLen = strlen(sourceStr);
    //获取字符串对应的符号长度、整数长度、小数长度、字符串长度信息
    getIntegerLenDecimalLenFlagLen(sourceStr, &sciInfo);
    isNumberNeedRoundForNoSciFormat(sourceStr, &sciInfo);

    //使用科学计数法
    if (isUseSciNotationFormat(&sciInfo)){
        return transStringUseSciFormat(sourceStr, destStr, &sciInfo);
    }

    if (sciInfo->isNeedRound) {
        return transStringRoundNoSciFormat(sourceStr, destStr, &sciInfo);
    }
    else {
        return transStringNoRoundNoSciFormat(sourceStr, destStr, &sciInfo);
    }
}


