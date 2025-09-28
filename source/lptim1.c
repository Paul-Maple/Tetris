#include "lptim1.h"

#include "mcu.h"
#include "nvic.h"
#include "clk.h"

// LPTIM1 (����� 01)
#define LPTIM1_CLK_LSI       RCC_CCIPR_LPTIM1SEL_0
// LPTIM1 (����� 11)
#define LPTIM1_CLK_LSE       (RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1)       

// �������� �������� ������������ �������
static void lptim1_tim_clear_clk(void)
{
    RCC->CCIPR &= ~LPTIM1_CLK_LSE; 
}

// ����� LSI � �������� ��������� ������������ ��� ������� LPTIM1 (����� 01)
static void lptim1_tim_set_lsi(void)
{
    RCC->CCIPR |= LPTIM1_CLK_LSI;
} 

// ����� LSE � �������� ��������� ������������ ��� ������� LPTIM1 (����� 11)
static void lptim1_tim_set_lse(void)
{
    RCC->CCIPR |= LPTIM1_CLK_LSE;
} 

// ���������� ������ led � ����� ��������� ������������
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
    // ����� LSI � �������� ��������� ������������ ��� ������� LPTIM1 (����� 01)
    lptim1_clk_notice();
    
    // �������� �������� (= 128)
    // LPTIM1_CFGR ����� ���� ���������� ������ ����� ���������� LPTIM1_CR_ENABLE = 0
    LPTIM1->CFGR = LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2;
    
    // ��������� ���������� �� ���������� � ARR
    // LPTIM1_IER ����� ���� ���������� ������ ����� ���������� LPTIM1_CR_ENABLE = 0
    LPTIM1->IER = LPTIM_IER_ARRMIE;
    
    // ��������� ������
    LPTIM1->CR = LPTIM_CR_ENABLE;
    
    // ����������� ����� �� 128
    // LPTIM1_ARR  ����� ���� ���������� ������ ����� ���������� LPTIM1_CR_ENABLE = 1
    LPTIM1->ARR = 128 - 1;
    
    // ����� ������������ ����� �� �������� � �������� ARR
    // SNGSTRT ����� ���� ���������� ������ ����� ���������� LPTIM1_CR_ENABLE = 1
    LPTIM1->CR |= LPTIM_CR_CNTSTRT;
    
    // ��������� ����������
    nvic_irq_enable(LPTIM1_IRQn);
}
