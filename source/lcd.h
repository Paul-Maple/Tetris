#ifndef __LCD_H
#define __LCD_H

#include <typedefs.h>

// Структура команды
typedef struct
{
    // Команда
    uint8_t cmd;
    // Указатель на данные
    const void *data;
    // Размер данных (В байтах)
    uint8_t size;
    
} lcd_cmd_t;

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
void lcd_image_set(const lcd_position_t *pos, const uint16_t color);

#endif // __LCD_H
