#ifndef __LED_H
#define __LED_H

#include <timer.h>

// Инициализация модуля
void led_init(void);

// Обработчик переключения состояния светодиода
void led_state_switch(timer_t *timer);

#endif // __LED_H
