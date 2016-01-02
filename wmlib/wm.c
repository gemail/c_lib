#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <basetype.h>
#include <list.h>

#define HASHTABLESIZE (256*256)
#define MAXLEN 256

typedef struct wm_pattern_struct
{
    struct wm_pattern_struct *pstNext;
    UCHAR *pucPat; //pattern array
    UINT uiLen; //length of pattern in bytes
} WM_PATTERN_STRUCT;

#define HASH_TYPE USHORT

#define SHIFTTABLESIZE (256*256)

typedef struct wm_struct
{
    WM_PATTERN_STRUCT *pstPatlist; //pattern list
    WM_PATTERN_STRUCT *pstPatArray; //array of patterns
    HASH_TYPE *pusHash; //last 2 characters pattern hash table
    HASH_TYPE *pusPrefix; //first 2 characters prefix table
    USHORT *pusNumArray; //array of group counts, # of patterns in each hash group
    UCHAR *pucShift; //bad word shift table
    UINT uiNumHashEntries;
    INT iNumPatterns; //number of patterns loaded
    INT iSmallest; //shortest length of all patterns
} WM_STRUCT;

INT nline = 1;
INT nfound = 0;

//模式串的最大长度MAXN - 1
#define MAXN 10001

//单词最大长度为MAXM - 1
#define MAXM 51

/******************************************************************************
 *    Function: wmNew
 *      Author: Mr.sancho
 *        Date: 2014年6月15日
 * Description:
 *   Arguments: void
 *
 *      Return: WM_STRUCT*
 *****************************************************************************/
WM_STRUCT *wmNew(VOID)
{
    WM_STRUCT *pstWm = (WM_STRUCT *)malloc(sizeof(WM_STRUCT));
    if (NULL == pstWm)
    {
        return NULL;
    }

    memset(pstWm, 0, sizeof(WM_STRUCT));

    pstWm->iNumPatterns = 0; //模式串的个数,初始为0
    pstWm->iSmallest = 1000; //最短模式串的长度
    return pstWm;
}

/******************************************************************************
 * FunctionName: wmFree
 *       Author: Mr.sancho
 *         Date: 2014年6月15日
 *  Description: 释放模式串集占用空间
 *    Arguments: WM_STRUCT *ps,模式串集
 *
 *       Return: void
 *****************************************************************************/
VOID wmFree(WM_STRUCT *pstWm) //释放空间函数
{
    if (pstWm->pstPatArray) //如果模式串集中存在子串，则先释放子串数组占用空间
    {
        if (pstWm->pstPatArray->pucPat)
            free(pstWm->pstPatArray->pucPat); //子串不为空，则释放
        free(pstWm->pstPatArray);
    }

    if (pstWm->pusNumArray)
        free(pstWm->pusNumArray);
    if (pstWm->pusHash)
        free(pstWm->pusHash);
    if (pstWm->pusPrefix)
        free(pstWm->pusPrefix);
    if (pstWm->pucShift)
        free(pstWm->pucShift);
    free(pstWm);
}

/******************************************************************************
 * FunctionName: wmAddPattern
 *       Author: Mr.sancho
 *         Date: 2014年6月15日
 *    Arguments: WM_STRUCT *pstWm, => 模式串集
 *    		 UCHAR *pcNewStr, => 要新增的子串
 *    		 INT iLength, 子串长度
 *  Description: 向模式串集pstWm中新增一个长度为m的子串q
 *
 *       Return: int
 *****************************************************************************/
