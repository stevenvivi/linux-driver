#ifndef _LIST_H
#define _LIST_H

struct list_head {                     
    struct list_head *next, *prev;  
 }; 

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

static inline void __list_add(struct list_head * newnode,
                                  struct list_head * prev,
                                  struct list_head * next)
{
    next->prev = newnode;
    newnode->next = next;
    newnode->prev = prev;
    prev->next = newnode;
}

//添加一个node
static inline void list_add(struct list_head *newnode, struct list_head *head)
{
    __list_add(newnode, head, head->next);
}

static inline void list_add_tail(struct list_head *newnode, struct list_head *head)
{
    __list_add(newnode, head->prev, head);
}

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}

static inline void __list_del(struct list_head * prev,
                                  struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}
//删除一个node
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = entry->prev = 0;
}

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); \
            pos = pos->next)

#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#endif