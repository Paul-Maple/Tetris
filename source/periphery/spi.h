#ifndef __SPI_H
#define __SPI_H

#include <typedefs.h>

// Передача 8 бит данных
void spi_transmit(const uint8_t data);

// Передача 16-битного цвета
void spi_transmit_color(const uint16_t color, const uint32_t size);

// Получение 16 бит данных с предварительной отправкой команды чтения
/* Отдельно отправлять команду на чтение и читать данные нельзя
   поскольку chip select должен быть в "0" непрерывно между 
   командой и чтением данных из памяти дисплея*/
void spi_receive(const uint8_t cmd, uint16_t *data, uint16_t size);

#endif // __SPI_H
