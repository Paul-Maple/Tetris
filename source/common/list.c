#include "list.h"

// ������������� �������� ������
void list_item_init(list_item_t *item)
{
    item->next = item->prev = NULL;
}

// �������� �� ������ ����
bool list_empty(const list_t *list) 
{
    ASSERT_NULL_PTR(list);
    
    if ((list->head == NULL) && (list->last == NULL))
        return true;
    else 
        return false;
}

// �������� ��������� �� ��������� �������
static list_item_t * next(const list_item_t *item)
{
  return item->next;
}

    /*** �������, �������� � ����� ������� � ������ ***/
// ������� �������� � ������ (���� ��� � �����)
void list_insert(list_t *list, list_item_t *item)
{
    ASSERT_NULL_PTR(list);
    ASSERT_NULL_PTR(item);
    
    item->next = NULL;
    
    // ������ ����
    if (list->head == NULL) 
    {
        item->prev = NULL;
        list->head = list->last = item;
    } 
    // ������ �� ����
    else
    {
        item->prev = list->last;
        list->last->next = item;
        list->last = item;
    }
}

// �������� �������� �� ������
void list_remove(list_t *list, list_item_t *item)
{
    ASSERT_NULL_PTR(list);
    ASSERT_NULL_PTR(item);
    
    // ��� ������ �������
    if (item->prev == NULL)  
        list->head = item->next;
    else 
        item->prev->next = item->next;
    
    // ��� ��������� �������
    if (item->next == NULL) 
        list->last = item->prev;
    else
        item->next->prev = item->prev;
    
    // ������� ���������� ���������� ��������
    list_item_init(item);
}

// �������� ������� �������� � ������
bool list_contains(const list_t *list, const list_item_t *item)
{   
    ASSERT_NULL_PTR(list);
    ASSERT_NULL_PTR(item);
    
    for(list_item_t *temp = list->head; temp != NULL; temp = next(temp)) 
    {
        if (temp == item) 
            return true;
    }
    
    return false;
}
