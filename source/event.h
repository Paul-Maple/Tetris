#ifndef __EVENT_H
#define __EVENT_H

#include "list.h"

// Статическая инициализация события
#define EVENT_STATIC_INIT(_cb)                                                  \
{                                                                               \
    LIST_ITEM_STATIC_INIT(),                                                    \
    .cb   = _cb                                                                 \
}

// Прототип Сallback функции
typedef void (*event_cb_ptr) (void);

// Структура события
typedef struct
{
    // Элемент списка
    list_item_t item;
    // Обработчик события
    event_cb_ptr cb;
} event_t;

// Инициализация события
void event_init(event_t *event, event_cb_ptr cb);

// Генерация события
void event_raise(event_t *event);

// Обработка событий
void event_process(void);

#endif // __EVENT_H
