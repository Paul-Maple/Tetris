#ifndef __BUTTON_H
#define __BUTTON_H

#include "timer.h"

// ��������� ������
typedef struct
{
    timer_t timer;      // ������
    bool pressed;       // ���� ������� ������
    
} button_t;

// ������������� ������
void button_init(void);

// ����������� ���������� ������
void button_0_iqr(void);

void button_1_iqr(void);

void button_2_iqr(void);

void button_3_iqr(void);

#endif // __BUTTON_H
