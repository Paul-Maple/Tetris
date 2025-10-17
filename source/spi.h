#ifndef __SPI_H
#define __SPI_H

#include <typedefs.h>

// Включение SPI
void spi_enable(void);

// Отключение SPI
void spi_disable(void);

// Передача одного байта данных
void spi_transmit(const uint8_t *data, const uint8_t size);

#endif // __SPI_H
