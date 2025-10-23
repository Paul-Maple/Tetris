#include "spi.h"
#include <nvic.h>
#include "io.h"

// Подготовка SPI к передаче
static void spi_preparing()
{
    // Включить тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    /* Регистры CR1 и CR2 должны быть настроены перед включением SPI */
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_BR_1 | SPI_CR1_BR_2 |                                   // Делитель частоты на 128
                SPI_CR1_MSTR |                                                  // Режим мастера
                SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;// |                              // Simplex mode, Tx-Only
                //SPI_CR1_CPHA;                                                 // Фаза сигнала
    
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 |                    // Передача по 8 бит
                SPI_CR2_SSOE; |                                                  // Вывод NSS включен и управляется SPI аппаратно
}

static void spi_enable(void)
{
    // Подготовка тактирования и настройка шины SPI
    spi_preparing();
    
    // Включить SPI
    SPI1->CR1 |= SPI_CR1_SPE; 
}

static void spi_disable(void)
{
    // Отключение шины SPI и тактирования SPI
    SPI1->CR1 &= ~SPI_CR1_SPE;
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
}

void spi_transmit(const uint8_t data)
{
    // Включить SPI
    spi_enable();
    
    // Ожидание освобождения буфера передатчика
    while (!(SPI1->SR & SPI_SR_TXE));
    
    // Запись в регистр данных (8-ми битный доступ)
    /* Без приведения происходит передача дополнительных 8-ми лишних нулевых бит */
    *(( __IO uint8_t * ) &SPI1->DR) = data;
    // SPI1->DR = data;
    
    // NSS low
    io_nss_set(false);
    
    // Ожидание освобождения TxFIFO
    while (!(SPI_SR_FTLVL_0 | SPI_SR_FTLVL_1));
    // Ожидание окончания передачи
    while (SPI1->SR & SPI_SR_BSY);
    
    // NSS high
    io_nss_set(true);
    
    // Отключить SPI
    spi_disable();
}
