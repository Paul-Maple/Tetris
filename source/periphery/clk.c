#include "clk.h"
#include <nvic.h>
#include <event.h>

// Модули, в которых есть тактирование от LSE/LSI
#include <timer.h>

// События сбоя и стабилизации LSE
event_t clk_lse_fail_event;
event_t clk_lse_ready_event;

// Инициализация источников тактирования (LSI и LSE)
void clk_init(void)
{
    // Инициализация событий стабилизации/сбоя LSE
    event_init(&clk_lse_ready_event, clk_lse_ready);
    event_init(&clk_lse_fail_event, clk_lse_fail);

        /* Запуск LSI */
    // Разрешить работу LSI
    RCC->CSR = RCC_CSR_LSION;
    
        /* Запуск LSE */  
    PWR->CR1 |= PWR_CR1_DBP;                            // Разрешаем запись в регистр RCC_BDCR
    RCC->BDCR = RCC_BDCR_BDRST;                         // Сбросить все значения в регисте BDCR
    RCC->CIER = RCC_CIER_LSERDYIE | RCC_CIER_LSECSSIE;  // Разрешить прерывание, вызванное стабилизацией и сбоем LSE
    EXTI->IMR1 = EXTI_IMR1_IM19;                        // Включить запрос прерывания с LSE 
    EXTI->RTSR1 = EXTI_RTSR1_RT19;                      // Триггер по фронту сигнала прерывания
    RCC->BDCR = RCC_BDCR_LSEON;                         // Разрешить работу LSE
    
    // Включить прерывания LSE в NVIC
    nvic_irq_enable(TAMP_STAMP_IRQn);   // Сбой
    nvic_irq_enable(RCC_IRQn);          // Стабилизация
}

    /*** Оповещение тактируемых модулей ***/
// Текущий источник НЧ
clk_lf_src_t clk_lf_src = CLK_LF_SRC_LSI;

// Производит смену источника НЧ и оповещение модулей о изменении НЧ
static void clk_lf_src_changed(clk_lf_src_t src)
{
    clk_lf_src = src;
    
    // Оповещение модулей
    timer_clk_notice();
    // FUTURE: Добавить другие модули
}

    /*** Обработчики событий LSE ***/
// Обработчик сбоя LSE
void clk_lse_fail(void)
{
    // Сбросить все значения в регисте BDCR
    RCC->BDCR = RCC_BDCR_BDRST;
    // Разрешить работу LSE
    RCC->BDCR = RCC_BDCR_LSEON;
    // Оповещение модуля led о смене источника тактирования
    clk_lf_src_changed(CLK_LF_SRC_LSI);    
}

// Обработчик готовности LSE
void clk_lse_ready(void)
{
    // Активация режима безопасности CSS
    RCC->BDCR |= RCC_BDCR_LSECSSON;
    // Разрешить прерывание, вызванное сбоем LSE
    RCC->CIER |= RCC_CIER_LSECSSIE;
    // Оповещение модуля led о смене источника тактирования
    clk_lf_src_changed(CLK_LF_SRC_LSE);
}

    /*** Обработчики прерываний LSE ***/
// Обработчик прерывания сбоя LSE 
void clk_lse_fail_isr(void)
{   
    // Сброс флагов прерывания сбоя LSE 
    RCC->CICR = RCC_CICR_LSECSSC;
    EXTI->PR1 |= EXTI_PR1_PIF19;
    
    // Запретить прерывание, вызванное сбоем LSE
    RCC->CIER &= ~RCC_CIER_LSECSSIE;
    
    // Добавить событие в очередь на обработку
    event_raise(&clk_lse_fail_event);
}

// Обработчик прерывания стабилизации LSE 
void clk_lse_ready_isr(void)
{
    // Сброс флага стабилизации LSE
    RCC->CICR |= RCC_CICR_LSERDYC;
    
    // Добавить событие в очередь на обработку
    event_raise(&clk_lse_ready_event);
}
