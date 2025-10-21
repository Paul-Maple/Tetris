#include "spi.h"
#include <mcu.h>
#include <nvic.h>
#include "io.h"

// Подготовка SPI к передаче
static void spi_preparing(void)
{
    // Включить тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    /* Регистры CR1 и CR2 должны быть настроены перед включением SPI */
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2 |           // Делитель частоты на 256
                SPI_CR1_MSTR |                                         // Режим мастера
                SPI_CR1_CPHA;//                                         // Фаза сигнала
                //SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;// |                // Однонапрвленная передача по 1 проводу + Tx-only
                //SPI_CR1_SSM | SPI_CR1_SSI;                         // Программное управление NSS + NSS state "1"
    
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3 |   // Передача по 16 бит
                SPI_CR2_SSOE |                                  // Вывод NSS включен и управляется SPI аппаратно
                SPI_CR2_NSSP;                                   // Вывод NSS "1" между передачами
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
    // Отключение шины SPI и тактирования SPI
    SPI1->CR1 &= ~SPI_CR1_SPE;
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
}

void spi_transmit(const uint16_t *data)
{
    ASSERT_NULL_PTR(data);
    
    // Включить SPI
    spi_enable();
    
    // Ожидание освобождения буфера передатчика
    while (!(SPI1->SR & SPI_SR_TXE));
    
    // Запись в регистр данных
    SPI1->DR = *(data);
    
    // NSS low
    io_nss_set(false);
    
    // Ожидание окончания передачи
    while (SPI1->SR & SPI_SR_BSY);
    
    // NSS high
    io_nss_set(true);
    
    // Отключить SPI
    spi_disable();
}
