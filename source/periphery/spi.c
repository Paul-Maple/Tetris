#include "spi.h"
#include <nvic.h>
#include <io.h>

/*** Частота CLK = 15 MHz ***/

// Макросы для настрйки регистра DS для передачи 8 или 16 бит
#define SPI_DS_8_BIT        (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2)
#define SPI_DS_16_BIT       (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3)

// Подготовка SPI к передаче
static void spi_preparing_tx(uint32_t SPI_DS)
{
    // Включить тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    /* Регистры CR1 и CR2 должны быть настроены перед включением SPI */
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_MSTR |                                                  // Режим мастера
                SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;                              // Simplex mode, Tx-Only
    
    SPI1->CR2 = SPI_DS |                                                        // Количество бит для передачи
                SPI_CR2_SSOE;                                                   // Вывод NSS включен и управляется SPI аппаратно
}

// Подготовка SPI к приёму
static void spi_preparing_rx(uint32_t SPI_DS)
{
    // Включить тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    /* Регистры CR1 и CR2 должны быть настроены перед включением SPI */
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_RXONLY;
    
    SPI1->CR2 = SPI_DS | SPI_CR2_SSOE;
}

// Включение SPI для передачи
static void spi_enable_tx(uint32_t SPI_DS)
{
    // Подготовка тактирования и настройка шины SPI
    spi_preparing_tx(SPI_DS);
    
    // Включить SPI
    SPI1->CR1 |= SPI_CR1_SPE; 
}

// Включение SPI
static void spi_enable_rx(uint32_t SPI_DS)
{
    // Подготовка тактирования и настройка шины SPI
    spi_preparing_rx(SPI_DS);
    
    // Включить SPI
    SPI1->CR1 |= SPI_CR1_SPE; 
}

// Отключение SPI
static void spi_disable(void)
{
    // Отключение шины SPI и тактирования SPI
    SPI1->CR1 &= ~SPI_CR1_SPE;
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
}

void spi_transmit(const uint8_t data)
{
    // Включить SPI
    spi_enable_tx(SPI_DS_8_BIT);
    
    // Ожидание освобождения буфера передатчика
    while (!(SPI1->SR & SPI_SR_TXE));
    
    // Запись в регистр данных (8-ми битный доступ)
    /* Без приведения происходит передача дополнительных 8-ми лишних нулевых бит */
    *(( __IO uint8_t * ) &SPI1->DR) = data;
    
    // Ожидание окончания передачи
    while (SPI1->SR & SPI_SR_BSY);
    
    // Отключить SPI
    spi_disable();
}

void spi_transmit_color(const uint16_t color, uint32_t size)
{
    // Включить умножитель частоты
    mcu_set_pll();
    
    // Включить SPI
    spi_enable_tx(SPI_DS_16_BIT);
    
    for (uint32_t i = 0; i < size; i++)
    {
       // Ожидание освобождения буфера передатчика
        while (!(SPI1->SR & SPI_SR_TXE));
        // Запись в регистр данных
        SPI1->DR = color; 
    }
    
    // Ожидание окончания передачи
    while (SPI1->SR & SPI_SR_BSY);
    
    // Отключить SPI
    spi_disable();
    
    // Отключить умножитель частоты
    mcu_reset_pll();
}

void spi_receive(uint16_t *data)
{
    // Включить SPI
    spi_enable_rx(SPI_DS_16_BIT);
    
    // Dummy Read для первого параметра
    for (uint8_t i = 0; i < 2; i++)
    {
        // Ожидание заполнения 16 бит в буффере приёмника
        while (!(SPI1->SR & (SPI_SR_FRLVL_1 | SPI_SR_FRLVL_0)));
        // Чтение 16 бит
        *data = SPI1->DR;
    }
    // Сбростить OVR флаг
    
    // Отключить SPI
    spi_disable();
}
