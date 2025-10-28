#include "test.h"
#include <lcd.h>

void test_init(void)
{
    
}

void test_1(void)
{
    // Установка координат
    const lcd_position_t position = 
    {
        .x1 = 10,
        .x2 = 160,
        .y1 = 10,
        .y2 = 260
    };
    
    lcd_draw_image(&position, LCD_COLOR_RED);     
}

void test_2(void)
{
    // Установка координат
    const lcd_position_t position = 
    {
        .x1 = 20,
        .x2 = 120,
        .y1 = 20,
        .y2 = 80
    };
    
    lcd_draw_image(&position, LCD_COLOR_GREEN); 
}

void test_3(void)
{
    // Установка координат
    const lcd_position_t position = 
    {
        .x1 = 30,
        .x2 = 50,
        .y1 = 30,
        .y2 = 50
    };
    
    lcd_draw_image(&position, LCD_COLOR_BLUE); 
}