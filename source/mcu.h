#ifndef __MCU_H
#define __MCU_H

/* ���������:        *
 * LPTIM1 �� LSI/LSE */

// �����������
#include <typedefs.h>
// ��� STM32L433
#include <stm32l443xx.h>
// ��� ����������
#include "intrinsics.h"

    /*** ������� ***/
// ������� �������� ������
#define MCU_LSE_FREQ        32768

// ������� ��� � ���� ������������ ������� ������������ 
#define MCU_US_TO_TIC(us, f)         ((uint32_t)((float64_t)(us) * (f) / 1.0e6 + 0.5))

    /*** �� ������� ***/
#define ever        __enable_interrupt();;

    /*** WFI ***/
// ���� � WFI
#define MCU_WFI_ENTER()         __WFI()

    /*** ���������� ***/
// ���������/���������� ����������
#define MCU_IQR_ENABLE()        __enable_interrupt()
#define MCU_IQR_DISABLE()       __disable_interrupt()

// ����������/����������� �������� ��������� ����������
#define MCU_IQR_SAVE()          __istate_t state = __get_interrupt_state()
#define MCU_IQR_RESTORE()       __set_interrupt_state(state)

// ����/����� � ���������� �� ���������� ������� ����
#define MCU_IQR_SAFE_LEAVE()        MCU_IQR_RESTORE()
#define MCU_IQR_SAFE_ENTER()                                                    \
    MCU_IQR_SAVE();                                                             \
    MCU_IQR_DISABLE()

    /*** ������������� ������ ***/
void mcu_init(void);

#endif // __MCU_H
