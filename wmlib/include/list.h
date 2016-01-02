/******************************************************************************
 *    FileName: list.h
 *      Author: Mr.sancho
 *        Date: 2014年6月20日
 * Description: list and APIs difinition:
 *              single-track list and double-track list
 *****************************************************************************/
#ifndef _LIST_H_
#define _LIST_H_

#ifndef _BASETYPE_H_
#error "You need include <basetype.h> first."
#endif

__BEGIN_DECLS

/*
 * single-track list structs and the APIs, following by macros that could be used to travel the list.
 */
typedef struct tagSTListNode
{
    struct tagSTListNode *pstNext;
} STL_NODE_S;

typedef struct tagSTListHead
{
    STL_NODE_S *pstFirst;
    STL_NODE_S *pstLast;
} STL_HEAD_S;

typedef VOID (STL_FREENODE_PF)(VOID *);

STATIC __inline__ VOID STL_InitHead(STL_HEAD_S * __restrict pstHead);
STATIC __inline__ VOID STL_InitNode(STL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T STL_IsEmpty(const STL_HEAD_S * __restrict pstHead);
STATIC __inline__ BOOL_T STL_IsNodeOfList(const STL_HEAD_S * __restrict pstHead, const STL_NODE_S * __restrict pstNode);
STATIC __inline__ STL_NODE_S *STL_GetFirst(const STL_HEAD_S * __restrict pstHead);
STATIC __inline__ STL_NODE_S *STL_GetLast(const STL_HEAD_S * __restrict pstHead);
STATIC __inline__ STL_NODE_S *STL_GetPrev(const STL_HEAD_S * __restrict pstHead, const STL_NODE_S * __restrict pstNode);
STATIC __inline__ STL_NODE_S *STL_GetNext(const STL_HEAD_S * __restrict pstHead, const STL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T STL_AddHead(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T STL_AddTail(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T STL_AddBefore(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstBeforeNode,
                                       STL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T STL_AddAfter(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstAfterNode,
                                      STL_NODE_S * __restrict pstNode);
STATIC __inline__ VOID STL_Delete(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstNode);
STATIC __inline__ VOID STL_FreeAll(STL_HEAD_S * __restrict pstHead, STL_FREENODE_PF *pfFreeCallBack);

/* traval the single-track list and visit every node by pstEntry */
#define STL_FOREACH_NODE(pstHead, pstEntry) \
        for (pstEntry = STL_GetFirst(pstHead); NULL != pstEntry; \
             (pstEntry == STL_GetLast(pstHead)) ? NULL : (pstEntry = pstEntry->pstNext))

/******************************************************************************
 *    Function: STL_InitHead
 *      Author: Mr.sancho
 *        Date: 2014年6月20日
 * Description: init the single-track list head
 *   Arguments: STL_HEAD_S *pstHead: the list head to be initialized
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID STL_InitHead(STL_HEAD_S * __restrict pstHead)
{
    if (NULL != pstHead)
    {
        pstHead->pstFirst = NULL;
        pstHead->pstLast = NULL;
    }

    return;
}

/******************************************************************************
 *    Function: STL_InitNode
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: init the single-track list node
 *   Arguments: STL_NODE_S *pstNode: the list node to be initialized
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID STL_InitNode(STL_NODE_S * __restrict pstNode)
{
    if (NULL != pstNode)
    {
        pstNode->pstNext = NULL;
    }

    return;
}

/******************************************************************************
 *    Function: STL_IsEmpty
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: check if the single-track list is empty
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *      Return: BOOL_T: BOOL_TRUE means the list is empty
 *****************************************************************************/
STATIC __inline__ BOOL_T STL_IsEmpty(const STL_HEAD_S * __restrict pstHead)
{
    if (NULL == pstHead)
    {
        return BOOL_TRUE;
    }

    return ((NULL == pstHead->pstFirst) ? BOOL_TRUE : BOOL_FALSE);
}

/******************************************************************************
 *    Function: STL_IsNodeOfList
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: check if pstNode is a node of the single-track list
 *   Arguments: const STL_HEAD_S *pstHead: the list head
 *              const STL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means pstNode is a node of the list
 *****************************************************************************/
STATIC __inline__ BOOL_T STL_IsNodeOfList(const STL_HEAD_S * __restrict pstHead, const STL_NODE_S * __restrict pstNode)
{
    STL_NODE_S *pstEntry;
    BOOL_T bResult;

    if ((NULL == pstHead) || (NULL == pstNode))
    {
        return BOOL_FALSE;
    }

    bResult = BOOL_FALSE;
    STL_FOREACH_NODE(pstHead, pstEntry)
    {
        if (pstEntry == pstNode)
        {
            bResult = BOOL_TRUE;
            break;
        }
    }

    return bResult;
}

/******************************************************************************
 *    Function: STL_GetFirst
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: get the first node of the single-track list
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *      Return: STL_NODE_S *
 *****************************************************************************/
STATIC __inline__ STL_NODE_S *STL_GetFirst(const STL_HEAD_S * __restrict pstHead)
{
    return ((BOOL_TRUE == STL_IsEmpty(pstHead)) ? NULL : pstHead->pstFirst);
}

/******************************************************************************
 *    Function: STL_GetLast
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: get the last node of the single-track list
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *      Return: STL_NODE_S *
 *****************************************************************************/
STATIC __inline__ STL_NODE_S *STL_GetLast(const STL_HEAD_S * __restrict pstHead)
{
    return ((BOOL_TRUE == STL_IsEmpty(pstHead)) ? NULL : pstHead->pstLast);
}

/******************************************************************************
 *    Function: STL_GetPrev
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: get the previous node of pstNode from the single-track list
 *   Arguments: const STL_HEAD_S * __restrict pstHead: the list head
 *              const STL_NODE_S * __restrict pstNode: the list node pointer according to
 *      Return: STL_NODE_S *
 *****************************************************************************/
STATIC __inline__ STL_NODE_S *STL_GetPrev(const STL_HEAD_S * __restrict pstHead, const STL_NODE_S * __restrict pstNode)
{
    STL_NODE_S *pstEntry;

    if ((NULL == pstHead) || (NULL == pstNode) || (BOOL_TRUE == STL_IsEmpty(pstHead)))
    {
        return NULL;
    }

    if (pstNode == pstHead->pstFirst)
    {
        return NULL;
    }

    STL_FOREACH_NODE(pstHead, pstEntry)
    {
        if (pstNode == pstEntry->pstNext)
        {
            break;
        }
    }

    return pstEntry;
}

/******************************************************************************
 *    Function: STL_GetPrev
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: get the previous node of pstNode from the single-track list
 *   Arguments: const STL_HEAD_S * __restrict pstHead: the list head
 *              const STL_NODE_S * __restrict pstNode: the list node pointer according to
 *      Return: STL_NODE_S *
 *****************************************************************************/
STATIC __inline__ STL_NODE_S *STL_GetNext(const STL_HEAD_S * __restrict pstHead, const STL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode) || (BOOL_TRUE == STL_IsEmpty(pstHead)))
    {
        return NULL;
    }

    if (pstNode == pstHead->pstLast)
    {
        return NULL;
    }

    return pstNode->pstNext;
}

/******************************************************************************
 *    Function: STL_AddHead
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: add the single-track list node pstNode to the head of list pointed by pstHead
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *              STL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T STL_AddHead(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode))
    {
        return BOOL_FALSE;
    }

    if (BOOL_TRUE == STL_IsEmpty(pstHead))
    {
        pstHead->pstLast = pstNode;
    }
    else
    {
        pstHead->pstLast->pstNext = pstNode;
    }

    pstNode->pstNext = pstHead->pstFirst;
    pstHead->pstFirst = pstNode;

    return BOOL_TRUE;
}

/******************************************************************************
 *    Function: STL_AddTail
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: add the single-track list node pstNode to the tail of list pointed by pstHead
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *              STL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T STL_AddTail(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode))
    {
        return BOOL_FALSE;
    }

    if (BOOL_TRUE == STL_IsEmpty(pstHead))
    {
        pstHead->pstFirst = pstNode;
    }
    else
    {
        pstHead->pstLast->pstNext = pstNode;
    }

    pstHead->pstLast = pstNode;
    pstNode->pstNext = pstHead->pstFirst;

    return BOOL_TRUE;
}

/******************************************************************************
 *    Function: STL_AddBefore
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: add the single-track list node pstNode before pstBeforeNode to the list
 *              pointed by pstHead
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *              STL_NODE_S *pstBeforeNode: the list node pointer according to
 *              STL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T STL_AddBefore(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstBeforeNode,
                                       STL_NODE_S * __restrict pstNode)
{
    STL_NODE_S *pstEntry;
    BOOL_T bResult;

    if ((NULL == pstHead) || (NULL == pstBeforeNode) || (NULL == pstNode) || (BOOL_TRUE == STL_IsEmpty(pstHead)))
    {
        return BOOL_FALSE;
    }

    if (pstBeforeNode == pstHead->pstFirst)
    {
        return STL_AddHead(pstHead, pstNode);
    }

    bResult = BOOL_FALSE;
    STL_FOREACH_NODE(pstHead, pstEntry)
    {
        if (pstBeforeNode == pstEntry->pstNext)
        {
            pstEntry->pstNext = pstNode;
            pstNode->pstNext = pstBeforeNode;

            bResult = BOOL_TRUE;
            break;
        }
    }

    return bResult;
}

/******************************************************************************
 *    Function: STL_AddAfter
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: add the single-track list node pstNode after pstAfterNode to the list
 *              pointed by pstHead
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *              STL_NODE_S *pstAfterNode: the list node pointer according to
 *              STL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T STL_AddAfter(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstAfterNode,
                                      STL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstAfterNode) || (NULL == pstNode) || (BOOL_TRUE == STL_IsEmpty(pstHead)))
    {
        return BOOL_FALSE;
    }

    pstNode->pstNext = pstAfterNode->pstNext;
    pstAfterNode->pstNext = pstNode;

    if (pstAfterNode == pstHead->pstLast)
    {
        pstHead->pstLast = pstNode;
    }

    return BOOL_TRUE;
}

/******************************************************************************
 *    Function: STL_Delete
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: delete the single-track list node pstNode from the list pointed by pstHead
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *              STL_NODE_S *pstNode: the list node pointer
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID STL_Delete(STL_HEAD_S * __restrict pstHead, STL_NODE_S * __restrict pstNode)
{
    STL_NODE_S *pstEntry;

    if ((NULL == pstHead) || (NULL == pstNode) || (BOOL_TRUE == STL_IsEmpty(pstHead)))
    {
        return;
    }

    if (pstHead->pstFirst == pstNode)
    {
        if (pstHead->pstLast == pstNode)
        {
            pstHead->pstFirst = NULL;
            pstHead->pstLast = NULL;
        }
        else
        {
            pstHead->pstFirst = pstNode->pstNext;
            pstHead->pstLast->pstNext = pstHead->pstFirst;
        }
    }
    else
    {
        STL_FOREACH_NODE(pstHead, pstEntry)
        {
            if (pstEntry->pstNext == pstNode)
            {
                pstEntry->pstNext = pstNode->pstNext;

                if (pstHead->pstLast == pstNode)
                {
                    pstHead->pstLast = pstEntry;
                }

                break;
            }
        }
    }

    pstNode->pstNext = NULL;

    return;
}

/******************************************************************************
 *    Function: STL_FreeAll
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: remove all nodes from the single-track list and free them by
 *              pfFreeCallBack(only if it is not null)
 *   Arguments: STL_HEAD_S *pstHead: the list head
 *              STL_FREENODE_PF *pfFreeCallBack: the call back function to free list node
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID STL_FreeAll(STL_HEAD_S * __restrict pstHead, STL_FREENODE_PF pfFreeCallBack)
{
    STL_NODE_S *pstEntry;

    while (BOOL_TRUE != STL_IsEmpty(pstHead))
    {
        pstEntry = pstHead->pstFirst;
        (VOID)STL_Delete(pstHead, pstEntry);

        if (NULL != pfFreeCallBack)
        {
            pfFreeCallBack((VOID *)pstEntry);
        }
    }

    return;
}

/********************************************** split line ************************************************/

/*
 * double-track list structs and the APIs, following by macros that could be used to travel the list.
 */
typedef struct tagDTListNode
{
    struct tagDTListNode *pstPrev;
    struct tagDTListNode *pstNext;
} DTL_NODE_S;

typedef struct tagDTListHead
{
    DTL_NODE_S *pstFirst;
    DTL_NODE_S *pstLast;
} DTL_HEAD_S;

typedef VOID (DTL_FREENODE_PF)(VOID *);

STATIC __inline__ VOID DTL_InitHead(DTL_HEAD_S * __restrict pstHead);
STATIC __inline__ VOID DTL_InitNode(DTL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T DTL_IsEmpty(const DTL_HEAD_S * __restrict pstHead);
STATIC __inline__ BOOL_T DTL_IsNodeOfList(const DTL_HEAD_S * __restrict pstHead, const DTL_NODE_S * __restrict pstNode);
STATIC __inline__ DTL_NODE_S *DTL_GetFirst(const DTL_HEAD_S * __restrict pstHead);
STATIC __inline__ DTL_NODE_S *DTL_GetLast(const DTL_HEAD_S * __restrict pstHead);
STATIC __inline__ DTL_NODE_S *DTL_GetPrev(const DTL_HEAD_S * __restrict pstHead, const DTL_NODE_S * __restrict pstNode);
STATIC __inline__ DTL_NODE_S *DTL_GetNext(const DTL_HEAD_S * __restrict pstHead, const DTL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T DTL_AddHead(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T DTL_AddTail(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T DTL_AddBefore(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstBeforeNode,
                                       DTL_NODE_S * __restrict pstNode);
STATIC __inline__ BOOL_T DTL_AddAfter(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstAfterNode,
                                      DTL_NODE_S * __restrict pstNode);
STATIC __inline__ VOID DTL_Delete(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstNode);
STATIC __inline__ VOID DTL_FreeAll(DTL_HEAD_S * __restrict pstHead, DTL_FREENODE_PF *pfFreeCallBack);

/* traval the double-track list and visit every node by pstEntry */
#define DTL_FOREACH_NODE(pstHead, pstEntry) \
        for (pstEntry = DTL_GetFirst(pstHead); NULL != pstEntry; \
             (pstEntry == DTL_GetLast(pstHead)) ? NULL : (pstEntry = pstEntry->pstNext))

/******************************************************************************
 *    Function: DTL_InitHead
 *      Author: Mr.sancho
 *        Date: 2014年6月20日
 * Description: init the double-track list head
 *   Arguments: DTL_HEAD_S *pstHead: the list head to be initialized
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID DTL_InitHead(DTL_HEAD_S * __restrict pstHead)
{
    if (NULL != pstHead)
    {
        pstHead->pstFirst = NULL;
        pstHead->pstLast = NULL;
    }

    return;
}

/******************************************************************************
 *    Function: DTL_InitNode
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: init the double-track list node
 *   Arguments: DTL_NODE_S *pstNode: the list node to be initialized
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID DTL_InitNode(DTL_NODE_S * __restrict pstNode)
{
    if (NULL != pstNode)
    {
        pstNode->pstPrev = NULL;
        pstNode->pstNext = NULL;
    }

    return;
}

/******************************************************************************
 *    Function: DTL_IsEmpty
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: check if the double-track list is empty
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *      Return: BOOL_T: BOOL_TRUE means the list is empty
 *****************************************************************************/
STATIC __inline__ BOOL_T DTL_IsEmpty(const DTL_HEAD_S * __restrict pstHead)
{
    if (NULL == pstHead)
    {
        return BOOL_TRUE;
    }

    return ((NULL == pstHead->pstFirst) ? BOOL_TRUE : BOOL_FALSE);
}

/******************************************************************************
 *    Function: DTL_IsNodeOfList
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: check if pstNode is a node of the double-track list
 *   Arguments: const DTL_HEAD_S *pstHead: the list head
 *              const DTL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means pstNode is a node of the list
 *****************************************************************************/
STATIC __inline__ BOOL_T DTL_IsNodeOfList(const DTL_HEAD_S * __restrict pstHead, const DTL_NODE_S * __restrict pstNode)
{
    DTL_NODE_S *pstEntry;
    BOOL_T bResult;

    if ((NULL == pstHead) || (NULL == pstNode))
    {
        return BOOL_FALSE;
    }

    bResult = BOOL_FALSE;
    DTL_FOREACH_NODE(pstHead, pstEntry)
    {
        if (pstEntry == pstNode)
        {
            bResult = BOOL_TRUE;
            break;
        }
    }

    return bResult;
}

/******************************************************************************
 *    Function: DTL_GetFirst
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: get the first node of the double-track list
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *      Return: DTL_NODE_S *
 *****************************************************************************/
STATIC __inline__ DTL_NODE_S *DTL_GetFirst(const DTL_HEAD_S * __restrict pstHead)
{
    return ((BOOL_TRUE == DTL_IsEmpty(pstHead)) ? NULL : pstHead->pstFirst);
}

/******************************************************************************
 *    Function: DTL_GetLast
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: get the last node of the double-track list
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *      Return: DTL_NODE_S *
 *****************************************************************************/
STATIC __inline__ DTL_NODE_S *DTL_GetLast(const DTL_HEAD_S * __restrict pstHead)
{
    return ((BOOL_TRUE == DTL_IsEmpty(pstHead)) ? NULL : pstHead->pstLast);
}

/******************************************************************************
 *    Function: DTL_GetPrev
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: get the previous node of pstNode from the double-track list
 *   Arguments: const DTL_HEAD_S * __restrict pstHead: the list head
 *              const DTL_NODE_S * __restrict pstNode: the list node pointer according to
 *      Return: DTL_NODE_S *
 *****************************************************************************/
STATIC __inline__ DTL_NODE_S *DTL_GetPrev(const DTL_HEAD_S * __restrict pstHead, const DTL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode) || (BOOL_TRUE == DTL_IsEmpty(pstHead)))
    {
        return NULL;
    }

    return ((pstNode == pstHead->pstFirst) ? NULL : pstNode->pstPrev);
}

/******************************************************************************
 *    Function: DTL_GetPrev
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: get the previous node of pstNode from the double-track list
 *   Arguments: const DTL_HEAD_S * __restrict pstHead: the list head
 *              const DTL_NODE_S * __restrict pstNode: the list node pointer according to
 *      Return: DTL_NODE_S *
 *****************************************************************************/
STATIC __inline__ DTL_NODE_S *DTL_GetNext(const DTL_HEAD_S * __restrict pstHead, const DTL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode) || (BOOL_TRUE == DTL_IsEmpty(pstHead)))
    {
        return NULL;
    }

    return ((pstNode == pstHead->pstLast) ? NULL : pstNode->pstNext);
}

/******************************************************************************
 *    Function: DTL_AddHead
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: add the double-track list node pstNode to the head of list pointed by pstHead
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *              DTL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T DTL_AddHead(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode))
    {
        return BOOL_FALSE;
    }

    if (BOOL_TRUE == DTL_IsEmpty(pstHead))
    {
        pstHead->pstFirst = pstNode;
        pstHead->pstLast = pstNode;

        pstNode->pstPrev = pstNode;
        pstNode->pstNext = pstNode;
    }
    else
    {
        pstNode->pstNext = pstHead->pstFirst;
        pstNode->pstPrev = pstHead->pstLast;

        pstHead->pstLast->pstNext = pstNode;
        pstHead->pstFirst->pstPrev = pstNode;

        pstHead->pstFirst = pstNode;
    }

    return BOOL_TRUE;
}

/******************************************************************************
 *    Function: DTL_AddTail
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: add the double-track list node pstNode to the tail of list pointed by pstHead
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *              DTL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T DTL_AddTail(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode))
    {
        return BOOL_FALSE;
    }

    if (BOOL_TRUE == DTL_IsEmpty(pstHead))
    {
        pstHead->pstFirst = pstNode;
        pstHead->pstLast = pstNode;

        pstNode->pstPrev = pstNode;
        pstNode->pstNext = pstNode;
    }
    else
    {
        pstNode->pstNext = pstHead->pstFirst;
        pstNode->pstPrev = pstHead->pstLast;

        pstHead->pstLast->pstNext = pstNode;
        pstHead->pstFirst->pstPrev = pstNode;

        pstHead->pstLast = pstNode;
    }

    return BOOL_TRUE;
}

/******************************************************************************
 *    Function: DTL_AddBefore
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: add the double-track list node pstNode before pstBeforeNode to the list
 *              pointed by pstHead
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *              DTL_NODE_S *pstBeforeNode: the list node pointer according to
 *              DTL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T DTL_AddBefore(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstBeforeNode,
                                       DTL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstBeforeNode) || (NULL == pstNode) || (BOOL_TRUE == DTL_IsEmpty(pstHead)))
    {
        return BOOL_FALSE;
    }

    if (pstBeforeNode == pstHead->pstFirst)
    {
        return DTL_AddHead(pstHead, pstNode);
    }

    pstNode->pstNext = pstBeforeNode;
    pstNode->pstPrev = pstBeforeNode->pstPrev;

    pstBeforeNode->pstPrev->pstNext = pstNode;
    pstBeforeNode->pstPrev = pstNode;

    return BOOL_TRUE;
}

/******************************************************************************
 *    Function: DTL_AddAfter
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: add the double-track list node pstNode after pstAfterNode to the list
 *              pointed by pstHead
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *              DTL_NODE_S *pstAfterNode: the list node pointer according to
 *              DTL_NODE_S *pstNode: the list node pointer
 *      Return: BOOL_T: BOOL_TRUE means add pstNode succeed to list
 *****************************************************************************/
STATIC __inline__ BOOL_T DTL_AddAfter(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstAfterNode,
                                      DTL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstAfterNode) || (NULL == pstNode) || (BOOL_TRUE == DTL_IsEmpty(pstHead)))
    {
        return BOOL_FALSE;
    }

    if (pstAfterNode == pstHead->pstLast)
    {
        return DTL_AddTail(pstHead, pstNode);
    }

    pstNode->pstNext = pstAfterNode->pstNext;
    pstNode->pstPrev = pstAfterNode;

    pstAfterNode->pstNext->pstPrev = pstNode;
    pstAfterNode->pstNext = pstNode;

    return BOOL_TRUE;
}

/******************************************************************************
 *    Function: DTL_Delete
 *      Author: Mr.sancho
 *        Date: 2014年6月21日
 * Description: delete the double-track list node pstNode from the list pointed by pstHead
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *              DTL_NODE_S *pstNode: the list node pointer
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID DTL_Delete(DTL_HEAD_S * __restrict pstHead, DTL_NODE_S * __restrict pstNode)
{
    if ((NULL == pstHead) || (NULL == pstNode) || (BOOL_TRUE == DTL_IsEmpty(pstHead)))
    {
        return;
    }

    if (pstNode == pstNode->pstPrev)
    {
        if (pstNode == pstHead->pstFirst)
        {
            pstHead->pstFirst = NULL;
            pstHead->pstLast = NULL;
        }
    }
    else
    {
        pstNode->pstNext->pstPrev = pstNode->pstPrev;
        pstNode->pstPrev->pstNext = pstNode->pstNext;

        if (pstNode == pstHead->pstFirst)
        {
            pstHead->pstFirst = pstNode->pstNext;
        }

        if (pstNode == pstHead->pstLast)
        {
            pstHead->pstLast = pstNode->pstPrev;
        }
    }

    pstNode->pstNext = NULL;
    pstNode->pstPrev = NULL;

    return;
}

/******************************************************************************
 *    Function: DTL_FreeAll
 *      Author: Mr.sancho
 *        Date: 2014年6月22日
 * Description: remove all nodes from the double-track list and free them by
 *              pfFreeCallBack(only if it is not null)
 *   Arguments: DTL_HEAD_S *pstHead: the list head
 *              DTL_FREENODE_PF *pfFreeCallBack: the call back function to free list node
 *      Return: VOID
 *****************************************************************************/
STATIC __inline__ VOID DTL_FreeAll(DTL_HEAD_S * __restrict pstHead, DTL_FREENODE_PF *pfFreeCallBack)
{
    DTL_NODE_S *pstEntry;

    while (BOOL_TRUE != DTL_IsEmpty(pstHead))
    {
        pstEntry = pstHead->pstFirst;
        (VOID)DTL_Delete(pstHead, pstEntry);

        if (NULL != pfFreeCallBack)
        {
            pfFreeCallBack((VOID *)pstEntry);
        }
    }

    return;
}

__END_DECLS
#endif
