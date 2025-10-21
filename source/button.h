#ifndef __BUTTON_H
#define __BUTTON_H

#include <timer.h>
#include <event.h>

/* Для каждой кнопки предусмотрен свой таймер,    *
 * для программного устранения дребезга контактов */

// Структура кнопки
typedef struct
{
    // Таймер (ПЕРВЫМ В СТРУКТУРЕ !!!!)
    timer_t timer;
    // Флаг нажатия кнопки    
    bool pressed;
    
} button_t;
// Попробовать реализовать весь модуль на одном таймере 

// Инициализация модуля
void button_init(void);

// Обработчики прерываний кнопок
void button_0_iqr(void);

void button_1_iqr(void);

void button_2_iqr(void);

void button_3_iqr(void);

#endif // __BUTTON_H
