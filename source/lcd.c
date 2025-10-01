#include "lcd.h"
#include "lcd_reg.h"
#include "spi.h"
#include "io.h"

// Регистр MAC (Memory access control 8-ми битный)
// TODO: Записать в регистр необходимые значения бит
uint8_t lcd_mac_reg = 0b00000000;

// Статическая инициализация элемента цепочки команд
#define LCD_CMD_STATIC_INIT(_time, _data, _size, _cmd)                          \
{                                                                               \
    .time = _time,                                                              \
    .data = _data,                                                              \
    .size = _size,                                                              \
    .cmd  = _cmd                                                                \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(time, data, cmd)                                           \
    LCD_CMD_STATIC_INIT(time, (const void*)(data), sizeof(data), cmd)

// Таймер для задержки отправки команды 
//static timer_t lcd_delay_cmd_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, lcd_chain_cmd_tx);

// Отправка цепочки команд по SPI
static void lcd_chain_cmd_tx(lcd_chain_cmd_t *chain)
{
    ASSERT_NULL_PTR(chain);
    
    // Счётчик команд в цепочке
    //static uint8_t cld_coutn_cmd = 0;

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
        //if (chain->time != 0)     
    }
    
    // Отключить SPI
    spi_disable();
}
/*
// Отправка оставшейся цепочки команд после задержки
static void lcd_cmd_tx(timer_t *timer)
{
    
}   
*/

void lcd_init(void)
{
    // Включить подсветку дисплея
    io_led_on();
    
    // Массив элементов цепочки команд
    static lcd_chain_cmd_t lcd_chain_init[] = 
    {
        // Инициализация команд
        //-----------Время задержки---Данные--Команда 
        LCD_CMD_INIT(LCD_TIC_RESET,     NULL, LCD_CMD_SOFT_RESET),              // Программный сброс
        LCD_CMD_INIT(LCD_TIC_SLEEP_OUT, NULL, LCD_CMD_SLEEP_OUT),               // Выход из режима энергосбережения
        LCD_CMD_INIT(0,                 NULL, LCD_CMD_DISPLAY_ON),              // Включить дисплей
        LCD_CMD_INIT(0,                 NULL, LCD_CMD_NOP),                     // Команда завершения цепочки команд
    };
    
    // Передача цепочки команд
    lcd_chain_cmd_tx(lcd_chain_init);
}

// Инициализация элемента цепочки команд для установки столбцов записи
lcd_chain_cmd_t lcd_cmd_collum_init(const lcd_image_t *image)
{
    lcd_chain_cmd_t cmd_collum_set;
    cmd_collum_set.time = 0;
    // TODO: Проверить приведение uint16_t к const void*
    cmd_collum_set.data = &image->collum_start;
    cmd_collum_set.size = sizeof(cmd_collum_set.data);
    cmd_collum_set.cmd  =  LCD_CMD_COLLUM_SET;
    return cmd_collum_set;
}

// Передача изображения
static void lcd_image_set(const lcd_image_t *image)
{
    lcd_chain_cmd_t collum_set = lcd_cmd_collum_init(image);
    
    // Массив элементов цепочки команд
    lcd_chain_cmd_t chain_image[] = 
    {
        collum_set,                                                             // Установить адреса начальной и конечной строки
        // Установить адреса началного и конечного столбца
        // Запись данных
    };

    // Передача 
    lcd_chain_cmd_tx(chain_image);  
}
