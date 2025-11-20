#include "list.h"

// Инициализация элемента списка
void list_item_init(list_item_t *item)
{
    item->next = item->prev = NULL;
}

// Проверка на пустой лист
bool list_empty(const list_t *list) 
{
    ASSERT_NULL_PTR(list);
    
    if ((list->head == NULL) && (list->last == NULL))
        return true;
    else 
        return false;
}

// Получает указатель на следующий элемент
static list_item_t * next(const list_item_t *item)
{
  return item->next;
}

    /*** Вставка, удаление и поиск элемета в списке ***/
// Вставка элемента в список (пока что в конец)
void list_insert(list_t *list, list_item_t *item)
{
    ASSERT_NULL_PTR(list);
    ASSERT_NULL_PTR(item);
    
    item->next = NULL;
    
    // Список пуст
    if (list->head == NULL) 
    {
        item->prev = NULL;
        list->head = list->last = item;
    } 
    // Список не пуст
    else
    {
        item->prev = list->last;
        list->last->next = item;
        list->last = item;
    }
}

// Удаление элемента из списка
void list_remove(list_t *list, list_item_t *item)
{
    ASSERT_NULL_PTR(list);
    ASSERT_NULL_PTR(item);
    
    // Это первый элемент
    if (item->prev == NULL)  
        list->head = item->next;
    else 
        item->prev->next = item->next;
    
    // Это последний элемент
    if (item->next == NULL) 
        list->last = item->prev;
    else
        item->next->prev = item->prev;
    
    // Очистка указателей удаляемого элемента
    list_item_init(item);
}

// Проверка наличия элемента в списке
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
