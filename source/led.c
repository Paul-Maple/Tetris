#include "led.h"
#include <mcu.h>

/* LED используется для проверки работы программного таймера */

// Программный таймер LED_LPTIM1
static timer_t led_timer;

// Инициализация модуля
void led_init(void)
{
    // Инициализация программного таймера LED_LPTIM1
    timer_init(&led_timer, TIMER_MODE_CONTINUOUS, led_state_switch);
    // Запуск программного таймера 
    timer_start(&led_timer, TIMER_TICKS_MS(500));
    // Инициализация состояния вывода светодиода ( Вкл.)
    GPIOB->ODR |= GPIO_ODR_OD13;
}

// Обработчик переключения состояния светодиода
void led_state_switch(timer_t *timer)
{
    // Переключение состояния светодиода
    GPIOB->ODR ^= GPIO_ODR_OD13;
}
