#include "lcd.h"
#include "lcd_reg.h"
#include "spi.h"
#include "io.h"

// Регистр MAC (Memory access control 8-ми битный)
// TODO: Записать в регистр необходимые значения бит
uint8_t lcd_mac_reg = 0b00000000;

// Статическая инициализация элемента цепочки команд
#define LCD_CMD_STATIC_INIT(mode, _time, cb, _data, _size, _cmd)                \
{                                                                               \
    .timer = TIMER_STATIC_INIT(mode, cb),                                       \
    .time  = _time,                                                             \
    .data  = _data,                                                             \
    .size  = _size,                                                             \
    .cmd   = _cmd                                                               \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(mode, _time, cb, data, cmd)                                \
    LCD_CMD_STATIC_INIT(mode, _time, cb, (const void*)(data), sizeof(data), cmd)

// Отправка цепочки команд по SPI
static void lcd_chain_cmd_tx(lcd_chain_cmd_t *chain)
{
    ASSERT_NULL_PTR(chain);
    
    // Включить SPI
    spi_enable();
    
    // Буфер для команд
    uint8_t buffer_cmd;
    // Цикл, пока не дойдёт до команды завершения передачи цепочки
    for ( ; chain->cmd != LCD_CMD_NOP; chain++)
    {
        // Установить вывод DCRS для отправки команды
        io_dcrs_set(LCD_CMD);
        
        // Отправка команды
        buffer_cmd = (uint8_t)chain->cmd;
        spi_transmit(&buffer_cmd);
        
        // Если команда имеет параметры
        if (chain->data != NULL)
        {
            // Установить вывод DCRS для отправки данных
            io_dcrs_set(LCD_DATA);
            
            // Буфер для данных
            uint8_t *buffer_data = (uint8_t *)chain->data;
            // Запись данных в буфер для отправки
            for (uint8_t i = 0; i < chain->size; i++)
                // Передача
                spi_transmit(&buffer_data[i]);
        }
        // Если требуется задержка перед отправкой следующей команды
        if (chain->time != 0)
        {
            // Заводим таймер
            timer_start(&chain->timer, chain->time); 
            // Прекращаем обработку текущей цепочки команд
            break;
        }      
    }
    
    // Отключить SPI
    spi_disable();
}

// Отправка оставшейся цепочки команд после задержки
static void lcd_cmd_tx(timer_t *timer)
{
    lcd_chain_cmd_t *chain = CONTAINER_OF(timer, lcd_chain_cmd_t, timer);
    // Следующий элемент
    chain++;
    // Передача следующего элемента цепочки команд
    lcd_chain_cmd_tx(chain);
}

void lcd_init(void)
{
    // Массив элементов цепочки команд
    static lcd_chain_cmd_t lcd_chain_init[] = 
    {
        LCD_CMD_INIT(TIMER_MODE_ONE_SHOT, LCD_TIC_RESET, lcd_cmd_tx, NULL, LCD_CMD_SOFT_RESET),    // Программный сброс
        LCD_CMD_INIT(TIMER_MODE_ONE_SHOT, LCD_TIC_SLEEP_OUT, lcd_cmd_tx, NULL, LCD_CMD_SLEEP_OUT), // Выход из режима энергосбережения
        LCD_CMD_INIT(TIMER_MODE_ONE_SHOT, 0, lcd_cmd_tx, NULL, LCD_CMD_DISPLAY_ON),                // Включить дисплей
        LCD_CMD_INIT(TIMER_MODE_ONE_SHOT, 0, lcd_cmd_tx, NULL, LCD_CMD_NOP)                        // Команда завершения цепочки команд
    };
   
    // Передача цепочки команд
    lcd_chain_cmd_tx(lcd_chain_init);
}
/*
// Передача изображения
static void lcd_pixels_set()
{
    // Массив элементов цепочки команд
    static const lcd_chain_cmd_t lcd_chain_image_set[4] = 
    {
        // Установить адрес строки
        // Установить адрес столбца
        // Запись данных
        LCD_CMD_CHAIN_END
    };
    
    // Передача 
    lcd_chain_cmd_tx(lcd_chain_image_set);    
}
*/
