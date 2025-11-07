#include "rng.h"

void rng_init(void)
{
    // Включить генератор случайных чисел
    RNG->CR = RNG_CR_RNGEN;
}

uint8_t rng_get_number(uint8_t max_number)
{
    // Ждём пока регистр не будет доступен для чтения
    while (!(RNG->SR & RNG_SR_DRDY));
        // Получить число в диапазоне от 0 до max_number
        return (RNG->DR & 0xFF) % (max_number + 1);
}
