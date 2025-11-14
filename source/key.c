#include "key.h"
#include "nvic.h"

// Макрос для сброса флага прерывания (При записи "1")
#define KEY_ISR_FLAG_CLEAR(line)     (EXTI->PR1 |= EXTI_PR1_PIF##line)

// Тики таймера для выжидания дребезга контактов
/* Время подобрано, не менять !!!! 
   По факту время дребезга на графике не превышало 300 мкСек */
#define KEY_CONTACT_BOUNCE_TIME      TIMER_TICKS_MS(20)

// Статическая инициализация кнопки
#define KEY_STATIC_INIT(mode, timer_cb, _name)                               \
{                                                                               \
    TIMER_STATIC_INIT(mode, timer_cb),                                          \
    .pressed = false,                                                           \
    .name = _name                                                               \
}

// Функция оповещения модуля "tetris"
extern void tetris_key_notice(key_name_t name, bool state);

// Обработчик события нажатия кнопки
/* Вызываются как callback у таймера в случае, *
 * если таймер дребезга контактов отработал    */
static void key_pressed_event_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Получить указатель на нажатую кнопку
    key_t *key = (key_t *)timer;
    
    // --- Кнока НАЖАТА --- //
    // Если кнопка была не в нажатом состоянии
    if (!key->pressed)
    {
        // Установка флага нажатия кнопки
        key->pressed = true;
        // Запретить прерывания по фронту и разрешить по спаду
        EXTI->RTSR1 &= ~(1 << key->name);
        EXTI->FTSR1 |= (1 << key->name);
    }
    
    // --- Кнока ОТПУЩЕНА --- //
    // Кнопка была в нажатом состоянии
    else
    {
        // Установка флага: кнопка не нажата
        key->pressed = false;
        // Запретить прерывания по фронту и разрешить по спаду
        EXTI->RTSR1 |= (1 << key->name);
        EXTI->FTSR1 &= ~(1 << key->name);
    }
    
    // Оповещение модулей об отпускании кнопки
    tetris_key_notice(key->name, key->pressed);
}

// Кнопки (Порядок имён и имена НЕ МЕНЯТЬ!!!! )
static key_t key_0 = KEY_STATIC_INIT(TIMER_MODE_ONE_SHOT, key_pressed_event_cb, KEY_NAME_DOWN);
static key_t key_1 = KEY_STATIC_INIT(TIMER_MODE_ONE_SHOT, key_pressed_event_cb, KEY_NAME_RIGHT);
static key_t key_2 = KEY_STATIC_INIT(TIMER_MODE_ONE_SHOT, key_pressed_event_cb, KEY_NAME_LEFT);
static key_t key_3 = KEY_STATIC_INIT(TIMER_MODE_ONE_SHOT, key_pressed_event_cb, KEY_NAME_ROTATE);

void key_init(void)
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
    KEY_ISR_FLAG_CLEAR(0);
    KEY_ISR_FLAG_CLEAR(1);
    KEY_ISR_FLAG_CLEAR(2);
    KEY_ISR_FLAG_CLEAR(3);
    
    // Включить перывания в NVIC
    nvic_irq_enable(EXTI0_IRQn);
    nvic_irq_enable(EXTI1_IRQn);
    nvic_irq_enable(EXTI2_IRQn);
    nvic_irq_enable(EXTI3_IRQn);
}

    /*** Обработчики прерываний ***/
void key_0_isr(void)
{
    // Сбросить флаг прерывания
    KEY_ISR_FLAG_CLEAR(0);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&key_0.timer, KEY_CONTACT_BOUNCE_TIME);
}

void key_1_isr(void)
{
    // Сбросить флаг прерывания
    KEY_ISR_FLAG_CLEAR(1);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&key_1.timer, KEY_CONTACT_BOUNCE_TIME);
}

void key_2_isr(void)
{
    // Сбросить флаг прерывания
    KEY_ISR_FLAG_CLEAR(2);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&key_2.timer, KEY_CONTACT_BOUNCE_TIME);
}

void key_3_isr(void)
{
    // Сбросить флаг прерывания
    KEY_ISR_FLAG_CLEAR(3);
    
    // Запуск таймера для выжидания дребезга контактов
    timer_start(&key_3.timer, KEY_CONTACT_BOUNCE_TIME);
}
