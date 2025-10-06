#include "lcd.h"
#include <lcd_reg.h>
#include "spi.h"
#include "io.h"

// Статическая инициализация элемента цепочки команд
#define LCD_CMD_STATIC_INIT(_data, _size, _cmd)                                 \
{                                                                               \
    .item = LIST_ITEM_STATIC_INIT(),                                            \
    .data = _data,                                                              \
    .size = _size,                                                              \
    .cmd  = _cmd                                                                \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(data, cmd)                                                 \
    LCD_CMD_STATIC_INIT((void*)(data), sizeof(data), cmd)

// Формат пикселя 5 + 6 + 5 bit RGB
static const uint8_t lcd_pixel_format = 0b01010101;
// Memory Access Control register
static const uint8_t lcd_mac_reg = 0b00000000;

// Список команд для отправки
list_t lcd_cmd_list;

// Предварительное объявление callback функции таймера
static void lcd_delay_cmd_tx(timer_t *timer);

// Таймер для задержки отправки команд
static timer_t lcd_delay_cmd_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, lcd_delay_cmd_tx);

// Отправка цепочки команд по SPI
static void lcd_chain_cmd_tx(const lcd_chain_cmd_t *chain)
{
    ASSERT_NULL_PTR(chain);
    
    // Включить SPI
    spi_enable();
    
    // Цикл, пока не дойдёт до команды завершения передачи цепочки
    for ( ; chain->cmd != LCD_CMD_NOP; chain++)
    {
        // Установить вывод DCRS в "0" для отправки команды
        io_dcrs_set(LCD_CMD);
        
        // Отправка команды
        spi_transmit(&chain->cmd);
        
        // Если команда имеет параметры
        if (chain->size > 0)
        {
            // Установить вывод DCRS в "1" для отправки данных
            io_dcrs_set(LCD_DATA);
            
            // Буфер для данных (MSB)
            uint8_t *buffer_data = (uint8_t *)chain->data;
            // Запись данных в буфер для отправки
            for (uint8_t i = 0; i < chain->size; i++)
                // Передача
                spi_transmit(&buffer_data[i]);
        }
        // Если команды с задержкой
        /*if (chain->cmd == LCD_CMD_SOFT_RESET || chain->cmd == LCD_CMD_SLEEP_OUT)
        {
            timer_start(&lcd_delay_cmd_timer, LCD_TIME_DELAY_5MS);
            // Конец передачи текущей цепочки команд
            break;
        }*/
    }
    
    // Отключить SPI
    spi_disable();
}


// Отправка команд после задержки
/* Вызывается, когда отработает таймер для задержки отправки команд */
static void lcd_delay_cmd_tx(timer_t *timer)
{
    //lcd_chain_cmd_tx();
}

void lcd_init(void)
{
    // Включить подсветку дисплея
    io_led_on();
    
    // Массив элементов цепочки команд для инициализации
    static const lcd_chain_cmd_t chain_init[] =    
    {
        // Инициализация команд
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SOFT_RESET),       // Программный сброс
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SLEEP_OUT),        // Выход из режима энергосбережения
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_DISPLAY_ON),       // Включить дисплей
        LCD_CMD_INIT(&lcd_pixel_format, LCD_PIXEL_FORMAT_SET),  // Формат пикселя (16-bit)
        LCD_CMD_INIT(&lcd_mac_reg, LCD_CMD_MAC_SET),            // Memory Access Control (default state)

    };

    // Передача
    lcd_chain_cmd_tx(chain_init);
}
/*
void lcd_image_set(const lcd_position_t *position, const uint16_t color)
{
    // Массив элементов цепочки команд
    const lcd_chain_cmd_t chain_image[4] = 
    {
        LCD_CMD_INIT(position->x, LCD_CMD_COLLUM_SET),  // Установить адреса начальной и конечной столбца
        LCD_CMD_INIT(position->y, LCD_CMD_LINE_SET),    // Установить адреса началного и конечного строки
        LCD_CMD_INIT(color, LCD_CMD_MEMORY_SET),        // Команда записи цвета
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_NOP)       // Команда завершения цепочки команд        
    };
    
    // Передача
    lcd_chain_cmd_tx(chain_image);  
} */
