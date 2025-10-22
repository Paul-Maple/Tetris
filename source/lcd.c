#include "lcd.h"
#include <lcd_reg.h>
#include "spi.h"
#include "io.h"
#include <timer.h>
#include <event.h>
#include <test.h>

/*   Последовательность команд для инициализации ILI9341:
io_lcd_hard_reset()     - Аппаратный сброс
io_led_on()             - Включить подсветку дисплея
lcd_reset()             - Программный сброс
lcd_sleep_out()         - Выход из режима сна
lcd_lcd_configuration() - Настройка LCD
lcd_diplay_on()         - Включить дисплей
*/

// Тики таймера для асинхронной задержки отправки команд
//#define LCD_TIME_DELAY_1MS          TIMER_TICKS_MS(1)
#define LCD_TIME_DELAY_5MS          TIMER_TICKS_MS(5)
#define LCD_TIME_DELAY_120MS        TIMER_TICKS_MS(120)

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
    .cmd  = _cmd,                                                               \
    .data = _data,                                                              \
    .size = _size                                                               \
}

//  Инициализация элемента цепочки команд
#define LCD_CMD_INIT(cmd, data)     LCD_CMD_STATIC_INIT(cmd, &(data), sizeof(data))

    /*** Регистры LCD ***/ /*
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
};*/

    /*** Предварительные обяъвления функций ***/
static void lcd_reset(void);
static void lcd_configuration(void);
static void lcd_sleep_out(void);
static void lcd_diplay_on(void);
extern void test_init(void);

// Список команд для инициализации
static list_t lcd_cmd_init_list = LIST_STATIC_INIT();

// События отправки команд
static event_t lcd_reset_event            = EVENT_STATIC_INIT(lcd_reset);
static event_t lcd_configuration_event    = EVENT_STATIC_INIT(lcd_configuration);
static event_t lcd_sleep_out_event        = EVENT_STATIC_INIT(lcd_sleep_out);
static event_t lcd_diplay_on_event        = EVENT_STATIC_INIT(lcd_diplay_on);
static event_t lcd_set_image_event        = EVENT_STATIC_INIT(test_init);

// Флаг активной задержки отправки команд
static bool lcd_delay_flag = false;

// Отправка команд после задержки
/* Вызывается как callback у сработавшего таймера задержки */
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

/*
// Отправка команды по SPI
static void lcd_cmd_tx(const lcd_cmd_t *msg)
{
    ASSERT_NULL_PTR(msg);
    
    // Установить вывод DCRS в "0" для отправки команды
    
    
    // Отправка команды
    spi_transmit_cmd(msg->cmd);
    
    // Если команда имеет параметры
    if (msg->size > 0)
    {
        // Установить вывод DCRS в "1" для отправки данных
    

        // Передача данных
        spi_transmit_data((uint16_t )&msg->data);
        spi_transmit_data((uint16_t )&msg->data + 1);
    }
}   */

// Настройка регистров LCD для инициализации
static void lcd_configuration(void)
{
	//power control A
	spi_transmit_cmd(0xCB);
	spi_transmit_data(0x39);
	spi_transmit_data(0x2C);
	spi_transmit_data(0x00);
	spi_transmit_data(0x34);
	spi_transmit_data(0x02);

	//power control B
	spi_transmit_cmd(0xCF);
	spi_transmit_data(0x00);
	spi_transmit_data(0xC1);
	spi_transmit_data(0x30);

	//driver timing control A
	spi_transmit_cmd(0xE8);
	spi_transmit_data(0x85);
	spi_transmit_data(0x00);
	spi_transmit_data(0x78);

	//driver timing control B
	spi_transmit_cmd(0xEA);
	spi_transmit_data(0x00);
	spi_transmit_data(0x00);

	//power on sequence control
	spi_transmit_cmd(0xED);
	spi_transmit_data(0x64);
	spi_transmit_data(0x03);
	spi_transmit_data(0x12);
	spi_transmit_data(0x81);

	//pump ratio control
	spi_transmit_cmd(0xF7);
	spi_transmit_data(0x20);

	//power control,VRH[5:0]
	spi_transmit_cmd(0xC0);
	spi_transmit_data(0x23);

	//Power control,SAP[2:0];BT[3:0]
	spi_transmit_cmd(0xC1);
	spi_transmit_data(0x10);

	//vcm control
	spi_transmit_cmd(0xC5);
	spi_transmit_data(0x3E);
	spi_transmit_data(0x28);

	//vcm control 2
	spi_transmit_cmd(0xC7);
	spi_transmit_data(0x86);

	//memory access control
	spi_transmit_cmd(0x36);
	spi_transmit_data(0x48);

	//pixel format
	spi_transmit_cmd(0x3A);
	spi_transmit_data(0x55);

	//frameration control,normal mode full colours
	spi_transmit_cmd(0xB1);
	spi_transmit_data(0x00);
	spi_transmit_data(0x18);

	//display function control
	spi_transmit_cmd(0xB6);
	spi_transmit_data(0x08);
	spi_transmit_data(0x82);
	spi_transmit_data(0x27);

	//3gamma function disable
	spi_transmit_cmd(0xF2);
	spi_transmit_data(0x00);

	//gamma curve selected
	spi_transmit_cmd(0x26);
	spi_transmit_data(0x01);

	//set positive gamma correction
	spi_transmit_cmd(0xE0);
	spi_transmit_data(0x0F);
	spi_transmit_data(0x31);
	spi_transmit_data(0x2B);
	spi_transmit_data(0x0C);
	spi_transmit_data(0x0E);
	spi_transmit_data(0x08);
	spi_transmit_data(0x4E);
	spi_transmit_data(0xF1);
	spi_transmit_data(0x37);
	spi_transmit_data(0x07);
	spi_transmit_data(0x10);
	spi_transmit_data(0x03);
	spi_transmit_data(0x0E);
	spi_transmit_data(0x09);
	spi_transmit_data(0x00);

	//set negative gamma correction
	spi_transmit_cmd(0xE1);
	spi_transmit_data(0x00);
	spi_transmit_data(0x0E);
	spi_transmit_data(0x14);
	spi_transmit_data(0x03);
	spi_transmit_data(0x11);
	spi_transmit_data(0x07);
	spi_transmit_data(0x31);
	spi_transmit_data(0xC1);
	spi_transmit_data(0x48);
	spi_transmit_data(0x08);
	spi_transmit_data(0x0F);
	spi_transmit_data(0x0C);
	spi_transmit_data(0x31);
	spi_transmit_data(0x36);
	spi_transmit_data(0x0F);
}

