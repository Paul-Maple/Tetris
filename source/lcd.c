#include "lcd.h"
#include <lcd_reg.h>
#include "spi.h"
#include "io.h"
#include <timer.h>
#include <test.h>

// Тики таймера для задержки отправки команд
#define LCD_TIME_DELAY_5MS          TIMER_TICKS_MS(5)
#define LCD_TIME_DELAY_120MS        TIMER_TICKS_MS(120)

// Состояния пина DCRS 
#define LCD_DCRS_DATA        1    // Данные
#define LCD_DCRS_CMD         0    // Команда

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
// TODO: Добавить необходимые команды, а лишние убрать

// Статическая инициализация элемента цепочки команд
#define LCD_CMD_STATIC_INIT(_data, _size, _cmd)                                 \
{                                                                               \
    .data = _data,                                                              \
    .size = _size,                                                              \
    .cmd  = _cmd                                                                \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(data, cmd)     LCD_CMD_STATIC_INIT(&(data), sizeof(data), cmd)

// Формат пикселя 5 + 6 + 5 bit RGB
static const uint8_t lcd_pixel_format = 0b01010101;
// Memory Access Control register
static const uint8_t lcd_mac_reg = 0b00000000;

// Предварительное объявление функции отправки команд после задержки
static void lcd_delay_cmd_tx(timer_t * timer);
// Таймер для задержки отправки команд
static timer_t lcd_delay_cmd_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, lcd_delay_cmd_tx);

// Прототип функции для отправки следующей команды
typedef void (* lcd_cmd_next_cb) (void);
// Функция для отправки следующей команды
static lcd_cmd_next_cb lcd_cmd_next;

// Отправка команд после задержки
/* Вызывается как callback у сработавшего таймера задержки */
static void lcd_delay_cmd_tx(timer_t * timer)
{
    // Вызов следующей функции
    lcd_cmd_next();
}

// Отправка команды по SPI
static void lcd_cmd_tx(const lcd_cmd_t *msg)
{
    ASSERT_NULL_PTR(msg);
    
    // Включить SPI
    spi_enable();
    
    // Установить вывод DCRS в "0" для отправки команды
    io_dcrs_set(LCD_DCRS_CMD);
    
    const uint8_t *buffer_cmd = &msg->cmd;
    // Отправка команды
    spi_transmit(buffer_cmd);
    
    // Если команда имеет параметры
    if (msg->size > 0)
    {
        // Установить вывод DCRS в "1" для отправки данных
        io_dcrs_set(LCD_DCRS_DATA);
        
        // Запись данных в буфер для отправки ( Big-Endian + LSB )
        for (uint8_t i = 0; i < msg->size; i++)
        {
            // Буфер для данных
            uint8_t *buffer_data = (uint8_t *)msg->data + i;
            // Передача
            spi_transmit(buffer_data);
        }
    }
    
    // Отключить SPI
    spi_disable();
}

// Формат пикселя (16-bit)
static void lcd_pixel_format_set(void)
{
    const lcd_cmd_t pixel_format = LCD_CMD_INIT(lcd_pixel_format, LCD_CMD_PIXEL_FORMAT_SET);
    
    // Передача
    lcd_cmd_tx(&pixel_format);
}

// Memory Access Control
static void lcd_mac_set(void)
{
    const lcd_cmd_t mac_reg = LCD_CMD_INIT(lcd_mac_reg, LCD_CMD_MAC_SET);
    
    // Передача
    lcd_cmd_tx(&mac_reg);
}
// Включить дисплей
static void lcd_diplay_on(void)
{
    const lcd_cmd_t display_on = LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_DISPLAY_ON);
    
    // Передача
    lcd_cmd_tx(&display_on);
    
    // Вызов функций для настройки дисплея
    lcd_pixel_format_set();
    lcd_mac_set();
    
    // Тестовая отрисовка
    test_init();
}

// Выход из режима сна
static void lcd_sleep_out(void)
{   
    const lcd_cmd_t sleep_out = LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SLEEP_OUT);
    
    // Передача
    lcd_cmd_tx(&sleep_out);
    // Следующая команда - display on
    lcd_cmd_next = lcd_diplay_on;
    // Запуск таймера для задержки отправки следующей команды (120 мс)
    timer_start(&lcd_delay_cmd_timer, LCD_TIME_DELAY_120MS);
}

// Программный сброс
static void lcd_reset(void)
{   
    const lcd_cmd_t reset = LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SOFT_RESET);
    
    // Передача
    lcd_cmd_tx(&reset);
    // Следующая команда - sleep out
    lcd_cmd_next = lcd_sleep_out;
    
    // Запуск таймера для задержки отправки следующей команды (5 мс)
    timer_start(&lcd_delay_cmd_timer, LCD_TIME_DELAY_5MS);
}

void lcd_init(void)
{
    // Включить подсветку дисплея
    io_led_on();
    
    // Установить вывод RESX = "1"
    //io_lcd_resx_high();
    // Установить вывод RESX = "0"
    //io_lcd_resx_low();
    
    // Программный сброс
    lcd_reset();
}

void lcd_image_set(const lcd_position_t *position, const uint16_t color)
{
    // Массив координат X (Big-Endian по 8 бит)
    uint8_t x[4] = 
    {
        (uint8_t)(position->x1 >> 8),
        (uint8_t)(position->x1),
        (uint8_t)(position->x2 >> 8),
        (uint8_t)(position->x2)
    };
    
    // Массив координат Y (Big-Endian по 8 бит) 
    uint8_t y[4] = 
    {
        (uint8_t)(position->y1 >> 8),
        (uint8_t)(position->y1),
        (uint8_t)(position->y2 >> 8),
        (uint8_t)(position->y2)
    };
    
    // Установка адреса началного и конечного столбца
    const lcd_cmd_t collum_set = LCD_CMD_INIT(x, LCD_CMD_COLLUM_SET);   
    // Установка адреса начальной и конечной строки
    const lcd_cmd_t line_set = LCD_CMD_INIT(y, LCD_CMD_LINE_SET);       
    // Команда записи цвета
    const lcd_cmd_t color_set = LCD_CMD_INIT(color, LCD_CMD_MEMORY_SET);               
    
    // Передача команд
    lcd_cmd_tx(&collum_set);
    lcd_cmd_tx(&line_set);
    lcd_cmd_tx(&color_set);
}
