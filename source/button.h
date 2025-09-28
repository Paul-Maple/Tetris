#ifndef __BUTTON_H
#define __BUTTON_H

#include "timer.h"

// Структура кнопки
typedef struct
{
    timer_t timer;      // Таймер
    bool pressed;       // Флаг нажатия кнопки
    
} button_t;

// Инициализация модуля
void button_init(void);

// Обработчики прерываний кнопок
void button_0_iqr(void);

void button_1_iqr(void);

void button_2_iqr(void);

void button_3_iqr(void);

#endif // __BUTTON_H
