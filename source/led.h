#ifndef __LED_H
#define __LED_H

#include "timer.h"

// ������������� ������
void led_init(void);

// ���������� ������������ ��������� ����������
void led_state_switch(timer_t *timer);

#endif // __LED_H
