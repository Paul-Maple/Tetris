#ifndef __LCD_H
#define __LCD_H

#include <typedefs.h>

/*** Разрешение дисплея 240x320 ***/
#define ILI9341

#ifdef ILI9341
    #define LCD_WIDTH       240
    #define LCD_HEIGHT      320
#endif // ILI9341

// Перечисление цветов (Формат: 16 бит)
enum
{
    // Ахроматические цвета
    LCD_COLOR_WHITE  = 0xFFFF,
    LCD_COLOR_BLACK  = 0x0000,
    LCD_COLOR_GRAY   = 0x8410,
    // Монохромные цвета
    LCD_COLOR_RED    = 0xF800,
    LCD_COLOR_GREEN  = 0x07E0,
    LCD_COLOR_BLUE   = 0x001F,
    LCD_COLOR_YELLOW = 0xFFE0,
    LCD_COLOR_ORANGE = 0xFC00,
    LCD_COLOR_CYAN   = 0x07FF,
    LCD_COLOR_PURPLE = 0xBB59,
    LCD_COLOR_PINK   = 0xF810,
};

// Цвет
typedef uint16_t lcd_color_t;

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
