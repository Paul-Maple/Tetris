#include "spi.h"
#include <mcu.h>
#include <nvic.h>

// Подготовка SPI к передаче
static void spi_preparing()
{
    // Включить тактирование SPI1
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    
    /* Регистры CR1 и CR2 должны быть настроены перед включением SPI */
    // Настройка регистров SPI
    SPI1->CR1 = SPI_CR1_BR_0 | SPI_CR1_BR_1         // Делитель частоты /4
                SPI_CR1_MSTR |          // Режим мастера
                SPI_CR1_BIDIMODE |      // Simplex mode
                SPI_CR1_BIDIOE;         // Tx only
     
    SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 |     // Передача по 8 бит
                SPI_CR2_SSOE;                                    // Без режима мультимастер
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
    ASSERT_NULL_PTR(data);
	
	// Проверяем, что SPI включен и тактируется
    if (!(RCC->APB2ENR & RCC_APB2ENR_SPI1EN)) 
	{} // Обработка ошибки - SPI не тактируется

    if (!(SPI1->CR1 & SPI_CR1_SPE)) 
	{} // Обработка ошибки - SPI не включен

	// Ждем, пока буфер передачи не станет пустым
    while (!(SPI1->SR & SPI_SR_TXE));
    
    // Запись в регистр данных
    //SPI1->DR = (*data);

    while (SPI1->SR & SPI_SR_BSY) 
	{} // Ждем завершения передачи
	//Регистр SCB->CFSR - причина HardFault
	//Регистр SCB->HFSR - статус HardFault
	//Регистр SCB->MMFAR - адрес памяти, вызвавший fault
}
