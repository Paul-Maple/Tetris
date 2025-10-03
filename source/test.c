#include "test.h"
#include "lcd.h"

void test_init(void)
{
    lcd_position_t pos = 
    {
        .x[0] = 120,
        .x[1] = 121,
        .y[0] = 160,
        .y[1] = 161
    };
    
    //lcd_image_set(&pos, LCD_COLOR_RED);
}
