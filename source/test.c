#include "test.h"
#include <lcd.h>

void test_init(void)
{
    // Установка координат
    const lcd_position_t position = 
    {
        .x1 = 120,
        .x2 = 121,
        .y1 = 160,
        .y2 = 161
    };
    
    // Красный пиксель посередине экрана
    lcd_image_set(&position, LCD_COLOR_GREEN); 
}

void test_func(void)
{
    
}