STATIC INT wmAddPattern(WM_STRUCT *pstWm, UCHAR *pcNewStr, INT iLength)
{
    WM_PATTERN_STRUCT *pstPattenWm; //定义一个子串结构
    UCHAR *pucPatten;

    pstPattenWm = (WM_PATTERN_STRUCT *)malloc(sizeof(WM_PATTERN_STRUCT));
    if (NULL == pstPattenWm)
    {
        return -1;
    }

    pucPatten = (UCHAR *)malloc(iLength + 1);
    if (NULL == pucPatten)
    {
        free(pstPattenWm);
        return -1;
    }

    memset(pstPattenWm, 0, sizeof(WM_PATTERN_STRUCT));

    pucPatten[iLength] = 0;
    memcpy(pucPatten, pcNewStr, iLength);

    pstPattenWm->pucPat = pucPatten; //据子串数组的长度分配空间
    pstPattenWm->uiLen = iLength; //子串长度赋值

    pstWm->pstPatlist = pstPattenWm;
    pstPattenWm->pstNext = pstWm->pstPatlist; //将新增子串加入字符串集列表中。队列形式，新增在队列头部
    pstWm->iNumPatterns++; //模式串集的子串个数增1

    if (pstPattenWm->uiLen < (UINT)pstWm->iSmallest)
    {
        pstWm->iSmallest = pstPattenWm->uiLen; //重新确定最短字符串长度
    }

    return 0;
}

/* ****************************************************************
 函数：static unsigned HASH16(UCHAR *)
 目的：对一串字符进行哈希计算。计算方式为：(((*T)<<8) | *(T+1))，
 参数：
 T => 要哈希计算的字符串
 返回：
 unsigned - 静态函数，返回对字符串T计算的哈希值
 ****************************************************************/
static UINT HASH16(UCHAR *T)
{
    /*/
     printf("T:%c\n",*(T));
     getchar();
     printf("T+1:%c\n",*(T+1));
     getchar();
     printf("T<<8:%c\n",(int)((*T)<<8));
     getchar();
     printf("HASH16:%d\n",((*T)<<8) | *(T+1));
     getchar();
     //*/
    return (USHORT)(((*T) << 8) | *(T + 1)); //对第一个字符左移8位，然后与第二个字符异或运算
}

/* ****************************************************************
 函数：sort(WM_STRUCT *)
 目的：对字符串集ps中的子串队列，根据子串串值的哈希值从小到大排序
 参数：
 ps => 模式串集
 返回：无
 ****************************************************************/
VOID sort(WM_STRUCT *ps)
{
    INT m = ps->iSmallest; //获取最短子串长度
    INT i, j;
    UCHAR *temp;
    INT flag; //冒泡排序的标志位。当一趟比较无交换时，说明已经完成排序，即可以跳出循环结束
    for (i = ps->iNumPatterns - 1, flag = 1; i > 0 && flag; i--) //循环对字符串集中的每个子串，根据其哈希值大小进行冒泡排序
    {
        flag = 0;
        for (j = 0; j < i; j++)
        {
            if (HASH16(&(ps->pstPatArray[j + 1].pucPat[m - 2])) < HASH16(&(ps->pstPatArray[j].pucPat[m - 2]))) //比较的为每个子串截取部分的最后两个字符的哈希值
            {
                flag = 1;
                temp = ps->pstPatArray[j + 1].pucPat;
                ps->pstPatArray[j + 1].pucPat = ps->pstPatArray[j].pucPat;
                ps->pstPatArray[j].pucPat = temp;
            }
        }
    }
}

/* ****************************************************************
 函数：static VOID wmPrepHashedPatternGroups(WM_STRUCT *)
 目的：计算共有多少个不同的哈希值，且从小到大
 参数：
 ps => 模式串集
 返回：
 ****************************************************************/
static VOID wmPrepHashedPatternGroups(WM_STRUCT *ps)
{
    USHORT usSindex, usHindex, usNingroup;
    INT i;
    INT m = ps->iSmallest;
    ps->uiNumHashEntries = HASHTABLESIZE; //HASH表的大小
    ps->pusHash = (HASH_TYPE*)malloc(sizeof(HASH_TYPE) * ps->uiNumHashEntries); //HASH表
    if (!ps->pusHash)
    {
        printf("No memory in wmPrepHashedPatternGroups()\n");
        return;
    }

    for (i = 0; i < (int)ps->uiNumHashEntries; i++) //HASH表预处理初始化，全部初始化为(HASH_TYPE)-1
    {
        ps->pusHash[i] = (HASH_TYPE)-1;
    }

    for (i = 0; i < ps->iNumPatterns; i++) //针对所有子串进行HASH预处理
    {
        usHindex = HASH16(&ps->pstPatArray[i].pucPat[m - 2]); //对模式子串的最后两个字符计算哈希值（匹配）
        usSindex = ps->pusHash[usHindex] = i;
        usNingroup = 1;
        //此时哈希表已经有序了
        while ((++i < ps->iNumPatterns) && (usHindex == HASH16(&ps->pstPatArray[i].pucPat[m - 2]))) //找后缀相同的子串数
            usNingroup++;
        ps->pusNumArray[usSindex] = usNingroup; //第i个子串，其后的子模式串与其后缀2字符相同子串的个数
        i--;
    }
}

