#include "led.h"
#include <mcu.h>

/* LED используется для проверки работы программного таймера */

// Время переключения светодиода
#define LED_SWITCH_TIME      500

// Программный таймер
static timer_t led_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, led_state_switch);

// Инициализация модуля
void led_init(void)
{
    // Запуск программного таймера
    timer_start(&led_timer, TIMER_TICKS_MS(LED_SWITCH_TIME));
    // Инициализация состояния вывода светодиода ( Вкл.)
    GPIOB->ODR |= GPIO_ODR_OD13;
}

// Обработчик переключения состояния светодиода
void led_state_switch(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Запуск программного таймера 
    timer_start(timer, TIMER_TICKS_MS(LED_SWITCH_TIME));
    // Переключение состояния светодиода
    GPIOB->ODR ^= GPIO_ODR_OD13;
}
