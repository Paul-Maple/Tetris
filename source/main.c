// Для включения прерываний глобально
#include <mcu.h>
// Для инициализации модулей
#include "io.h"
#include "clk.h"
#include "timer.h"
#include "key.h"
#include "rng.h"
#include "lcd.h"
#include "led.h"
#include "lpm.h"
#include "event.h"
//#include "test.h"

__noreturn void main(void)
{
    // Запретить прерывания перед инициализацией модулей
    MCU_IQR_DISABLE();
    
    // Порядок инициализации модулей не менять !!!!!!!
    mcu_init();
    io_init();
    clk_init();
    timer_module_init();
    key_init();
    //rng_init();
    lcd_init();
    led_init();
    /*  В этом проекте Stop 2 Mode включать нельзя    *
     *  т.к. при выходе из WFI системная частота      *
     *  выставленная при инициализации на PLL 80 МГц  *
     *  сбрасывается в дефолтное значение 4 МГц       */
    //lpm_init();
    
    // Главный цикл
    for (MCU_IQR_ENABLE();;)
        event_process();    
}