/* ****************************************************************
 函数：static VOID wmPrepShiftTable(WM_STRUCT *)
 目的：建立shift表，算出每个字符块要移动的距离
 参数：
 ps => 模式串集
 返回：

 ****************************************************************/
static VOID wmPrepShiftTable(WM_STRUCT *ps)
{
    INT i;
    USHORT m, k, cindex;
    unsigned shift;
    m = (USHORT)ps->iSmallest;
    ps->pucShift = (UCHAR*)malloc(SHIFTTABLESIZE * sizeof(char));
    if (!ps->pucShift)
    {
        return;
    }

    for (i = 0; i < SHIFTTABLESIZE; i++) //初始化Shift表，初始值为最短字符串的长度
    {
        ps->pucShift[i] = (unsigned)(m - 2 + 1);
    }

    for (i = 0; i < ps->iNumPatterns; i++) //针对每个子串预处理
    {
        for (k = 0; k < m - 1; k++)
        {
            shift = (USHORT)(m - 2 - k);
            cindex = ((ps->pstPatArray[i].pucPat[k] << 8) | (ps->pstPatArray[i].pucPat[k + 1])); //B为2
            if (shift < ps->pucShift[cindex])
                ps->pucShift[cindex] = shift; //k=m-2时，shift=0，
        }
    }
}

/* ****************************************************************
 函数：static VOID wmPrepPrefixTable(WM_STRUCT *)
 目的：建立Prefix表
 参数：
 ps => 模式串集
 返回：
 无
 ****************************************************************/
static VOID wmPrepPrefixTable(WM_STRUCT *ps) //建立Prefix表
{
    INT i;
    ps->pusPrefix = (HASH_TYPE*)malloc(sizeof(HASH_TYPE) * ps->iNumPatterns); //分配空间长度为所有子串的个数*
    if (!ps->pusPrefix)
    {
        printf("No memory in wmPrepPrefixTable()\n");
        return;
    }

    for (i = 0; i < ps->iNumPatterns; i++) //哈希建立Prefix表
    {
        ps->pusPrefix[i] = HASH16(ps->pstPatArray[i].pucPat); //对每个模式串的前缀进行哈希
    }
}

/* ****************************************************************
 函数：VOID wmGroupMatch(WM_STRUCT *,INT ,UCHAR *,UCHAR *)
 目的：后缀哈希值相同，比较前缀以及整个字符串匹配
 参数：
 ps => 模式串集
 lindex =>
 Tx => 要进行匹配的字符串序列
 T => 模式子串
 返回：
 无
 ****************************************************************/
VOID wmGroupMatch(WM_STRUCT *ps, INT lindex, //lindex为后缀哈希值相同的那些模式子串中的一个模式子串的index
                  UCHAR *Tx, UCHAR *T)
{
    WM_PATTERN_STRUCT *patrn;
    WM_PATTERN_STRUCT *patrnEnd;
    INT text_prefix;
    UCHAR *px, *qx;

    patrn = &ps->pstPatArray[lindex];
    patrnEnd = patrn + ps->pusNumArray[lindex];

    text_prefix = HASH16(T);

    for (; patrn < patrnEnd; patrn++)
    {
        if (ps->pusPrefix[lindex++] != text_prefix)
            continue;
        else //如果后缀哈希值相同，则
        {
            px = patrn->pucPat; //取patrn的字串
            qx = T;
            while (*(px++) == *(qx++) && *(qx - 1) != '\0')
                ; //整个模式串进行比较
            if (*(px - 1) == '\0') //匹配到了结束位置，说明匹配成功
            {
                printf("Match pattern \"%s\" at line %d column %d\n", patrn->pucPat, nline, T - Tx + 1);
                nfound++;
            }
        }
    }
}

