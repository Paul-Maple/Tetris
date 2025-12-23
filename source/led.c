#include "led.h"
#include <mcu.h>

/* LED используется для проверки работы программного таймера */

// Программный таймер
//static timer_t led_timer = TIMER_STATIC_INIT(TIMER_MODE_CONTINUOUS, led_state_switch);
static timer_t led_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, led_state_switch);

// Инициализация модуля
void led_init(void)
{
    // Запуск программного таймера 
    timer_start(&led_timer, TIMER_TICKS_MS(500));
    // Инициализация состояния вывода светодиода ( Вкл.)
    GPIOB->ODR |= GPIO_ODR_OD13;
}

// Обработчик переключения состояния светодиода
void led_state_switch(timer_t *timer)
{
    timer_start(&led_timer, TIMER_TICKS_MS(500));
    // Переключение состояния светодиода
    GPIOB->ODR ^= GPIO_ODR_OD13;
}
