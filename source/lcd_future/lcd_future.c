/*
#include "lcd.h"
#include <lcd_reg.h>
#include "spi.h"
#include "io.h"
#include <timer.h>
#include <event.h>
#include <test.h>

// Тики таймера для асинхронной задержки отправки команд
#define LCD_TIME_DELAY_1MS          TIMER_TICKS_MS(1)
#define LCD_TIME_DELAY_5MS          TIMER_TICKS_MS(5)
#define LCD_TIME_DELAY_120MS        TIMER_TICKS_MS(120)

// Состояния пина DCRS 
#define LCD_DCRS_DATA        1    // Данные
#define LCD_DCRS_CMD         0    // Команда

// Команды для управления дисплеем
#define LCD_CMD_SOFT_RESET              0x01         // Программный сброс
#define LCD_CMD_SLEEP_OUT               0x11         // Выход из режима сна       
#define LCD_CMD_DISPLAY_ON              0x29         // Включение дисплея
#define LCD_CMD_DISPLAY_OFF             0x28         // Выключение дисплея
#define LCD_CMD_NORMAL_MODE             0x13         // Нормальный режим отображения
#define LCD_CMD_MAC_SET                 0x36         // Настройка доступа к памяти
#define LCD_CMD_PIXEL_FORMAT_SET        0x3A         // Установка формата RGB
#define LCD_CMD_INTERFACE_CONTROL_SET   0x6F         // Установка способа передачи и отображения данных
#define LCD_CMD_COLLUM_SET              0x2A         // Установка адреса столбца
#define LCD_CMD_LINE_SET                0x2B         // Установка адреса строки    
#define LCD_CMD_MEMORY_SET              0x2C         // Передача данных от МК до кадровой памяти
#define LCD_CMD_GAMMA_SET               0x26         // Установка гаммы
#define LCD_CMD_COLOR_SET               0x2D         // Преобразование цветовой глубины с 16 бит в 18 бит
#define LCD_CMD_NOP                     0x00         // Пустая команда (Для завершения передачи)
// TODO: Добавить необходимые команды, а лишние убрать

// Статическая инициализация элемента цепочки команд
#define LCD_CMD_STATIC_INIT(_cmd, _data, _size)                                 \
{                                                                               \
    .cmd  = _cmd                                                                \
    .data = _data,                                                              \
    .size = _size,                                                              \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(data, cmd)     LCD_CMD_STATIC_INIT(cmd, &(data), sizeof(data))

//   Последовательность команд для инициализации ILI9341:
io_lcd_hard_reset()     - Аппаратный сброс
io_led_on()             - Включить подсветку дисплея
lcd_reset()             - Программный сброс
lcd_pixel_format_set()  - Формат пикселя
lcd_mac_set()           - Доступ к памяти
lcd_ic_set()            - Способа передачи и отображения данных
lcd_sleep_out()         - Выход из режима сна
lcd_diplay_on()         - Включить дисплей


    // Регистры LCD /
// Регистр формата пикселя 5 + 6 + 5  ( 16 bit RGB )
static const uint8_t lcd_pixel_format_reg = LCD_PIXEL_16_BIT;
// Регистр доступа к памяти дисплея
static const uint8_t lcd_mac_reg = 0b00000000;
// Регистр способа передачи и отображения данных
static const uint8_t lcd_ic_reg[3] = 
{
    0b00000000,
    0b00000000,
    LCD_IC3_DM_RGB | LCD_IC3_RM_RGB,
};

    // Предварительные обяъвления функций 
static void lcd_reset(void);
static void lcd_pixel_format_set(void);
static void lcd_mac_set(void);
static void lcd_ic_set(void);
static void lcd_sleep_out(void);
static void lcd_diplay_on(void);

// Список команд для инициализации
static list_t lcd_cmd_init_list = LIST_STATIC_INIT();

// События отправки команд
static event_t lcd_reset_event            = EVENT_STATIC_INIT(lcd_reset);
static event_t lcd_pixel_format_set_event = EVENT_STATIC_INIT(lcd_pixel_format_set);
static event_t lcd_mac_set_event          = EVENT_STATIC_INIT(lcd_mac_set);
static event_t lcd_ic_set_event           = EVENT_STATIC_INIT(lcd_ic_set);
static event_t lcd_sleep_out_event        = EVENT_STATIC_INIT(lcd_sleep_out);
static event_t lcd_diplay_on_event        = EVENT_STATIC_INIT(lcd_diplay_on);

// Флаг активной задержки отправки команд
static bool lcd_delay_flag = false;

// Отправка команд после задержки
// Вызывается как callback у сработавшего таймера задержки 
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
static void lcd_cmd_tx(const lcd_cmd_t *msg)
{
    ASSERT_NULL_PTR(msg);
    
    // Включить SPI
    spi_enable();
    
    // Установить вывод DCRS в "0" для отправки команды
    io_dcrs_set(LCD_DCRS_CMD);
    
    // Отправка команды
    spi_transmit((uint8_t *)&msg->cmd);
    
    // Если команда имеет параметры
    if (msg->size > 0)
    {
        // Установить вывод DCRS в "1" для отправки данных
        io_dcrs_set(LCD_DCRS_DATA);

        // Передача
        spi_transmit((uint8_t *)msg->data, );
    }
    
    // Отключить SPI
    spi_disable();
}

static void lcd_delay_timer_start(timer_interval_t interval)
{
    // Запуск таймера
    timer_start(&lcd_delay_cmd_timer, interval);
    // Установка флага задержки
    lcd_delay_flag = true;
}

// Установка контроля интерфейса 
static void lcd_ic_set(void)
{
    const lcd_cmd_t interface_control = LCD_CMD_INIT(lcd_ic_reg, LCD_CMD_INTERFACE_CONTROL_SET);
    
    // Передача
    lcd_cmd_tx(&interface_control);
}

// Формат пикселя (16-bit)
static void lcd_pixel_format_set(void)
{
    const lcd_cmd_t pixel_format = LCD_CMD_INIT(lcd_pixel_format_reg, LCD_CMD_PIXEL_FORMAT_SET);
    
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
    
    // Тестовая отрисовка (TODO: удалить)
    test_init();
}

// Выход из режима сна
static void lcd_sleep_out(void)
{
    const lcd_cmd_t sleep_out = LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SLEEP_OUT);
    
    // Передача
    lcd_cmd_tx(&sleep_out);
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_120MS);
}

// Программный сброс
static void lcd_reset(void)
{
    const lcd_cmd_t reset = LCD_CMD_STATIC_INIT(NULL, 0, LCD_CMD_SOFT_RESET);
    
    // Передача
    lcd_cmd_tx(&reset);
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_5MS);
}

void lcd_init(void)
{
    // Аппаратный сброс
    io_lcd_hard_reset();
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_120MS);
    
    // Включить подсветку дисплея
    io_led_on();
    
    // Добавляем события для инициализации в список
    list_insert(&lcd_cmd_init_list, &lcd_reset_event.item);
    list_insert(&lcd_cmd_init_list, &lcd_pixel_format_set_event.item);
    list_insert(&lcd_cmd_init_list, &lcd_mac_set_event.item);
    list_insert(&lcd_cmd_init_list, &lcd_ic_set_event.item);
    list_insert(&lcd_cmd_init_list, &lcd_sleep_out_event.item);
    list_insert(&lcd_cmd_init_list, &lcd_diplay_on_event.item);
}

void lcd_image_set(const lcd_position_t *position, const uint16_t color)
{
    // Массив координат X (Big-Endian по 8 бит)
    const uint8_t x[4] = 
    {
        (uint8_t)(position->x1 >> 8),
        (uint8_t)(position->x1),
        (uint8_t)(position->x2 >> 8),
        (uint8_t)(position->x2)
    };
    
    // Массив координат Y (Big-Endian по 8 бит)
    const uint8_t y[4] = 
    {
        (uint8_t)(position->y1 >> 8),
        (uint8_t)(position->y1),
        (uint8_t)(position->y2 >> 8),
        (uint8_t)(position->y2)
    };
    
    // Массив цвета (Big-Endian по 8 бит)
    const uint8_t color_big_endian[2] = 
    {
        (uint8_t)(color >> 8),
        (uint8_t)(color)
    };
    
    // Установка адреса началного и конечного столбца
    const lcd_cmd_t collum_set = LCD_CMD_INIT(x, LCD_CMD_COLLUM_SET);   
    // Установка адреса начальной и конечной строки
    const lcd_cmd_t line_set = LCD_CMD_INIT(y, LCD_CMD_LINE_SET);       
    // Команда записи цвета
    const lcd_cmd_t color_set = LCD_CMD_INIT(color_big_endian, LCD_CMD_MEMORY_SET);               
    
    // Передача команд
    lcd_cmd_tx(&collum_set);
    lcd_cmd_tx(&line_set);
    lcd_cmd_tx(&color_set);
}
*/
