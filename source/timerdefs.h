#ifndef __TIMERDEFS_H
#define __TIMERDEFS_H

#include "mcu.h"

/* Тут макросы для параметров таймеров */

    /*** Для таймера LPTIM1 ***/
// Делитель для записи в регистр CFGR
#define LPTIM1_PRESC_128       (LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2)

// Ограничение счёта 
#define LPTIM1_ARR      128

    /*** Для расчёта тиков таймера ***/
// Частота тактирования аппаратного таймера [Гц]
#define TIMER_CLK       MCU_LSE_FREQ

// Перевод мкСек в тики 
#define TIMER_TICKS_US(us)               ((uint32_t)MCU_US_TO_TIC(us, TIMER_CLK))
// Перевод мСек в тики 
#define TIMER_TICKS_MS(ms)               TIMER_TICKS_US((ms) * 1000.0)
// Перевод Сек в тики
#define TIMER_TICKS_SEC(sec)              TIMER_TICKS_MS((sec) * 1000.0)

#endif // __TIMERDEFS_H
