#ifndef __NVIC_H
#define __NVIC_H

#include "mcu.h"

// ¬ключает прерывание 
inline void nvic_irq_enable(IRQn_Type irq)
{
    assert(irq >= 0);
    NVIC_EnableIRQ(irq);
}

#endif // __NVIC_H
