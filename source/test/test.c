#include "test.h"
#include "lcd.h"

void test_init(void)
{
    lcd_position_t pos = {50,60,50,60};
    lcd_image_set(&pos, LCD_COLOR_RED);
}
