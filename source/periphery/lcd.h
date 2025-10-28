#ifndef __LCD_H
#define __LCD_H

#include <typedefs.h>

// Перечисление цветов (Формат: 16 бит)
enum
{
    LCD_COLOR_WHITE = 0xFFFF,
    LCD_COLOR_BLACK = 0x0000,
    LCD_COLOR_RED   = 0xF800,
    LCD_COLOR_GREEN = 0x07E0,
    LCD_COLOR_BLUE  = 0x001F
    // TODO: Дописать цвета
};

// Структура координат записи
typedef struct
{
    // Координаты начала и конца записи
    uint16_t x1;
    uint16_t x2;
    
    uint16_t y1;
    uint16_t y2;
    
} lcd_position_t;

// Инициализация дисплея
void lcd_init(void);

// Установка изображения
void lcd_draw_image(const lcd_position_t position, const uint16_t color);

// Очистка дисплея (Заливка белым цветом)
void lcd_clear(void);

#endif // __LCD_H
