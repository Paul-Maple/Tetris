#include "lcd.h"
#include "lcd_reg.h"
#include "spi.h"
#include "io.h"

// Регистр Memory access control (8-ми битный)
uint8_t lcd_mac_reg = LCD_MAC_D1 | LCD_MAC_D2;

// Статическая инициализация элемента цепочки команд
#define LCD_CMD_STATIC_INIT(d, c, s)                                            \
{                                                                               \
    .data = d,                                                                  \
    .cmd  = c,                                                                  \
    .size = s                                                                   \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(data, cmd)     LCD_CMD_STATIC_INIT(data, cmd, sizeof(data))

// Команда завершения передачи
#define LCD_CMD_CHAIN_END       LCD_CMD_STATIC_INIT(NULL, LCD_CMD_NOP, 0)

// Отправка цепочки команд по SPI
static void lcd_chain_cmd_tx(const lcd_chain_cmd_t *chain)
{
    // Включить SPI
    spi_enable();
    
    // Цикл, пока не дойдёт до команды завершения передачи цепочки
    for ( ; chain->cmd != LCD_CMD_NOP; chain++)
    {
        // Установить вывод DCRS для отправки команды
        io_dcrs_set(LCD_CMD);
        // Отправка команды
        spi_transmit((uint8_t *)chain->cmd);
        
        // Если команда имеет параметры
        if (chain->data != NULL)
        {
            // Установить вывод DCRS для отправки данных
            io_dcrs_set(LCD_DATA);

            // Запись данных в буфер для отправки
            for (uint8_t i = 0; i < chain->size; i++)
            {
                uint8_t *buffer = (uint8_t *) chain->data + i;
                
                //Ждем, пока не освободится буфер передатчика
                while(!(SPI1->SR & SPI_SR_TXE))
                    { }
                spi_transmit(buffer);
            }
        }
    }
    
    // Отключить SPI
    spi_disable();
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
void lcd_init(void)
{
    // Массив элементов цепочки команд
    static const lcd_chain_cmd_t lcd_chain_display_on[] = 
    {
        LCD_CMD_STATIC_INIT(NULL, LCD_CMD_SOFT_RESET, 0),
        LCD_CMD_STATIC_INIT(NULL, LCD_CMD_SLEEP_OUT, 0),
        LCD_CMD_INIT(&lcd_mac_reg, LCD_CMD_MAC_SET),
        LCD_CMD_STATIC_INIT(NULL, LCD_CMD_DISPLAY_ON, 0),
        LCD_CMD_CHAIN_END
    };
    
    // Передача
    lcd_chain_cmd_tx(lcd_chain_display_on);    
}
