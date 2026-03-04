#include "clk.h"
#include "nvic.h"
#include "event.h"
// Модули, в которых есть тактирование от LSE/LSI
#include "timer.h"

// События сбоя и стабилизации LSE
static event_t clk_lse_fail_event;
static event_t clk_lse_ready_event;

// Предварительное объявление обработчиков сбоя и готовности LSE
static void clk_lse_fail(void);
static void clk_lse_ready(void);

void clk_init(void)
{
        /* Настройка PLL */
    // Выкл. PLL
    RCC->CR &= ~RCC_CR_PLLON;
    while (RCC->CR & RCC_CR_PLLRDY);
    
    // Установить задержку на чтение Flash (f <= 80 MHz) и предварительную выборку
    FLASH->ACR |= FLASH_ACR_LATENCY_4WS | FLASH_ACR_PRFTEN;
    
    // Сбросить регистр PLLCFGR
    RCC->PLLCFGR = 0x0;
    
    // f(PLLCLK) = (( 4 MHz / 1 ) * 80 ) / 4 = 80 MHz
    // f(PLLQ)   = (( 4 MHz / 1 ) * 80 ) / 8 = 40 MHz                           // PLLM = 1
    RCC->PLLCFGR =  RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_6 |                   // PLLN = 80
                    RCC_PLLCFGR_PLLR_0 |                                        // PLLR = 4
                    RCC_PLLCFGR_PLLQ_0 | RCC_PLLCFGR_PLLQ_1;                    // PLLQ = 8
    
    // Вход PLL - MSI
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;
    
    // Вкл. PLL
    RCC->CR |= RCC_CR_PLLON;
    // Ожидание готовности PLL
    while (!(RCC->CR & RCC_CR_PLLRDY));
    // Вкл. выход PLLCLK и PLLQ
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN | RCC_PLLCFGR_PLLQEN; 
    // PLL selected as system clock
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    // Ожидание переключения системной частоты на PLL
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
    
    // Инициализация событий стабилизации/сбоя LSE
    event_init(&clk_lse_ready_event, clk_lse_ready);
    event_init(&clk_lse_fail_event, clk_lse_fail);
    
        /* Запуск LSI */
    RCC->CSR = RCC_CSR_LSION;           // Разрешить работу LSI
    
        /* Запуск LSE */
    PWR->CR1 |= PWR_CR1_DBP;            // Разрешаем запись в регистр RCC_BDCR
    RCC->BDCR = RCC_BDCR_BDRST;         // Сбросить все значения в регисте BDCR
    RCC->CIER = RCC_CIER_LSERDYIE;      // Разрешить прерывание, вызванное стабилизацией LSE
    EXTI->IMR1 = EXTI_IMR1_IM19;        // Включить запрос прерывания с LSE 
    EXTI->RTSR1 = EXTI_RTSR1_RT19;      // Триггер по фронту сигнала прерывания
    RCC->BDCR = RCC_BDCR_LSEON;         // Разрешить работу LSE
    
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
static void clk_lse_fail(void)
{
    // Сбросить все значения в регисте BDCR
    RCC->BDCR = RCC_BDCR_BDRST;
    // Запуск LSE после сбоя
    RCC->BDCR = RCC_BDCR_LSEON;
    // Оповещение модуля led о смене источника тактирования
    clk_lf_src_changed(CLK_LF_SRC_LSI);    
}

static void clk_lse_ready(void)
{
    // Активация режима безопасности CSS
    RCC->BDCR |= RCC_BDCR_LSECSSON;
    // Разрешить прерывание, вызванное сбоем LSE
    RCC->CIER |= RCC_CIER_LSECSSIE;
    // Оповещение модуля led о смене источника тактирования
    clk_lf_src_changed(CLK_LF_SRC_LSE);
}

    /*** Обработчики прерываний LSE ***/
void clk_lse_fail_isr(void)
{   
    // Сброс флагов прерывания сбоя LSE 
    RCC->CICR |= RCC_CICR_LSECSSC;
    EXTI->PR1 |= EXTI_PR1_PIF19;
    
    // Запретить прерывание, вызванное сбоем LSE
    RCC->CIER &= ~RCC_CIER_LSECSSIE;
    
    // Добавить событие в очередь на обработку
    event_raise(&clk_lse_fail_event);
}

void clk_lse_ready_isr(void)
{
    // Сброс флага стабилизации LSE
    RCC->CICR |= RCC_CICR_LSERDYC;
    
    // Добавить событие в очередь на обработку
    event_raise(&clk_lse_ready_event);
}
