#include "mcu.h"

// Инициализация модуля
void mcu_init(void)
{
        /* Тактирование */
    // Включить тактирование таймера и контроля питания (Для доступа к записи в регистр RCC_BDCR)
    // RCC_BDCR защищен от записи и требует предварительно разблокировки
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN | RCC_APB1ENR1_LPTIM1EN;
     
    // Включить тактирование GPIO
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN |     // Порты А, В
                    RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOHEN;      // Порты С, Н
}
