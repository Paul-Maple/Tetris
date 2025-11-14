#include "lcd.h"
#include "lcd_reg.h"
#include "spi.h"
#include "io.h"
#include "timer.h"
#include "event.h"
#include "tetris.h"

/*   Последовательность команд для инициализации ILI9341:
io_lcd_hard_reset()     - Аппаратный сброс
io_led_on()             - Включить подсветку дисплея
lcd_reset()             - Программный сброс
lcd_sleep_out()         - Выход из режима сна
lcd_lcd_configuration() - Настройка LCD
lcd_diplay_on()         - Включить дисплей
*/

// Состояния пина DCRS 
#define LCD_DCRS_CMD         0    // Команда
#define LCD_DCRS_DATA        1    // Данные

// Тики таймера для асинхронной задержки отправки команд
#define LCD_TIME_DELAY_50MS          TIMER_TICKS_MS(50)
#define LCD_TIME_DELAY_120MS        TIMER_TICKS_MS(120)

// Команды для управления дисплеем
#define LCD_CMD_SOFT_RESET              0x01         // Программный сброс
#define LCD_CMD_SLEEP_OUT               0x11         // Выход из режима сна       
#define LCD_CMD_DISPLAY_ON              0x29         // Включение дисплея
#define LCD_CMD_DISPLAY_OFF             0x28         // Выключение дисплея
#define LCD_CMD_MAC_SET                 0x36         // Настройка доступа к памяти
#define LCD_CMD_PIXEL_FORMAT_SET        0x3A         // Установка формата RGB
#define LCD_CMD_COLLUM_SET              0x2A         // Установка адреса столбца
#define LCD_CMD_LINE_SET                0x2B         // Установка адреса строки    
#define LCD_CMD_MEMORY_SET              0x2C         // Передача данных от МК до кадровой памяти
#define LCD_CMD_MEMORY_READ             0x2E         // Чтение памяти дисплея
// TODO: Добавить необходимые команды, а лишние убрать

// Предварительные обяъвления функций
static void lcd_reset(void);
static void lcd_configuration(void);
static void lcd_sleep_out(void);
static void lcd_diplay_on(void);

// Список команд для инициализации
static list_t lcd_cmd_init_list = LIST_STATIC_INIT();

// События отправки команд
static event_t lcd_reset_event         = EVENT_STATIC_INIT(lcd_reset);
static event_t lcd_configuration_event = EVENT_STATIC_INIT(lcd_configuration);
static event_t lcd_sleep_out_event     = EVENT_STATIC_INIT(lcd_sleep_out);
static event_t lcd_diplay_on_event     = EVENT_STATIC_INIT(lcd_diplay_on);
static event_t lcd_start_game_event    = EVENT_STATIC_INIT(tetris_init);

// Флаг активной задержки отправки команд
static bool lcd_delay_flag = false;

// Отправка команд после задержки
/* Вызывается как callback у сработавшего таймера задержки *
 * или явно, в случае если задерка для отправки не нужна   */
static void lcd_delay_cmd_tx(timer_t * timer)
{
    // Сброс флага задержки
    lcd_delay_flag = false;
    
    // Обработка списка команд 
    for (list_item_t *temp_item = lcd_cmd_init_list.head; temp_item != NULL; temp_item = temp_item->next)
    {
        event_t * const temp_event = (event_t *)lcd_cmd_init_list.head;
        // Вызов отправки команды
        temp_event->cb();
        // Удаляем из списка
        list_remove(&lcd_cmd_init_list, &temp_event->item);
        
        // Если после отправки команды установлена задержка - выход из цикла
        if (lcd_delay_flag)
            break;
    }
}

// Таймер для задержки отправки команд
static timer_t lcd_delay_cmd_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, lcd_delay_cmd_tx);

