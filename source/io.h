#ifndef __IO_H
#define __IO_H

#include <typedefs.h>

// Инициализация GPIO
void io_init(void);

// Аппаратный сброс дисплея
void io_lcd_hard_reset(void);

// Установка состояния пина DCRS
void io_dcrs_set(const bool state);

// Включить подсветку дисплея
void io_led_on(void);

#endif // __GPIO_H
