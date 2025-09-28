#include "button.h"
#include "nvic.h"
#include "event.h"

// ����� ����� ���������� (��� ������ "1")
#define BUTTON_ISR_FLAG_CLEAR(line)     (EXTI->PR1 |= EXTI_PR1_PIF##line)

// ���� ������� ��� ��������� �������� ��������� (10 ����)
// TODO: ��������� ���������� ����� ���������
#define BUTTON_CONTACT_BOUNCE_TIME      TIMER_TICKS_MS(10)

// ����������� ������������� ������
#define BUTTON_STATIC_INIT(mode, cb)                                            \
{                                                                               \
    TIMER_STATIC_INIT(mode, cb),                                                \
    .pressed = false                                                            \
}

    /*** ����������� ������� ������� ������ ***/
/* ���������� ��� callback � ������� � ������,  *
 * ���� ������ �������� ��������� ���������     */
static void button_0_pressed_event_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    button_t *button = (button_t *)timer;
    
    // ���� ������ ���� �� � ������� ���������
    if (!button->pressed)
    {
        // ��������� ����� ������� ������
        button->pressed = true; 
        
        // TODO: �������� ������� �� ������� ������
        //event_raise();
    }
    
    // ������ ���� � ������� ���������
    else
        button->pressed = false;
}

static void button_1_pressed_event_cb(timer_t *timer)
{
    
}

static void button_2_pressed_event_cb(timer_t *timer)
{
    
}

static void button_3_pressed_event_cb(timer_t *timer)
{
    
}

// ������
button_t button_0 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_0_pressed_event_cb);
button_t button_1 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_1_pressed_event_cb);
button_t button_2 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_2_pressed_event_cb);
button_t button_3 = BUTTON_STATIC_INIT(TIMER_MODE_ONE_SHOT, button_3_pressed_event_cb);

void button_init(void)
{
    /* ��������� ���������� ������ */
    
    // ������ �� ���������� � ����� EXTI 0, 1, 2, 4
    EXTI->IMR1 = EXTI_IMR1_IM0 | EXTI_IMR1_IM1 | EXTI_IMR1_IM2 | EXTI_IMR1_IM3;
    
    // ��������� ���������� �� ������ �������
    EXTI->RTSR1 = EXTI_RTSR1_RT0 | EXTI_RTSR1_RT1 | EXTI_RTSR1_RT2 | EXTI_RTSR1_RT3;
    // ��������� ���������� �� ����� ������� 
    EXTI->FTSR1 = EXTI_FTSR1_FT0 | EXTI_FTSR1_FT1 | EXTI_FTSR1_FT2 | EXTI_FTSR1_FT3;
    
    // �������� ���������
    nvic_irq_enable(EXTI0_IRQn);
    nvic_irq_enable(EXTI1_IRQn);
    nvic_irq_enable(EXTI2_IRQn);
    nvic_irq_enable(EXTI3_IRQn);
}

void button_0_iqr(void)
{
    // �������� ���� ����������
    BUTTON_ISR_FLAG_CLEAR(0);
    
    // ������ ������� ��� ��������� �������� ���������
    timer_start(&button_0.timer, BUTTON_CONTACT_BOUNCE_TIME);
}

void button_1_iqr(void)
{
    BUTTON_ISR_FLAG_CLEAR(2);
}

void button_2_iqr(void)
{
    BUTTON_ISR_FLAG_CLEAR(3);
}

void button_3_iqr(void)
{
    BUTTON_ISR_FLAG_CLEAR(4);
}
