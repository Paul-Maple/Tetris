#ifndef __SPI_H
#define __SPI_H

#include <typedefs.h>

// Передача 8 бит данных
void spi_transmit(const uint8_t data);

// Передача 16-битного цвета
void spi_transmit_color(const uint16_t color, const uint32_t size);

// Получение 16 бит данных 
void spi_receive(uint16_t *data);

#endif // __SPI_H
