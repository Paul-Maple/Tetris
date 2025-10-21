// Для включения прерываний глобально
#include "mcu.h"
// Для инициализации модулей
#include <io.h>
#include <clk.h>
#include "timer.h"
#include <button.h>
#include <lcd.h>
#include <led.h>
#include <lpm.h>
#include "event.h"

//#include "test.h"

__noreturn void main(void)
{
    // Порядок инициализации модулей не менять !!!!!!!
    mcu_init();
    io_init();
    clk_init();
    timer_module_init();
    button_init();
    lcd_init();
    led_init();
    lpm_init();
    
    //test_init();
    
    // Главный цикл
    for(MCU_IQR_ENABLE();;)
        event_process();  
    /***  while (1);  ***/    
}
