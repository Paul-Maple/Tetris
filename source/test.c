#include "test.h"
#include <lcd.h>

void test_init(void)
{
    // Красный пиксель посередине экрана
    const lcd_position_t position = 
    {
        .x1 = 120,
        .x2 = 121,
        .y1 = 160,
        .y2 = 161
    };
    
    lcd_image_set(&position, LCD_COLOR_RED); 
}

void test_func(void)
{
    
}
