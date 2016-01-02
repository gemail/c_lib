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

//ģʽ������󳤶�MAXN - 1
#define MAXN 10001

//������󳤶�ΪMAXM - 1
#define MAXM 51

/******************************************************************************
 *    Function: wmNew
 *      Author: Mr.sancho
 *        Date: 2014��6��15��
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

    pstWm->iNumPatterns = 0; //ģʽ���ĸ���,��ʼΪ0
    pstWm->iSmallest = 1000; //���ģʽ���ĳ���
    return pstWm;
}

/******************************************************************************
 * FunctionName: wmFree
 *       Author: Mr.sancho
 *         Date: 2014��6��15��
 *  Description: �ͷ�ģʽ����ռ�ÿռ�
 *    Arguments: WM_STRUCT *ps,ģʽ����
 *
 *       Return: void
 *****************************************************************************/
VOID wmFree(WM_STRUCT *pstWm) //�ͷſռ亯��
{
    if (pstWm->pstPatArray) //���ģʽ�����д����Ӵ��������ͷ��Ӵ�����ռ�ÿռ�
    {
        if (pstWm->pstPatArray->pucPat)
            free(pstWm->pstPatArray->pucPat); //�Ӵ���Ϊ�գ����ͷ�
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
 *         Date: 2014��6��15��
 *    Arguments: WM_STRUCT *pstWm, => ģʽ����
 *    		 UCHAR *pcNewStr, => Ҫ�������Ӵ�
 *    		 INT iLength, �Ӵ�����
 *  Description: ��ģʽ����pstWm������һ������Ϊm���Ӵ�q
 *
 *       Return: int
 *****************************************************************************/
STATIC INT wmAddPattern(WM_STRUCT *pstWm, UCHAR *pcNewStr, INT iLength)
{
    WM_PATTERN_STRUCT *pstPattenWm; //����һ���Ӵ��ṹ
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

    pstPattenWm->pucPat = pucPatten; //���Ӵ�����ĳ��ȷ���ռ�
    pstPattenWm->uiLen = iLength; //�Ӵ����ȸ�ֵ

    pstWm->pstPatlist = pstPattenWm;
    pstPattenWm->pstNext = pstWm->pstPatlist; //�������Ӵ������ַ������б��С�������ʽ�������ڶ���ͷ��
    pstWm->iNumPatterns++; //ģʽ�������Ӵ�������1

    if (pstPattenWm->uiLen < (UINT)pstWm->iSmallest)
    {
        pstWm->iSmallest = pstPattenWm->uiLen; //����ȷ������ַ�������
    }

    return 0;
}

/* ****************************************************************
 ������static unsigned HASH16(UCHAR *)
 Ŀ�ģ���һ���ַ����й�ϣ���㡣���㷽ʽΪ��(((*T)<<8) | *(T+1))��
 ������
 T => Ҫ��ϣ������ַ���
 ���أ�
 unsigned - ��̬���������ض��ַ���T����Ĺ�ϣֵ
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
    return (USHORT)(((*T) << 8) | *(T + 1)); //�Ե�һ���ַ�����8λ��Ȼ����ڶ����ַ��������
}

/* ****************************************************************
 ������sort(WM_STRUCT *)
 Ŀ�ģ����ַ�����ps�е��Ӵ����У������Ӵ���ֵ�Ĺ�ϣֵ��С��������
 ������
 ps => ģʽ����
 ���أ���
 ****************************************************************/
VOID sort(WM_STRUCT *ps)
{
    INT m = ps->iSmallest; //��ȡ����Ӵ�����
    INT i, j;
    UCHAR *temp;
    INT flag; //ð������ı�־λ����һ�˱Ƚ��޽���ʱ��˵���Ѿ�������򣬼���������ѭ������
    for (i = ps->iNumPatterns - 1, flag = 1; i > 0 && flag; i--) //ѭ�����ַ������е�ÿ���Ӵ����������ϣֵ��С����ð������
    {
        flag = 0;
        for (j = 0; j < i; j++)
        {
            if (HASH16(&(ps->pstPatArray[j + 1].pucPat[m - 2])) < HASH16(&(ps->pstPatArray[j].pucPat[m - 2]))) //�Ƚϵ�Ϊÿ���Ӵ���ȡ���ֵ���������ַ��Ĺ�ϣֵ
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
 ������static VOID wmPrepHashedPatternGroups(WM_STRUCT *)
 Ŀ�ģ����㹲�ж��ٸ���ͬ�Ĺ�ϣֵ���Ҵ�С����
 ������
 ps => ģʽ����
 ���أ�
 ****************************************************************/
static VOID wmPrepHashedPatternGroups(WM_STRUCT *ps)
{
    USHORT usSindex, usHindex, usNingroup;
    INT i;
    INT m = ps->iSmallest;
    ps->uiNumHashEntries = HASHTABLESIZE; //HASH��Ĵ�С
    ps->pusHash = (HASH_TYPE*)malloc(sizeof(HASH_TYPE) * ps->uiNumHashEntries); //HASH��
    if (!ps->pusHash)
    {
        printf("No memory in wmPrepHashedPatternGroups()\n");
        return;
    }

    for (i = 0; i < (int)ps->uiNumHashEntries; i++) //HASH��Ԥ�����ʼ����ȫ����ʼ��Ϊ(HASH_TYPE)-1
    {
        ps->pusHash[i] = (HASH_TYPE)-1;
    }

    for (i = 0; i < ps->iNumPatterns; i++) //��������Ӵ�����HASHԤ����
    {
        usHindex = HASH16(&ps->pstPatArray[i].pucPat[m - 2]); //��ģʽ�Ӵ�����������ַ������ϣֵ��ƥ�䣩
        usSindex = ps->pusHash[usHindex] = i;
        usNingroup = 1;
        //��ʱ��ϣ���Ѿ�������
        while ((++i < ps->iNumPatterns) && (usHindex == HASH16(&ps->pstPatArray[i].pucPat[m - 2]))) //�Һ�׺��ͬ���Ӵ���
            usNingroup++;
        ps->pusNumArray[usSindex] = usNingroup; //��i���Ӵ���������ģʽ�������׺2�ַ���ͬ�Ӵ��ĸ���
        i--;
    }
}

/* ****************************************************************
 ������static VOID wmPrepShiftTable(WM_STRUCT *)
 Ŀ�ģ�����shift�����ÿ���ַ���Ҫ�ƶ��ľ���
 ������
 ps => ģʽ����
 ���أ�

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

    for (i = 0; i < SHIFTTABLESIZE; i++) //��ʼ��Shift����ʼֵΪ����ַ����ĳ���
    {
        ps->pucShift[i] = (unsigned)(m - 2 + 1);
    }

    for (i = 0; i < ps->iNumPatterns; i++) //���ÿ���Ӵ�Ԥ����
    {
        for (k = 0; k < m - 1; k++)
        {
            shift = (USHORT)(m - 2 - k);
            cindex = ((ps->pstPatArray[i].pucPat[k] << 8) | (ps->pstPatArray[i].pucPat[k + 1])); //BΪ2
            if (shift < ps->pucShift[cindex])
                ps->pucShift[cindex] = shift; //k=m-2ʱ��shift=0��
        }
    }
}

/* ****************************************************************
 ������static VOID wmPrepPrefixTable(WM_STRUCT *)
 Ŀ�ģ�����Prefix��
 ������
 ps => ģʽ����
 ���أ�
 ��
 ****************************************************************/
static VOID wmPrepPrefixTable(WM_STRUCT *ps) //����Prefix��
{
    INT i;
    ps->pusPrefix = (HASH_TYPE*)malloc(sizeof(HASH_TYPE) * ps->iNumPatterns); //����ռ䳤��Ϊ�����Ӵ��ĸ���*
    if (!ps->pusPrefix)
    {
        printf("No memory in wmPrepPrefixTable()\n");
        return;
    }

    for (i = 0; i < ps->iNumPatterns; i++) //��ϣ����Prefix��
    {
        ps->pusPrefix[i] = HASH16(ps->pstPatArray[i].pucPat); //��ÿ��ģʽ����ǰ׺���й�ϣ
    }
}

/* ****************************************************************
 ������VOID wmGroupMatch(WM_STRUCT *,INT ,UCHAR *,UCHAR *)
 Ŀ�ģ���׺��ϣֵ��ͬ���Ƚ�ǰ׺�Լ������ַ���ƥ��
 ������
 ps => ģʽ����
 lindex =>
 Tx => Ҫ����ƥ����ַ�������
 T => ģʽ�Ӵ�
 ���أ�
 ��
 ****************************************************************/
VOID wmGroupMatch(WM_STRUCT *ps, INT lindex, //lindexΪ��׺��ϣֵ��ͬ����Щģʽ�Ӵ��е�һ��ģʽ�Ӵ���index
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
        else //�����׺��ϣֵ��ͬ����
        {
            px = patrn->pucPat; //ȡpatrn���ִ�
            qx = T;
            while (*(px++) == *(qx++) && *(qx - 1) != '\0')
                ; //����ģʽ�����бȽ�
            if (*(px - 1) == '\0') //ƥ�䵽�˽���λ�ã�˵��ƥ��ɹ�
            {
                printf("Match pattern \"%s\" at line %d column %d\n", patrn->pucPat, nline, T - Tx + 1);
                nfound++;
            }
        }
    }
}

/* ****************************************************************
 ������INT wmPrepPatterns(WM_STRUCT *ps)
 Ŀ�ģ���ģʽ����Ԥ������plist�õ�msPatArray
 ������
 ps => ģʽ����
 ���أ�
 INT - Ԥ����ɹ�0��ʧ��-1
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

    sort(ps); //��ϣ����
    wmPrepHashedPatternGroups(ps); //��ϣ��
    wmPrepShiftTable(ps); //shift��
    wmPrepPrefixTable(ps); //Prefix��
    return 0;
}

/* ****************************************************************
 ������VOID wmSearch(WM_STRUCT *ps,UCHAR *Tx,INT n)
 Ŀ�ģ��ַ���ƥ�����
 ������
 ps => ģʽ����
 Tx => �����ҵ��ַ�������
 n => �����ҵ��ַ�������
 ���أ�
 ��
 ****************************************************************/
VOID wmSearch(WM_STRUCT *ps, UCHAR *Tx, INT n)
{
    INT Tleft, lindex, tshift;
    UCHAR *T, *Tend, *window;
    Tleft = n;
    Tend = Tx + n;
    if (n < ps->iSmallest) /*�����ҵ��ַ������б���Сģʽ�Ӵ����̣�
     ��Ȼ�ǲ����ܲ��ҵ��ģ�ֱ���˳�*/
        return;

    for (T = Tx, window = Tx + ps->iSmallest - 1; window < Tend; T++, window++, Tleft--)
    {
        tshift = ps->pucShift[(*(window - 1) << 8) | *window];
        while (tshift) //��tshift!=0,��ƥ��
        {
            window += tshift;
            T += tshift;
            Tleft -= tshift;
            if (window > Tend)
                return;
            tshift = ps->pucShift[(*(window - 1) << 8) | *window];
        }
        //tshift=0��������׺��ϣֵ�Ѿ���ͬ
        if ((lindex = ps->pusHash[(*(window - 1) << 8) | *window]) == (HASH_TYPE)-1)
            continue;
        lindex = ps->pusHash[(*(window - 1) << 8) | *window];
        wmGroupMatch(ps, lindex, Tx, T); //��׺��ϣֵ��ͬ���Ƚ�ǰ׺������ģʽ��
    }
}

/******************************************************************************
 * FunctionName: main
 *       Author: Mr.sancho
 *         Date: 2014��6��10��
 *    Arguments: int, char**
 *  Description:
 *
 *       Return: int
 *****************************************************************************/
INT main(INT iArgs, CHAR **ppArgv)
{
    INT iLength, n;
    WM_STRUCT *pstWm;
    CHAR keyword[MAXM]; //����
    CHAR str[MAXN]; //ģʽ��

    pstWm = wmNew(); //����ģʽ����
    printf("Scanf the number of pattern words ->\n"); //����ģʽ������ģʽ�Ӵ��ĸ���,n
    scanf("%d", &n);
    printf("Scanf the pattern words ->\n");
    while (n--)
    {
        scanf("%s", keyword); //����ÿ��ģʽ�Ӵ�
        iLength = strlen(keyword);
        wmAddPattern(pstWm, keyword, iLength); //����ģʽ�Ӵ�
    }

    wmPrepPatterns(pstWm); //��ģʽ����Ԥ����
    printf("Scanf the text string ->\n");
    scanf("%s", str); //����Ҫ��ƥ����ַ�������
    iLength = strlen(str);
    wmSearch(pstWm, str, iLength);
    wmFree(pstWm);
    getchar();

    return 0;
}
