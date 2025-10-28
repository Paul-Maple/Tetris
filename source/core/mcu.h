#ifndef __MCU_H
#define __MCU_H

/* Переферия:        *
 * LPTIM1 на LSI/LSE *
 * SPI               */

// Стандартные
#include <typedefs.h>
// Для STM32L433
#include <stm32l443xx.h>
// Для прерываний
#include "intrinsics.h"

    /*** Частота ***/
// Частота внешнего кварца
#define MCU_LSE_FREQ        32768

// Перевод мкс в тики относительно частоты тактирования 
#define MCU_US_TO_TIC(us, f)         ((uint32_t)((float64_t)(us) * (f) / 1.0e6 + 0.5))

    /*** По приколу ***/
#define ever        __enable_interrupt();;

    /*** WFI ***/
// Вход в WFI
#define MCU_WFI_ENTER()         __WFI()

    /*** Прерывания ***/
// Включение/Отключение прерываний
#define MCU_IQR_ENABLE()        __enable_interrupt()
#define MCU_IQR_DISABLE()       __disable_interrupt()

// Сохранение/Возвращение текущего состояния прерываний
#define MCU_IQR_SAVE()          __istate_t state = __get_interrupt_state()
#define MCU_IQR_RESTORE()       __set_interrupt_state(state)

// Вход/Выход в безопасный от прерываний участок кода
#define MCU_IQR_SAFE_LEAVE()        MCU_IQR_RESTORE()
#define MCU_IQR_SAFE_ENTER()                                                    \
    MCU_IQR_SAVE();                                                             \
    MCU_IQR_DISABLE()

    /*** Инициализация модуля ***/
void mcu_init(void);

// Установить умножитель частоты
void mcu_set_pll(void);

// Сбросить умножитель частоты
void mcu_reset_pll(void);

#endif // __MCU_H
