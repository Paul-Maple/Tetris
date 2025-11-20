#ifndef __IO_H
#define __IO_H

#include <typedefs.h>

// Инициализация GPIO
void io_init(void);

// Установка состояния пина RESX
void io_resx_set(void);

// Сброс состояния пина RESX
void io_resx_reset(void);

// Установка состояния пина DCRS
void io_dcrs_set(const bool state);

// Включить подсветку дисплея
void io_led_on(void);

#endif // __IO_H