static void lcd_delay_timer_start(timer_interval_t interval)
{
    // Запуск таймера
    timer_start(&lcd_delay_cmd_timer, interval);
    // Установка флага задержки
    lcd_delay_flag = true;
}
/*
// Установка контроля интерфейса 
static void lcd_ic_set(void)
{
    const lcd_cmd_t interface_control = LCD_CMD_INIT(LCD_CMD_INTERFACE_CONTROL_SET, lcd_ic_reg);
    
    // Передача
    lcd_cmd_tx(&interface_control);
}

// Формат пикселя (16-bit)
static void lcd_pixel_format_set(void)
{
    const lcd_cmd_t pixel_format = LCD_CMD_INIT(LCD_CMD_PIXEL_FORMAT_SET, lcd_pixel_format_reg);
    
    // Передача
    lcd_cmd_tx(&pixel_format);
}

// Memory Access Control
static void lcd_mac_set(void)
{
    const lcd_cmd_t mac_reg = LCD_CMD_INIT(LCD_CMD_MAC_SET, lcd_mac_reg);
    
    // Передача
    lcd_cmd_tx(&mac_reg);
}   */

// Включить дисплей
static void lcd_diplay_on(void)
{
    const lcd_cmd_t display_on = LCD_CMD_STATIC_INIT(LCD_CMD_DISPLAY_ON, NULL, 0);
    
    // Передача
    //lcd_cmd_tx(&display_on);
    
    spi_transmit_cmd(display_on.cmd);
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_120MS);
}

// Выход из режима сна
static void lcd_sleep_out(void)
{
    const lcd_cmd_t sleep_out = LCD_CMD_STATIC_INIT(LCD_CMD_SLEEP_OUT, NULL, 0);
    
    // Передача
    //lcd_cmd_tx(&sleep_out);
    
    spi_transmit_cmd(sleep_out.cmd);
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_120MS);
}

// Программный сброс
static void lcd_reset(void)
{
    const lcd_cmd_t reset = LCD_CMD_STATIC_INIT(LCD_CMD_SOFT_RESET, NULL, 0);
    
    // Передача
    //lcd_cmd_tx(&reset);
    
    spi_transmit_cmd(reset.cmd);
    
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
    
    list_insert(&lcd_cmd_init_list, &lcd_set_image_event.item);                 // Set image
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_120MS);
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
    /*
    const uint16_t x[2] = 
    {
        position->x1,
        position->x2
    }; 
    
    const uint16_t y[2] = 
    {
        position->y1,
        position->y2
    }; 
    
    // Установка адреса началного и конечного столбца
    const lcd_cmd_t collum_set = LCD_CMD_INIT(LCD_CMD_COLLUM_SET, x);   
    // Установка адреса начальной и конечной строки
    const lcd_cmd_t line_set = LCD_CMD_INIT(LCD_CMD_LINE_SET, y);       
    // Команда записи цвета
    const lcd_cmd_t color_set = LCD_CMD_INIT(LCD_CMD_MEMORY_SET, color);               
    
    // Передача команд
    lcd_cmd_tx(&collum_set);
    lcd_cmd_tx(&line_set);
    lcd_cmd_tx(&color_set);
    */
    spi_transmit_cmd(LCD_CMD_COLLUM_SET);
    spi_transmit_data(x[0]);
    spi_transmit_data(x[1]);
    spi_transmit_data(x[2]);
    spi_transmit_data(x[3]);
    
    spi_transmit_cmd(LCD_CMD_LINE_SET);
    spi_transmit_data(y[0]);
    spi_transmit_data(y[1]);
    spi_transmit_data(y[2]);
    spi_transmit_data(y[3]);
    
    spi_transmit_cmd(LCD_CMD_MEMORY_SET);
    spi_transmit_data(color_big_endian[0]);
    spi_transmit_data(color_big_endian[1]);
    
}
