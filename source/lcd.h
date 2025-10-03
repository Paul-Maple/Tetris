#ifndef __LCD_H
#define __LCD_H

#include <timer.h>

// Перечисление команд управления дисплеем
enum
{    
    LCD_CMD_SOFT_RESET      = 0x01,     // Программный сброс
    LCD_CMD_SLEEP_OUT       = 0x11,     // Выход из режима сна       
    LCD_CMD_NORMAL_MODE     = 0x13,     // Нормальный режим отображения
    LCD_CMD_MAC_SET         = 0x36,     // Настройка доступа к памяти
    LCD_CMD_DISPLAY_ON      = 0x29,     // Включение дисплея
    LCD_CMD_DISPLAY_OFF     = 0x28,     // Выключение дисплея
    LCD_CMD_COLLUM_SET      = 0x2A,     // Установка адреса столбца
    LCD_CMD_LINE_SET        = 0x2B,     // Установка адреса строки    
    LCD_CMD_MEMORY_SET      = 0x2C,     // Передача данных от МК до кадровой памяти
    LCD_PIXEL_FORMAT_SET    = 0x3A,     // Установка формата RGB
    LCD_GAMMA_SET           = 0x26,     // Установка гаммы
    LCD_CMD_COLOR_SET       = 0x2D,     // Установка цвета
    
    LCD_CMD_NOP             = 0x00      // Пустая команда (Для завершения передачи)
    // TODO: Добавить необходимые команды
};

// Перечисление цветов (Формат: 16 бит)
enum
{
    LCD_COLOR_RED = 0xF800, 
    // TODO: Дописать цвета
};

// Перечисление состояний пина DCRS 
enum
{
    LCD_CMD  = 0,    // Команда
    LCD_DATA = 1     // Данные
};
/*
// Перечисление тиков таймера для задержки отправки команд
enum
{
    LCD_TIC_RESET     = TIMER_TICKS_MS(5),
    LCD_TIC_SLEEP_OUT = TIMER_TICKS_MS(5)
};  */

// Структура элемента цепочки команд
typedef struct
{
    // Указатель на данные
    const void *data;
    // Размер данных (в байтах)
    uint8_t size;
    // Команда
    uint8_t cmd;
    
} lcd_chain_cmd_t;

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
