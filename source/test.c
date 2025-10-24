#include "test.h"
#include <lcd.h>

void test_init(void)
{
    // Установка координат
    const lcd_position_t position = 
    {
        .x1 = 40,
        .x2 = 200,
        .y1 = 40,
        .y2 = 300
    };
    
    // Красный пиксель посередине экрана
    lcd_image_set(&position, LCD_COLOR_RED); 
}

void test_func(void)
{
    
}
