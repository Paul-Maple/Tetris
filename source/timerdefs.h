#ifndef __TIMERDEFS_H
#define __TIMERDEFS_H

#include "mcu.h"

/* ��� ������� ��� ���������� �������� */

    /*** ��� ������� LPTIM1 ***/
// �������� ��� ������ � ������� CFGR
#define LPTIM1_PRESC_128       (LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2)

// ����������� ����� 
#define LPTIM1_ARR      128

    /*** ��� ������� ����� ������� ***/
// ������� ������������ ����������� ������� [��]
#define TIMER_CLK       MCU_LSE_FREQ

// ������� ����� � ���� 
#define TIMER_TICKS_US(us)               ((uint32_t)MCU_US_TO_TIC(us, TIMER_CLK))
// ������� ���� � ���� 
#define TIMER_TICKS_MS(ms)               TIMER_TICKS_US((ms) * 1000.0)
// ������� ��� � ����
#define TIMER_TICKS_SEC(sec)              TIMER_TICKS_MS((sec) * 1000.0)

#endif // __TIMERDEFS_H
