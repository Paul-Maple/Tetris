#include "spi.h"
#include "mcu.h"
#include "nvic.h"

void spi_enable(void)
{
    // Включить тактирование SPI1
    RCC->APB2ENR = RCC_APB2ENR_SPI1EN; 
    
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_BR_0 |          // Делитель частоты /4
                SPI_CR1_MSTR |          // Режим мастера
                SPI_CR1_SPE;            // Включить SPI
    
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 |     // Передача по 8 бит
                SPI_CR2_SSOE;                                    // Без режима мультимастер
}

void spi_disable(void)
{
    // Отключение spi и его тактирования
    SPI1->CR1 &= ~SPI_CR1_SPE;
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
}

void spi_transmit(const uint8_t *data)
{
    ASSERT_NULL_PTR(data);
    
    //Ждем, пока не освободится буфер передатчика
    while(!(SPI1->SR & SPI_SR_TXE))
        { }
    // TODO: Переделать эту хуйню с пустым ожиданием (Она временная)
    
    // Запись в регистр данных
    SPI1->DR = (*data);
}
