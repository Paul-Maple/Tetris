#include "button.h"
#include <nvic.h>

// Макрос для сброса флага прерывания (При записи "1")
#define BUTTON_ISR_FLAG_CLEAR(line)     (EXTI->PR1 |= EXTI_PR1_PIF##line)

// Тики таймера для выжидания дребезга контактов
/* Время подобрано, не менять !!!! 
   По факту время дребезга на графике не превышало 300 мкСек */
#define BUTTON_CONTACT_BOUNCE_TIME      TIMER_TICKS_MS(20)

// Статическая инициализация кнопки
#define BUTTON_STATIC_INIT(mode, timer_cb, _name)                               \
{                                                                               \
    TIMER_STATIC_INIT(mode, timer_cb),                                          \
    .pressed = false,                                                           \
    .name = _name                                                               \
}

// Функция оповещения модуля "tetris"
extern void tetris_button_pressed_notice(button_name_t name, bool state);

// Обработчик события нажатия кнопки
/* Вызываются как callback у таймера в случае, *
 * если таймер дребезга контактов отработал    */
static void button_pressed_event_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Получить указатель на нажатую кнопку
    button_t *button = (button_t *)timer;
    
    // --- Кнока НАЖАТА --- //
    // Если кнопка была не в нажатом состоянии
    if (!button->pressed)
    {
        // Установка флага нажатия кнопки
        button->pressed = true;
        // Запретить прерывания по фронту и разрешить по спаду
        EXTI->RTSR1 &= ~(1 << button->name);
        EXTI->FTSR1 |= (1 << button->name);
    }
    
    // --- Кнока ОТПУЩЕНА --- //
    // Кнопка была в нажатом состоянии
    else
    {
        // Установка флага: кнопка не нажата
        button->pressed = false;
        // Запретить прерывания по фронту и разрешить по спаду
        EXTI->RTSR1 |= (1 << button->name);
        EXTI->FTSR1 &= ~(1 << button->name);
    }
    
    // Оповещение модулей об отпускании кнопки
    tetris_button_pressed_notice(button->name, button->pressed);
}

// Кнопки (Порядок имён и имена НЕ МЕНЯТЬ!!!! )
static button_t button_0 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb, BUTTON_NAME_DOWN);
static button_t button_1 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb, BUTTON_NAME_RIGHT);
static button_t button_2 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb, BUTTON_NAME_LEFT);
static button_t button_3 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_pressed_event_cb, BUTTON_NAME_ROTATE);

void button_init(void)
{
        /*** Настройка прерываний кнопок ***/    
    // Разрешить прерывания по фронту сигнала
    EXTI->RTSR1 |= EXTI_RTSR1_RT0 | EXTI_RTSR1_RT1 | EXTI_RTSR1_RT2 | EXTI_RTSR1_RT3;
    
    // Прерывания на пинах 0, 1, 2, 3 порта С
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC | 
                         SYSCFG_EXTICR1_EXTI1_PC | 
                         SYSCFG_EXTICR1_EXTI2_PC | 
                         SYSCFG_EXTICR1_EXTI3_PC;
    
    // Запрос на прерывание с линий EXTI 0, 1, 2, 3
    EXTI->IMR1 |= EXTI_IMR1_IM0 | EXTI_IMR1_IM1 | EXTI_IMR1_IM2 | EXTI_IMR1_IM3;
    
    // Сбросить флаги прерывания
    BUTTON_ISR_FLAG_CLEAR(0);
    BUTTON_ISR_FLAG_CLEAR(1);
    BUTTON_ISR_FLAG_CLEAR(2);
    BUTTON_ISR_FLAG_CLEAR(3);
    
    // Включить перывания в NVIC
    nvic_irq_enable(EXTI0_IRQn);
    nvic_irq_enable(EXTI1_IRQn);
    nvic_irq_enable(EXTI2_IRQn);
    nvic_irq_enable(EXTI3_IRQn);
}

    /*** Обработчики прерываний ***/
void button_0_isr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(0);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_0.timer, BUTTON_CONTACT_BOUNCE_TIME);
}

void button_1_isr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(1);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_1.timer, BUTTON_CONTACT_BOUNCE_TIME);
}

void button_2_isr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(2);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_2.timer, BUTTON_CONTACT_BOUNCE_TIME);
}

void button_3_isr(void)
{
    // Сбросить флаг прерывания
    BUTTON_ISR_FLAG_CLEAR(3);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&button_3.timer, BUTTON_CONTACT_BOUNCE_TIME);
}
