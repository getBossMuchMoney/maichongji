/*=============================================================================*
 *         Copyright(c) 2023-2043, LyRectifier Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  PRODUCT  : Pulse150_450A_4out
 *
 *  FILENAME : list.h
 *  PURPOSE  :
 *  AUTHOR   : liusj
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *  2023Äê12ÔÂ28ÈÕ       V1.0                          Created.
 *
 *============================================================================*/
#ifndef MODULE_SHELLMODULE_LIST_H_
#define MODULE_SHELLMODULE_LIST_H_

#ifdef list_GLOBALS
#define list_EXT
#else
#define list_EXT extern
#endif
/********************************************************************************
 *include header file                              *
 ********************************************************************************/
#include <stddef.h>

/********************************************************************************
 *const define                               *
 ********************************************************************************/
#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({            \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

/********************************************************************************
 * Variable declaration                              *
 ********************************************************************************/
struct list_head
{
    struct list_head *next, *prev;
};

/********************************************************************************
 * function declaration                              *
 ********************************************************************************/

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list)
{
    list->next = list;
    list->prev = list;
}

static inline void __list_add(struct list_head *new,
        struct list_head *prev,
        struct list_head *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void __list_del_entry(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = NULL;
    entry->prev = NULL;
}

static inline void list_del_init(struct list_head *entry)
{
    __list_del_entry(entry);
    INIT_LIST_HEAD(entry);
}

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each_safe(pos,n,head)\
    for (pos=(head)->next,n=pos->next;pos!=head;\
        pos=n,n=pos->next)

#endif /* MODULE_SHELLMODULE_LIST_H_ */