// Отправка команды по SPI
static void lcd_cmd_tx(const uint8_t cmd)
{
    // Установить вывод DCRS в >0> для отправки команды
    io_dcrs_set(LCD_DCRS_CMD);
    
    // Передача
    spi_transmit(cmd);
}

// Отправка данных по SPI
static void lcd_data_tx(const uint8_t data)
{
    // Установить вывод DCRS в >1> для отправки данных
    io_dcrs_set(LCD_DCRS_DATA);
    
    // Передача
    spi_transmit(data);
}

// Отправка данных по SPI
static void lcd_color_tx(const uint16_t color, const uint32_t size)
{
    // Установить вывод DCRS в >1> для отправки данных
    io_dcrs_set(LCD_DCRS_DATA);
    
    // Передача
    spi_transmit_color(color, size);
}

// Настройка регистров LCD для инициализации
static void lcd_configuration(void)
{	
    // Memory access control
	lcd_cmd_tx(LCD_CMD_MAC_SET);
	lcd_data_tx((uint8_t)LCD_MAC_BGR);
    
	// Pixel format
	lcd_cmd_tx(LCD_CMD_PIXEL_FORMAT_SET);
	lcd_data_tx((uint8_t)LCD_PIXEL_16_BIT);
}

// Запуск таймера задержки отправки команд и данных по SPI
static void lcd_delay_timer_start(timer_interval_t interval)
{
    // Запуск таймера
    timer_start(&lcd_delay_cmd_timer, interval);
    // Установка флага задержки
    lcd_delay_flag = true;
}

// Включить дисплей
static void lcd_diplay_on(void)
{
    lcd_cmd_tx(LCD_CMD_DISPLAY_ON);
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_50MS);
}

// Выход из режима сна
static void lcd_sleep_out(void)
{
    lcd_cmd_tx(LCD_CMD_SLEEP_OUT);
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_120MS);
}

// Программный сброс
static void lcd_reset(void)
{
    lcd_cmd_tx(LCD_CMD_SOFT_RESET);
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_120MS);
}

void lcd_init(void)
{
    // Аппаратный сброс
    io_lcd_hard_reset();
    
    // Включить подсветку дисплея
    io_led_on();
    
    // Добавляем события для инициализации в список
    list_insert(&lcd_cmd_init_list, &lcd_reset_event.item);                     // Soft reset
    list_insert(&lcd_cmd_init_list, &lcd_sleep_out_event.item);                 // Sleep OUT
    list_insert(&lcd_cmd_init_list, &lcd_configuration_event.item);             // Settings LCD
    list_insert(&lcd_cmd_init_list, &lcd_diplay_on_event.item);                 // Display ON
    
    list_insert(&lcd_cmd_init_list, &lcd_start_game_event.item);                // Start game
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_50MS);
}

void lcd_draw_image(const lcd_position_t position, const uint16_t color)
{
    // Отправка координат X
    lcd_cmd_tx(LCD_CMD_COLLUM_SET);
    lcd_data_tx(position.x1 >> 8);
    lcd_data_tx(position.x1);
    lcd_data_tx(position.x2 >> 8);
    lcd_data_tx(position.x2);
    
    // Отправка координат Y
    lcd_cmd_tx(LCD_CMD_LINE_SET);
    lcd_data_tx(position.y1 >> 8);
    lcd_data_tx(position.y1);
    lcd_data_tx(position.y2 >> 8);
    lcd_data_tx(position.y2);
    
    // Заливка области одним цветом
    lcd_cmd_tx(LCD_CMD_MEMORY_SET);
    
    // Отправка цвета области
    lcd_color_tx(color, (uint32_t)((position.x2 - position.x1 + 1) * (position.y2 - position.y1 + 1)));
}

void lcd_clear(void)
{
    // Установка координат
    const lcd_position_t position = 
    {
        .x1 = 0,
        .x2 = 239,
        .y1 = 0,
        .y2 = 319
    };
    
    lcd_draw_image(position, LCD_COLOR_WHITE); 
}
