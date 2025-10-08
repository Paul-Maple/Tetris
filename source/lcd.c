#include "lcd.h"
#include <lcd_reg.h>
#include "spi.h"
#include "io.h"
#include <timer.h>

// Тики таймера для задержки отправки команд
#define LCD_TIME_DELAY_5MS          TIMER_TICKS_MS(5)
#define LCD_TIME_DELAY_120MS        TIMER_TICKS_MS(120)

// Состояния пина DCRS 
#define LCD_CMD         0    // Команда
#define LCD_DATA        1    // Данные

// Команды для управления дисплеем
#define LCD_CMD_SOFT_RESET          0x01         // Программный сброс
#define LCD_CMD_SLEEP_OUT           0x11         // Выход из режима сна       
#define LCD_CMD_NORMAL_MODE         0x13         // Нормальный режим отображения
#define LCD_CMD_DISPLAY_ON          0x29         // Включение дисплея
#define LCD_CMD_DISPLAY_OFF         0x28         // Выключение дисплея
#define LCD_CMD_MAC_SET             0x36         // Настройка доступа к памяти
#define LCD_CMD_COLLUM_SET          0x2A         // Установка адреса столбца
#define LCD_CMD_LINE_SET            0x2B         // Установка адреса строки    
#define LCD_CMD_MEMORY_SET          0x2C         // Передача данных от МК до кадровой памяти
#define LCD_CMD_PIXEL_FORMAT_SET    0x3A         // Установка формата RGB
#define LCD_CMD_GAMMA_SET           0x26         // Установка гаммы
#define LCD_CMD_COLOR_SET           0x2D         // Преобразование цветовой глубины с 16 бит в 18 бит
#define LCD_CMD_NOP                 0x00         // Пустая команда (Для завершения передачи)
// TODO: Добавить необходимые команды, 
// а лишние убрать

// Статическая инициализация элемента цепочки команд
#define LCD_CMD_STATIC_INIT(_data, _size, _cmd)                                 \
{                                                                               \
    .data = _data,                                                              \
    .size = _size,                                                              \
    .cmd  = _cmd                                                                \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(data, cmd)     LCD_CMD_STATIC_INIT((void*)(data), sizeof(data), cmd)
   
// Формат пикселя 5 + 6 + 5 bit RGB
static const uint8_t lcd_pixel_format = 0b01010101;
// Memory Access Control register
//static const uint8_t lcd_mac_reg = 0b00000000;

// Предварительное объявление функции отправки команд после задержки
static void lcd_delay_cmd_tx(timer_t * timer);

// Таймер для задержки отправки команд
static timer_t lcd_delay_cmd_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, lcd_delay_cmd_tx);

// Отправка цепочки команд по SPI
static void lcd_chain_cmd_tx(const lcd_chain_cmd_t *chain)
{           
    ASSERT_NULL_PTR(chain);
    
    // Включить SPI
    spi_enable();
    
    // Проходимся по списку команд
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
            
            // Буфер для данных 
            uint8_t *buffer_data = (uint8_t *)chain->data;
            
            // Запись данных в буфер для отправки (MSB)
            for (uint8_t i = 0; i < chain->size; i++)
                // Передача
                spi_transmit(&buffer_data[i]);
        }
        
        // Если команды с задержкой
        if (chain->cmd == LCD_CMD_SOFT_RESET || chain->cmd == LCD_CMD_SLEEP_OUT)
        {
            // Задержка нужна
            //timer_start(&lcd_delay_cmd_timer, LCD_TIME_DELAY_5MS);
            //break;
        }
    }
    
    // Отключить SPI
    spi_disable();
}

// Отправка команд после задержки
/* Вызывается как callback у сработавшего таймера задержки */
static void lcd_delay_cmd_tx(timer_t * timer)
{
    //lcd_chain_cmd_tx();
}

void lcd_init(void)
{
    // Включить подсветку дисплея
    io_led_on();
    
    // Статический массив элементов цепочки команд для инициализации
    const lcd_chain_cmd_t chain_init[] = 
    {
        // Инициализация команд
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SOFT_RESET),           // Программный сброс
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SLEEP_OUT),            // Выход из режима сна
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_DISPLAY_ON),           // Включить дисплей
        LCD_CMD_INIT(&lcd_pixel_format, LCD_CMD_PIXEL_FORMAT_SET),  // Формат пикселя (16-bit)
        //LCD_CMD_INIT(&lcd_mac_reg, LCD_CMD_MAC_SET),              // Memory Access Control (default state)
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_NOP)                   // Команда завершения цепочки команд
    };
    
    // Отправка цепочки команд
    lcd_chain_cmd_tx(chain_init);
}

void lcd_image_set(const lcd_position_t *position, const uint16_t color)
{
    // Массив элементов цепочки команд для установки изображения
    const lcd_chain_cmd_t chain_image[] = 
    {
        LCD_CMD_INIT(position->x, LCD_CMD_COLLUM_SET),   // Установить адреса начальной и конечной столбца
        LCD_CMD_INIT(position->y, LCD_CMD_LINE_SET),     // Установить адреса началного и конечного строки
        LCD_CMD_INIT(color, LCD_CMD_MEMORY_SET),         // Команда записи цвета
        LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_NOP)        // Команда завершения цепочки команд        
    };
    
    // Передача
    lcd_chain_cmd_tx(chain_image);
}   
