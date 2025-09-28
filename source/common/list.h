#ifndef __LIST_H
#define __LIST_H

#include <typedefs.h>

// ����������� ������������� �������� ������
#define LIST_ITEM_STATIC_INIT                                                   \
{                                                                               \
    .next = NULL,                                                               \
    .prev = NULL                                                                \
}

    /*** ������� ������ ***/
struct list_item_s;
typedef struct list_item_s list_item_t;

// ��������� �������� ������
struct list_item_s
{   
    list_item_t *next;      
    list_item_t *prev;      
};

// ������������� �������� ������
void list_item_init(list_item_t *item);
    
    /*** ������ ***/
// �������� ������
typedef struct
{   
    list_item_t *head;
    list_item_t *last;
} list_t;

    /*** ������� �������������� �� ������� ***/
// �������� �� ������ ����
bool list_empty(const list_t *list);

// �������� ������� �������� � ������
bool list_contains(const list_t *list, const list_item_t *item);

// ������� �������� � ������ (� �����)
void list_insert(list_t *list, list_item_t *item);

// �������� �������� �� ������
void list_remove(list_t *list, list_item_t *item);

#endif // __LIST_H
