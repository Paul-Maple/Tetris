#include "button.h"
#include <nvic.h>

// Сброс флага прерывания (При записи "1")
#define BUTTON_ISR_FLAG_CLEAR(line)     (EXTI->PR1 |= EXTI_PR1_PIF##line)

// Тики таймера для выжидания дребезга контактов (10 мСек)
// TODO: Подобрать правильное время выжидания
#define BUTTON_CONTACT_BOUNCE_TIME      TIMER_TICKS_MS(10)

// Статическая инициализация кнопки         
#define BUTTON_STATIC_INIT(mode, timer_cb)                                      \
{                                                                               \
    TIMER_STATIC_INIT(mode, timer_cb),                                          \
    .pressed = false                                                            \
}

static void button_switch_state_notice(button_t *button, bool state)
{
    ASSERT_NULL_PTR(button);
    // Оповещение модулей
    // module_1_button_switch_state_notice(button, state);
    // module_2_button_switch_state_notice(button, state);
    // и т.д.
}

// Обработчик события нажатия кнопки
/* Вызываются как callback у таймера в случае, *
 * если таймер дребезга контактов отработал    */
static void button_pressed_event_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Получить указатель на нажатую кнопку
    button_t *button = (button_t *)timer;
    
    // Если кнопка была не в нажатом состоянии
    if (!button->pressed)
    {
        // Установка флага нажатия кнопки
        button->pressed = true; 
        
        // Оповещение модулей о нажатии кнопки
        button_switch_state_notice(button, button->pressed);
        
        return;
    }
    
    // Кнопка была в нажатом состоянии
    else
    {
        // Установка флага: кнопка не нажата
        button->pressed = false;
        
        // Оповещение модулей об отпускании кнопки
        button_switch_state_notice(button, button->pressed);
    }
}

// Кнопки
static button_t button_0 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb);
//static button_t button_1 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb);
//static button_t button_2 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb);
//static button_t button_3 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb);

void button_init(void)
{
        /*** Настройка прерываний кнопок ***/
    // Запрос на прерывание с линий EXTI 0, 1, 2, 4
    EXTI->IMR1 |= EXTI_IMR1_IM0 | EXTI_IMR1_IM1 | EXTI_IMR1_IM2 | EXTI_IMR1_IM3;
    
    // Разрешить прерывания по фронту сигнала
    EXTI->RTSR1 |= EXTI_RTSR1_RT0 | EXTI_RTSR1_RT1 | EXTI_RTSR1_RT2 | EXTI_RTSR1_RT3;
    // Разрешить прерывания по спаду сигнала 
    EXTI->FTSR1 |= EXTI_FTSR1_FT0 | EXTI_FTSR1_FT1 | EXTI_FTSR1_FT2 | EXTI_FTSR1_FT3;
    
    // Прерывания на пинах 0, 1, 2, 3 порта С
    SYSCFG->EXTICR[0] = (SYSCFG_EXTICR1_EXTI0 & SYSCFG_EXTICR1_EXTI0_PC) | 
                        (SYSCFG_EXTICR1_EXTI1 & SYSCFG_EXTICR1_EXTI1_PC) | 
                        (SYSCFG_EXTICR1_EXTI2 & SYSCFG_EXTICR1_EXTI2_PC) | 
                        (SYSCFG_EXTICR1_EXTI3 & SYSCFG_EXTICR1_EXTI3_PC);
    
    // Включить перывания в NVIC
    nvic_irq_enable(EXTI0_IRQn);
    nvic_irq_enable(EXTI1_IRQn);
    nvic_irq_enable(EXTI2_IRQn);
    nvic_irq_enable(EXTI3_IRQn);
}

    /*** Обработчики прерываний ***/
void button_0_iqr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(0);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_0.timer, BUTTON_CONTACT_BOUNCE_TIME);
}

void button_1_iqr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(1);
        
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_0.timer, BUTTON_CONTACT_BOUNCE_TIME);
}

void button_2_iqr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(2);
        
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_0.timer, BUTTON_CONTACT_BOUNCE_TIME);
}

void button_3_iqr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(3);
        
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_0.timer, BUTTON_CONTACT_BOUNCE_TIME);
}
