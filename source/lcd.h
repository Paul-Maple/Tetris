#ifndef __LCD_H
#define __LCD_H

#include <typedefs.h>

// Структура элемента цепочки команд
typedef struct
{
    // Указатель на данные
    void *data;
    // Размер данных (В байтах)
    uint8_t size;
    // Команда
    uint8_t cmd;
    
} lcd_chain_cmd_t;

// Перечисление цветов (Формат: 16 бит)
enum
{
    LCD_COLOR_RED = 0xF800, 
    // TODO: Дописать цвета
};

// Структура координат записи
typedef struct
{
    // Координаты начала и конца записи
    uint16_t x[2];
    uint16_t y[2];
    
} lcd_position_t;

// Инициализация дисплея
void lcd_init(void);

// Установка изображения
void lcd_image_set(const lcd_position_t *pos, const uint16_t color);

#endif // __LCD_H