/* ****************************************************************
 函数：INT wmPrepPatterns(WM_STRUCT *ps)
 目的：对模式串集预处理，由plist得到msPatArray
 参数：
 ps => 模式串集
 返回：
 INT - 预处理成功0，失败-1
 ****************************************************************/
INT wmPrepPatterns(WM_STRUCT *ps)
{
    INT kk;
    WM_PATTERN_STRUCT *plist;

    ps->pstPatArray = (WM_PATTERN_STRUCT *)malloc(sizeof(WM_PATTERN_STRUCT) * ps->iNumPatterns);
    if (!ps->pstPatArray)
    {
        return -1;
    }

    ps->pusNumArray = (USHORT*)malloc(sizeof(short) * ps->iNumPatterns);
    if (!ps->pusNumArray)
    {
        return -1;
    }

    for (kk = 0, plist = ps->pstPatlist; plist != NULL && kk < ps->iNumPatterns; plist = plist->pstNext)
    {
        memcpy(&ps->pstPatArray[kk++], plist, sizeof(WM_PATTERN_STRUCT));
    }

    sort(ps); //哈希排序
    wmPrepHashedPatternGroups(ps); //哈希表
    wmPrepShiftTable(ps); //shift表
    wmPrepPrefixTable(ps); //Prefix表
    return 0;
}

/* ****************************************************************
 函数：VOID wmSearch(WM_STRUCT *ps,UCHAR *Tx,INT n)
 目的：字符串匹配查找
 参数：
 ps => 模式串集
 Tx => 被查找的字符串序列
 n => 被查找的字符串长度
 返回：
 无
 ****************************************************************/
VOID wmSearch(WM_STRUCT *ps, UCHAR *Tx, INT n)
{
    INT Tleft, lindex, tshift;
    UCHAR *T, *Tend, *window;
    Tleft = n;
    Tend = Tx + n;
    if (n < ps->iSmallest) /*被查找的字符串序列比最小模式子串还短，
     显然是不可能查找到的，直接退出*/
        return;

    for (T = Tx, window = Tx + ps->iSmallest - 1; window < Tend; T++, window++, Tleft--)
    {
        tshift = ps->pucShift[(*(window - 1) << 8) | *window];
        while (tshift) //当tshift!=0,无匹配
        {
            window += tshift;
            T += tshift;
            Tleft -= tshift;
            if (window > Tend)
                return;
            tshift = ps->pucShift[(*(window - 1) << 8) | *window];
        }
        //tshift=0，表明后缀哈希值已经相同
        if ((lindex = ps->pusHash[(*(window - 1) << 8) | *window]) == (HASH_TYPE)-1)
            continue;
        lindex = ps->pusHash[(*(window - 1) << 8) | *window];
        wmGroupMatch(ps, lindex, Tx, T); //后缀哈希值相同，比较前缀及整个模式串
    }
}

/******************************************************************************
 * FunctionName: main
 *       Author: Mr.sancho
 *         Date: 2014年6月10日
 *    Arguments: int, char**
 *  Description:
 *
 *       Return: int
 *****************************************************************************/
INT main(INT iArgs, CHAR **ppArgv)
{
    INT iLength, n;
    WM_STRUCT *pstWm;
    CHAR keyword[MAXM]; //单词
    CHAR str[MAXN]; //模式串

    pstWm = wmNew(); //创建模式串集
    printf("Scanf the number of pattern words ->\n"); //输入模式串集中模式子串的个数,n
    scanf("%d", &n);
    printf("Scanf the pattern words ->\n");
    while (n--)
    {
        scanf("%s", keyword); //输入每个模式子串
        iLength = strlen(keyword);
        wmAddPattern(pstWm, keyword, iLength); //新增模式子串
    }

    wmPrepPatterns(pstWm); //对模式串集预处理
    printf("Scanf the text string ->\n");
    scanf("%s", str); //输入要被匹配的字符串序列
    iLength = strlen(str);
    wmSearch(pstWm, str, iLength);
    wmFree(pstWm);
    getchar();

    return 0;
}
