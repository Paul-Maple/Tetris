#include "test.h"
#include <lcd.h>

void test_init(void)
{
    // Красный пиксель посередине экрана
    const lcd_position_t position = 
    {
        .x[0] = 120,
        .x[1] = 121,
        .y[0] = 160,
        .y[1] = 161
    };
    
    lcd_image_set(&position, LCD_COLOR_RED); 
}

void test_func(void)
{
    
}
