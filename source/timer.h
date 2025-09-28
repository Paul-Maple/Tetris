#ifndef __TIMER_H
#define __TIMER_H

#include <list.h>
#include "mcu.h"

// ��������� ����� �������
typedef uint32_t timer_interval_t;

// ������� ������������ ����������� ������� [��]
#define TIMER_CLK       MCU_LSE_FREQ

// ������� ����� � ���� 
#define TIMER_TICKS_US(us)      ((timer_interval_t)MCU_US_TO_TIC(us, TIMER_CLK))
// ������� ���� � ���� 
#define TIMER_TICKS_MS(ms)      TIMER_TICKS_US((ms) * 1000.0)
// ������� ��� � ����
#define TIMER_TICKS_SEC(sec)    TIMER_TICKS_MS((sec) * 1000.0)

// ����������� ������������� �������
#define TIMER_STATIC_INIT(_mode, cb)                                            \
{                                                                               \
    LIST_ITEM_STATIC_INIT,                                                      \
    .init.mode = _mode,                                                         \
    .init.handler = cb,                                                         \
    .data.reload = 0,                                                           \
    .data.rasied = false                                                        \
}

// ������������� ������ (��������������� ���������� ������� � �������)
void timer_module_init(void);

// ���������� ������ lptim1 � ����� ��������� ������������
void timer_clk_notice(void);

// ��������������� ���������� ��������� �������
typedef struct timer_s timer_t;

// �������� ����������� �������
typedef void (*timer_handler_ptr) (timer_t *timer);

// ������������ ������� ������ �������
typedef enum
{
    // ����������� �����
    TIMER_MODE_CONTINUOUS,
    // ��������� �����
    TIMER_MODE_ONE_SHOT
} timer_mode_t;

// ��������� �������
typedef struct timer_s
{
    // ������� ������ 
    list_item_t item;
    
    // ������ ������������� 
    struct
    {
        // ����� ������ 
        timer_mode_t mode;
        // ���������� ������� �������
        timer_handler_ptr handler;
    } init;
    
    // ������ ��� ��������� 
    struct
    {
        // �������� �� ������������ 
        uint32_t reload;
        // �������� �� ������������
        uint32_t remain;
        // ���� ������������ ������� 
        bool rasied;
    } data;
    
} timer_s;

// ������������� ������������ �������
/* ��������� ���� ��� ��� ������� ������������ ������� � ��� ������, ��� �� ������������ */
void timer_init(timer_t *timer, timer_mode_t mode, timer_handler_ptr handler);

// ������ ������������ �������
void timer_start(timer_t *timer, uint32_t ticks);

// ��������� ������������ �������
void timer_stop(timer_t *timer);

// ��������� ����������� ��������
void timer_processing_raised(void);

// ���������� ���������� ����������� ������� LPTIM1
void timer_lptim1_isr(void);

#endif // __TIMER_H
