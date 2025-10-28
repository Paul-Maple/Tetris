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
    
        /* Настройка PLL */
    // Выкл. PLL
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY);
    // Вход PLL - MSI
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;
    
    // f(VCO clock) = f(PLL clock input) * (PLLN / PLLM) = 4 MHz / 1 * 30 = 120 MHz
    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM_0 | RCC_PLLCFGR_PLLM_1 | RCC_PLLCFGR_PLLM_2);    // PLLM = 1
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_1 | RCC_PLLCFGR_PLLN_2 | 
                    RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_4 |                           // PLLN = 30
                    RCC_PLLCFGR_PLLR_0;                                                 // PLLR = 4                                          
    // f (PLLCLK) = VCO / PLLR = 120 MHz / 4 = 30 MHz
}

void mcu_set_pll(void)
{
    // Вкл. PLL
    RCC->CR |= RCC_CR_PLLON;
    // Вкл. выход PLLCLK
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
    // Ожидание готовности PLL
    while (!(RCC->CR & RCC_CR_PLLRDY));
    // PLL selected as system clock
    RCC->CFGR |= RCC_CFGR_SW_0 | RCC_CFGR_SW_1;
}

void mcu_reset_pll(void)
{
    // MSI selected as system clock
    RCC->CFGR &= ~(RCC_CFGR_SW_0 | RCC_CFGR_SW_1);
    // Выкл. PLL
    RCC->CR &= ~RCC_CR_PLLON;
}