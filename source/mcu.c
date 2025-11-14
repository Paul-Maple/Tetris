#include "mcu.h"

// Инициализация модуля
void mcu_init(void)
{
        /* Тактирование */
    // Включить тактирование таймера и контроля питания (Для доступа к записи в регистр RCC_BDCR)
    // RCC_BDCR защищен от записи и требует предварительно разблокировки
    RCC->APB1ENR1 = RCC_APB1ENR1_PWREN | RCC_APB1ENR1_LPTIM1EN;
    
    // Тактирование регистра SYSCFG для настройки прерываний EXTI
    RCC->APB2ENR = RCC_APB2ENR_SYSCFGEN;
    
    // Включить тактирование GPIO
    RCC->AHB2ENR = RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN |                  // Порты А, В
                   RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOHEN;                   // Порты С, Н
    
    // Включение MCO для вывода частоты на GPIO для проверки
    RCC->CFGR = //RCC_CFGR_MCOPRE_1 |                     // MCO = CLK / 4  (Раскомментирую при более высоких частотах)
                //RCC_CFGR_MCOSEL_0 | RCC_CFGR_MCOSEL_2;  // PLL
                RCC_CFGR_MCOSEL_0;                      // SYSCLK
    
        /* Настройка PLL */
    // Выкл. PLL
    /*RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY);
    
    // Вход PLL - MSI
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;
    
    // f(VCO clock) = f(PLL clock input) * (PLLN / PLLM) = 4 MHz * ( 40 / 1 ) = 160 MHz
    // f (PLLCLK) = VCO / PLLR = 160 MHz / 4 = 40 MHz
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_0 | RCC_PLLCFGR_PLLM_1 | RCC_PLLCFGR_PLLM_2);    // PLLM = 1
    RCC->PLLCFGR |= //RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_5 |                           // PLLN = 40
                    RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 |                           // PLLN = 20 - при 20 всё норм
                    RCC_PLLCFGR_PLLR_0;                                                 // PLLR = 4
    
    // Вкл. PLL
    RCC->CR |= RCC_CR_PLLON;
    // Вкл. выход PLLCLK
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
    // Ожидание готовности PLL
    while (!(RCC->CR & RCC_CR_PLLRDY));
    // Установить задержку на чтение Flash memory (f <= 48 MHz)
    FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
    // PLL selected as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
*/
}
