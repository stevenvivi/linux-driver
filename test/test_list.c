#include  <stdio.h>
#include  <stdlib.h>
#include "list.h"

struct my_list{
    struct list_head list;
    char value[10]; 
};

int main(int argc, char **argv){
    
    struct my_list *tmp;
    struct list_head *pos, *q;
    unsigned int i;
    
    struct my_list mylist;
    INIT_LIST_HEAD(&mylist.list); /*初始化链表头*/
    
    /* 给mylist增加元素 */
    for(i=3; i!=0; --i){
        tmp= (struct my_list *)malloc(sizeof(struct my_list));
        
        /* 或者INIT_LIST_HEAD(&tmp->list); */
        printf("enter value:");
        scanf("%s", tmp->value);
        
        
        list_add(&(tmp->list), &(mylist.list));
        /* 也可以用list_add_tail() 在表尾增加元素*/
    }
    printf("\n");
    
    printf("traversing the list using list_for_each()\n");
    list_for_each(pos, &mylist.list){
        
    /* 在这里 pos->next 指向next 节点, pos->prev指向前一个节点.这里的节点是
        struct my_list类型. 但是，我们需要访问节点本身,而不是节点中的list字段，
        宏list_entry()正是为此目的。*/     
        
    tmp= list_entry(pos, struct my_list, list);
    printf("%s ", tmp->value);
    }
    printf("\n");

    printf("deleting the list using list_for_each_safe()\n");
    list_for_each_safe(pos, q,&mylist.list){
        tmp= list_entry(pos, struct my_list, list);
        printf("%s ", tmp->value);
        list_del(pos);
        free(tmp);
    }
}