#include "led.h"
#include "mcu.h"

/* LED ������������ ��� �������� ������ ������������ ������� */

// ����������� ������ LED_LPTIM1
static timer_t led_timer;

// ������������� ������
void led_init(void)
{
    // ������������� ������������ ������� LED_LPTIM1
    timer_init(&led_timer, TIMER_MODE_CONTINUOUS, led_state_switch);
    // ������ ������������ ������� 
    timer_start(&led_timer, TIMER_TICKS_MS(500));
    // ������������� ��������� ������ ���������� ( ���.)
    GPIOB->ODR |= GPIO_ODR_OD13;
}

// ���������� ������������ ��������� ����������
void led_state_switch(timer_t *timer)
{
    // ������������ ��������� ����������
    GPIOB->ODR ^= GPIO_ODR_OD13;
}
