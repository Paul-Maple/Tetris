#include "lpm.h"
#include "mcu.h"

// Инициализация режима энергосбережения
void lpm_init(void)
{   
    // Включить тактирование контроля питания
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
    
    // Включение режима Stop2
    PWR->CR1 |= PWR_CR1_LPMS_STOP2;
    
    // Настройка отладки в WFI (Вкл./Выкл. источники FCLK и HCLK) 
    DBGMCU->CR |= DBGMCU_CR_DBG_STOP;           // Debug on                  
    //DBGMCU->CR &= ~DBGMCU_CR_DBG_STOP;        // Debug off
    
    // Включить режим глубокого сна
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
}
