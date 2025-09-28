#ifndef __SPI_H
#define __SPI_H

#include <typedefs.h>

// ��������� SPI
void spi_enable(void);

// ���������� SPI
void spi_disable(void);

// �������� ������ ����� ������
void spi_transmit(uint8_t *data);

#endif // __SPI_H
