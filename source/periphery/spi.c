#include "spi.h"
#include <nvic.h>
#include <io.h>

/*** Частота CLK = 10 MHz ***/

// Макросы для настрйки регистра DS для передачи 8 или 16 бит
#define SPI_DS_6_BIT        (SPI_CR2_DS_0 | SPI_CR2_DS_2)
#define SPI_DS_8_BIT        (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2)
#define SPI_DS_16_BIT       (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3)

// Подготовка SPI к передаче
static void spi_preparing_tx(uint32_t SPI_DS)
{
    // Включить тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    /* Регистры CR1 и CR2 должны быть настроены перед включением SPI */
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_MSTR;                                                 // Режим мастера

    
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
    SPI1->CR1 = SPI_CR1_MSTR;                                                   // Режим мастера
    
    SPI1->CR2 = SPI_DS |                                                        // Количество бит для передачи
                SPI_CR2_SSOE |                                                  // Вывод NSS включен и управляется SPI аппаратно
                SPI_CR2_FRXTH;                                                  // Порог Rx FIFO 8 bit
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

void spi_transmit_color(const uint16_t color, const uint32_t size)
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

void spi_receive(const uint8_t cmd, uint16_t *color, uint16_t size)
{
    // Включить SPI
    spi_enable_rx(SPI_DS_8_BIT);
    
    // Чтение 4 байт из Rx буффера что бы он гарантированно был пуст перед чтением полезных данных
    uint8_t dummy_readed = *(( __IO uint8_t * ) &SPI1->DR);
    dummy_readed = *(( __IO uint8_t * ) &SPI1->DR);
    dummy_readed = *(( __IO uint8_t * ) &SPI1->DR);
    dummy_readed = *(( __IO uint8_t * ) &SPI1->DR);
    
        /* Передача команды чтения*/
    // Ожидание освобождения буфера передатчика
    while (!(SPI1->SR & SPI_SR_TXE));
    // Запись команды в регистр данных (8-ми битный доступ)
    *(( __IO uint8_t * ) &SPI1->DR) = cmd;
    // Ожидание окончания передачи
    while (SPI1->SR & SPI_SR_BSY);
    
    // Ожидание заполнения 8 бит в буффере приёмника
    while (!(SPI1->SR & SPI_SR_RXNE));
    // Чтение пустого байта
    dummy_readed = *(( __IO uint8_t * ) &SPI1->DR);
    
        /* Чтение данных */
    // Ожидание освобождения буфера передатчика
    while (!(SPI1->SR & SPI_SR_TXE));
    // Dummy transmit для генерации сигнала CLK
    *(( __IO uint8_t * ) &SPI1->DR) = 0x0;
    // Ожидание окончания передачи
    while (SPI1->SR & SPI_SR_BSY);
    // Ожидание заполнения 8 бит в буффере приёмника
    while (!(SPI1->SR & SPI_SR_RXNE));
    // Dummy read для первого байта
    dummy_readed = *(( __IO uint8_t * ) &SPI1->DR);
    
    // Ожидание освобождения буфера передатчика
    while (!(SPI1->SR & SPI_SR_TXE));
    // Dummy transmit для генерации сигнала CLK
    *(( __IO uint8_t * ) &SPI1->DR) = 0x0;
    
    
    for (uint16_t i = 0; i < size; i++)
    {
        // Обнуляем переменную цвета текущего пикселя для чтения
        color[i] = 0;
        
        // Преобразование 3-х байт в 16-ти битный цвет
        for (uint8_t j = 0; j < 3; j++)
        {
            // Ожидание заполнения 8 бит в буффере приёмника и окончания передачи
            while ((!(SPI1->SR & SPI_SR_RXNE)) && (SPI1->SR & SPI_SR_BSY));
            // Чтение байта
            uint8_t readed = *(( __IO uint8_t * ) &SPI1->DR);
            // Ожидание освобождения буфера передатчика
            while (!(SPI1->SR & SPI_SR_TXE));
            // Dummy transmit for generate CLK
            *(( __IO uint8_t * ) &SPI1->DR) = 0x0;
            
            switch (j)
            {
                case 0:
                    readed &= 0b11111000;
                    color[i] |= readed << 8;
                    break;
                    
                case 1:
                    readed &= 0b11111100;
                    color[i] |= readed << 3;
                    break;
                    
                case 2:
                    readed &= 0b11111000;
                    readed = readed >> 3;
                    color[i] |= readed << 0;
                    break;
            }
        }
    }
    
    // Ожидание окончания передачи фиктивных данных
    while (SPI1->SR & SPI_SR_BSY);
    
    // Отключить SPI
    spi_disable();
}
