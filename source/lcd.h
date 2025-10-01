#ifndef __LCD_H
#define __LCD_H

#include "timer.h"

// Перечисление команд управления дисплеем
typedef enum
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
} lcd_cmd_t;

// Перечисление состояний пина DCRS 
enum
{
    LCD_CMD  = 0,    // Команда
    LCD_DATA = 1     // Данные
};

// Перечисление тиков таймера для задержки отправки команд
enum
{
    LCD_TIC_RESET     = TIMER_TICKS_MS(5),
    LCD_TIC_SLEEP_OUT = TIMER_TICKS_MS(5)
};

// Структура элемента цепочки команд
typedef struct
{
    // Интервал срабатывания таймера задержки команды
    timer_interval_t time;
    // Указатель на данные
    const void *data;
    // Размер данных (в байтах)
    uint8_t size;
    // Команда
    lcd_cmd_t cmd;
    
} lcd_chain_cmd_t;

// Структура изображения
typedef struct
{
    // Столбец начала и конца записи
    uint16_t collum_start;
    uint16_t collum_end;
    
    // Строка начала и конца записи
    uint16_t line_start;
    uint16_t line_end;
    
    // Данные для записи
    void *rgb_data;
    
} lcd_image_t;

// Инициализация дисплея
void lcd_init(void);

#endif // __LCD_H
