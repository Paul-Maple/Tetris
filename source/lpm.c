#include "lpm.h"
#include "mcu.h"

// ������������� ������ ����������������
void lpm_init(void)
{   
    // �������� ������������ �������� �������
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
    
    // ��������� ������ Stop2
    PWR->CR1 |= PWR_CR1_LPMS_STOP2;
    
    // ��������� ������� � WFI (���./����. ��������� FCLK � HCLK) 
    DBGMCU->CR |= DBGMCU_CR_DBG_STOP;           // Debug on                  
    //DBGMCU->CR &= ~DBGMCU_CR_DBG_STOP;        // Debug off
    
    // �������� ����� ��������� ���
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
}
