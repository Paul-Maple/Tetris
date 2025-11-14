#ifndef __RNG_h
#define __RNG_h

#include <mcu.h>

// Инициализация модуля
void rng_init(void);

// Получение случайного числа в диапазоне от 0 до max_number
uint8_t rng_get_number(uint8_t max_number);

#endif // __RNG_h
