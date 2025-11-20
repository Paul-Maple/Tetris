#include "mcu.h"

// Инициализация модуля
void mcu_init(void)
{
        /* Тактирование */
    // Включить тактирование таймера и контроля питания (Для доступа к записи в регистр RCC_BDCR)
    // RCC_BDCR защищен от записи и требует предварительно разблокировки
    RCC->APB1ENR1 = RCC_APB1ENR1_LPTIM1EN | RCC_APB1ENR1_PWREN;
    
    // Тактирование регистра SYSCFG для настройки прерываний EXTI
    RCC->APB2ENR = RCC_APB2ENR_SYSCFGEN;
    
    // Включить тактирование GPIO
    RCC->AHB2ENR = RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN |                  // Порты А, В
                   RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOHEN;                   // Порты С, Н
    
    // Включение MCO для вывода системной частоты на GPIO
    RCC->CFGR |= RCC_CFGR_MCOPRE_1 |    // MCO = CLK / 4
                 RCC_CFGR_MCOSEL_0;     // SYSCLK
}
