#include "lptim1.h"

#include "mcu.h"
#include "nvic.h"
#include "clk.h"

// LPTIM1 (Режим 01)
#define LPTIM1_CLK_LSI       RCC_CCIPR_LPTIM1SEL_0
// LPTIM1 (Режим 11)
#define LPTIM1_CLK_LSE       (RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1)       

// Сбросить источник тактирования таймера
static void lptim1_tim_clear_clk(void)
{
    RCC->CCIPR &= ~LPTIM1_CLK_LSE; 
}

// Выбор LSI в качестве источника тактирования для таймера LPTIM1 (Режим 01)
static void lptim1_tim_set_lsi(void)
{
    RCC->CCIPR |= LPTIM1_CLK_LSI;
} 

// Выбор LSE в качестве источника тактирования для таймера LPTIM1 (Режим 11)
static void lptim1_tim_set_lse(void)
{
    RCC->CCIPR |= LPTIM1_CLK_LSE;
} 

// Оповещение модуля led о смене источника тактирования
void lptim1_clk_notice(void)
{
    lptim1_tim_clear_clk();
    
    switch (clk_lf_src)
    {
        case CLK_LF_SRC_LSI:
            lptim1_tim_set_lsi();
            break;
            
        case CLK_LF_SRC_LSE:
            lptim1_tim_set_lse();
            break;
            
        default:
            assert(false);
    }
}

void lptim1_init(void)
{
    // Выбор LSI в качестве источника тактирования для таймера LPTIM1 (Режим 01)
    lptim1_clk_notice();
    
    // Устновка делителя (= 128)
    // LPTIM1_CFGR может быть установлен ТОЛЬКО когда установлен LPTIM1_CR_ENABLE = 0
    LPTIM1->CFGR = LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2;
    
    // Разрешить прерывание по совпадению с ARR
    // LPTIM1_IER может быть установлен ТОЛЬКО когда установлен LPTIM1_CR_ENABLE = 0
    LPTIM1->IER = LPTIM_IER_ARRMIE;
    
    // Запустить таймер
    LPTIM1->CR = LPTIM_CR_ENABLE;
    
    // Ограничение счёта до 128
    // LPTIM1_ARR  может быть установлен только когда установлен LPTIM1_CR_ENABLE = 1
    LPTIM1->ARR = 128 - 1;
    
    // Режим непрерывного счёта до значения в регистре ARR
    // SNGSTRT может быть установлен только когда установлен LPTIM1_CR_ENABLE = 1
    LPTIM1->CR |= LPTIM_CR_CNTSTRT;
    
    // Разрешить прерывания
    nvic_irq_enable(LPTIM1_IRQn);
}
