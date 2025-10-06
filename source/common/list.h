#ifndef __LIST_H
#define __LIST_H

#include <typedefs.h>

// Статическая инициализация элемента списка
#define LIST_ITEM_STATIC_INIT()                                                 \
{                                                                               \
    .next = NULL,                                                               \
    .prev = NULL                                                                \
}

    /*** Элемент списка ***/
struct list_item_s;
typedef struct list_item_s list_item_t;

// Структура элемента списка
struct list_item_s
{   
    list_item_t *next;      
    list_item_t *prev;      
};

// Инициализация элемента списка
void list_item_init(list_item_t *item);
    
    /*** Список ***/
// Стуктура списка
typedef struct
{
    list_item_t *head;
    list_item_t *last;
} list_t;

    /*** Функции взаимодействия со списком ***/
// Проверка на пустой лист
bool list_empty(const list_t *list);

// Проверка наличия элемента в списке
bool list_contains(const list_t *list, const list_item_t *item);

// Вставка элемента в список (в конец)
void list_insert(list_t *list, list_item_t *item);

// Удаление элемента из списка
void list_remove(list_t *list, list_item_t *item);

#endif // __LIST_H
