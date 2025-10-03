#include "spi.h"
#include <mcu.h>
#include <nvic.h>

// Подготовка SPI к передаче
static void spi_preparing(void)
{
    // Включить тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    /* Регистры CR1 и CR2 должны быть настроены перед включением SPI */
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_BR_0 | SPI_CR1_BR_2 |   // Делитель частоты /64
                SPI_CR1_MSTR;                   // Режим мастера                 

    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 |     // Передача по 8 бит
                SPI_CR2_NSSP | SPI_CR2_SSOE;                     // NSS pulse and SS output enable
}

void spi_enable(void)
{
    // Подготовка тактирования и настройка шины SPI
    spi_preparing();
    
    // Включить SPI
    SPI1->CR1 |= SPI_CR1_SPE; 
}

void spi_disable(void)
{
    // Отключение spi и его тактирования
    SPI1->CR1 &= ~SPI_CR1_SPE;
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
}

void spi_transmit(const uint8_t *data)
{
    //ASSERT_NULL_PTR(data);
    // Запись в регистр данных
    SPI1->DR = (*data);
}
