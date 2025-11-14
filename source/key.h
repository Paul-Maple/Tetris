#ifndef __KEY_H
#define __KEY_H

#include "timer.h"
#include "event.h"

/* Для каждой кнопки предусмотрен свой таймер,    *
 * для программного устранения дребезга контактов */

// Перечисление функциональных имён кнопок
typedef enum
{
    /* Порядок имён НЕ МЕНЯТЬ!!!! */
    KEY_NAME_DOWN,
    KEY_NAME_RIGHT,
    KEY_NAME_LEFT,
    KEY_NAME_ROTATE,
    
} key_name_t;

// Структура кнопки
typedef struct
{
    // Таймер (ПЕРВЫМ В СТРУКТУРЕ !!!!)
    timer_t timer;
    // Флаг нажатия кнопки    
    bool pressed;
    // Имя кнопки
    const key_name_t name;
    
} key_t;
// TODO: Попробовать реализовать весь модуль на одном таймере 

// Инициализация модуля
void key_init(void);

// Обработчики прерываний кнопок
void key_0_isr(void);

void key_1_isr(void);

void key_2_isr(void);

void key_3_isr(void);

#endif // __KEY_H
