// TODO: ������� ��������� ��������� ����� (� ��������� _t)
// ��� ��������� ���������� ���������
#include "mcu.h"
// ��� ������������� �������
#include "io.h"
#include "clk.h"
#include "spi.h"
#include "timer.h"
#include "button.h"
#include "lcd.h"
#include "led.h"
#include "lpm.h"
#include "event.h"

__noreturn void main(void)
{
    // ������� ������������� ������� �� ������ !!!!!!!
    mcu_init();
    io_init();
    clk_init();  
    timer_module_init();
    button_init();
    lcd_init();
    led_init();
    lpm_init();
    
    // ������� ����
    for(MCU_IQR_ENABLE();;)
        event_process();
}
