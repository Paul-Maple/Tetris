#ifndef __BUTTON_H
#define __BUTTON_H

#include <timer.h>
#include <event.h>

/* Для каждой кнопки предусмотрен свой таймер,    *
 * для программного устранения дребезга контактов */

// Перечисление функциональных имён кнопок
typedef enum
{
    /* Порядок имён НЕ МЕНЯТЬ!!!! */
    BUTTON_NAME_DOWN,
    BUTTON_NAME_RIGHT,
    BUTTON_NAME_LEFT,
    BUTTON_NAME_ROTATE,
    
} button_name_t;

// Структура кнопки
typedef struct
{
    // Таймер (ПЕРВЫМ В СТРУКТУРЕ !!!!)
    timer_t timer;
    // Флаг нажатия кнопки    
    bool pressed;
    // Имя кнопки
    const button_name_t name;
    
} button_t;
// TODO: Попробовать реализовать весь модуль на одном таймере 

// Инициализация модуля
void button_init(void);

// Обработчики прерываний кнопок
void button_0_isr(void);

void button_1_isr(void);

void button_2_isr(void);

void button_3_isr(void);

#endif // __BUTTON_H
