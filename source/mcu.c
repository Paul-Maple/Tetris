#include "mcu.h"

// ������������� ������
void mcu_init(void)
{
        /* ������������ */
    // �������� ������������ ������� � �������� ������� (��� ������� � ������ � ������� RCC_BDCR)
    // RCC_BDCR ������� �� ������ � ������� �������������� �������������
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN | RCC_APB1ENR1_LPTIM1EN;
     
    // �������� ������������ GPIO
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN |     // ����� �, �
                    RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOHEN;      // ����� �, �
}
