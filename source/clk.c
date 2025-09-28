#include "clk.h"
#include "nvic.h"
#include "event.h"

// ������, � ������� ���� ������������ �� LSE/LSI
#include "timer.h"

// ������� ���� � ������������ LSE
event_t clk_lse_fail_event;
event_t clk_lse_ready_event;

// ������������� ���������� ������������ (LSI � LSE)
void clk_init(void)
{
    // ������������� ������� ������������/���� LSE
    event_init(&clk_lse_ready_event, clk_lse_ready);
    event_init(&clk_lse_fail_event, clk_lse_fail);

        /* ������ LSI */
    // ��������� ������ LSI
    RCC->CSR = RCC_CSR_LSION;
    
        /* ������ LSE */  
    PWR->CR1 |= PWR_CR1_DBP;                            // ��������� ������ � ������� RCC_BDCR
    RCC->BDCR = RCC_BDCR_BDRST;                         // �������� ��� �������� � ������� BDCR
    RCC->CIER = RCC_CIER_LSERDYIE | RCC_CIER_LSECSSIE;  // ��������� ����������, ��������� ������������� � ����� LSE
    EXTI->IMR1 = EXTI_IMR1_IM19;                        // �������� ������ ���������� � LSE 
    EXTI->RTSR1 = EXTI_RTSR1_RT19;                      // ������� �� ������ ������� ����������
    RCC->BDCR = RCC_BDCR_LSEON;                         // ��������� ������ LSE
    
    // �������� ���������� LSE � NVIC
    nvic_irq_enable(TAMP_STAMP_IRQn);   // ����
    nvic_irq_enable(RCC_IRQn);          // ������������
}

    /*** ���������� ����������� ������� ***/
// ������� �������� ��
clk_lf_src_t clk_lf_src = CLK_LF_SRC_LSI;

// ���������� ����� ��������� �� � ���������� ������� � ��������� ��
static void clk_lf_src_changed(clk_lf_src_t src)
{
    clk_lf_src = src;
    
    // ���������� �������
    timer_clk_notice();
    // FUTURE: �������� ������ ������
}

    /*** ����������� ������� LSE ***/
// ���������� ���� LSE
void clk_lse_fail(void)
{
    // �������� ��� �������� � ������� BDCR
    RCC->BDCR = RCC_BDCR_BDRST;
    // ��������� ������ LSE
    RCC->BDCR = RCC_BDCR_LSEON;
    // ���������� ������ led � ����� ��������� ������������
    clk_lf_src_changed(CLK_LF_SRC_LSI);    
}

// ���������� ���������� LSE
void clk_lse_ready(void)
{
    // ��������� ������ ������������ CSS
    RCC->BDCR |= RCC_BDCR_LSECSSON;
    // ��������� ����������, ��������� ����� LSE
    RCC->CIER |= RCC_CIER_LSECSSIE;
    // ���������� ������ led � ����� ��������� ������������
    clk_lf_src_changed(CLK_LF_SRC_LSE);
}

    /*** ����������� ���������� LSE ***/
// ���������� ���������� ���� LSE 
void clk_lse_fail_isr(void)
{   
    // ����� ������ ���������� ���� LSE 
    RCC->CICR = RCC_CICR_LSECSSC;
    EXTI->PR1 |= EXTI_PR1_PIF19;
    
    // ��������� ����������, ��������� ����� LSE
    RCC->CIER &= ~RCC_CIER_LSECSSIE;
    
    // �������� ������� � ������� �� ���������
    event_raise(&clk_lse_fail_event);
}

// ���������� ���������� ������������ LSE 
void clk_lse_ready_isr(void)
{
    // ����� ����� ������������ LSE
    RCC->CICR |= RCC_CICR_LSERDYC;
    
    // �������� ������� � ������� �� ���������
    event_raise(&clk_lse_ready_event);
}
