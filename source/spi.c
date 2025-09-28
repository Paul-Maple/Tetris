#include "spi.h"
#include "mcu.h"
#include "nvic.h"

void spi_enable(void)
{
    // �������� ������������ SPI1
    RCC->APB2ENR = RCC_APB2ENR_SPI1EN; 
    
    // ��������� ��������� SPI
    SPI1->CR1 = SPI_CR1_BR_0 |          // �������� ������� /4
                SPI_CR1_MSTR |          // ����� �������
                SPI_CR1_SPE;            // �������� SPI
    
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 |     // �������� �� 8 ���
                SPI_CR2_SSOE;                                   // ��� ������ ������������
}

void spi_disable(void)
{
    // ���������� spi � ��� ������������
    SPI1->CR1 &= ~SPI_CR1_SPE;
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
}

void spi_transmit(uint8_t *data)
{
    ASSERT_NULL_PTR(data);
    // ������ � ������� ������
    SPI1->DR = (*data);
}
