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

// Состояния пина DCRS 
#define LCD_DCRS_CMD         0    // Команда
#define LCD_DCRS_DATA        1    // Данные

// Тики таймера для асинхронной задержки отправки команд
//#define LCD_TIME_DELAY_1MS          TIMER_TICKS_MS(1)
#define LCD_TIME_DELAY_50MS          TIMER_TICKS_MS(50)
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
static event_t lcd_reset_event         = EVENT_STATIC_INIT(lcd_reset);
static event_t lcd_configuration_event = EVENT_STATIC_INIT(lcd_configuration);
static event_t lcd_sleep_out_event     = EVENT_STATIC_INIT(lcd_sleep_out);
static event_t lcd_diplay_on_event     = EVENT_STATIC_INIT(lcd_diplay_on);
static event_t lcd_set_image_event     = EVENT_STATIC_INIT(test_init);

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
    // Установить вывод DCRS в "0" для отправки команды
    io_dcrs_set(LCD_DCRS_CMD);
    
    // Передача
    spi_transmit(cmd);
}

// Отправка данных по SPI
static void lcd_data_tx(const uint8_t data)
{
    // Установить вывод DCRS в "1" для отправки данных
    io_dcrs_set(LCD_DCRS_DATA);
    
    // Передача
    spi_transmit(data);
}

// Отправка данных по SPI
static void lcd_color_tx(const uint16_t color, const uint32_t size)
{
    // Установить вывод DCRS в "1" для отправки данных
    io_dcrs_set(LCD_DCRS_DATA);
    
    // Передача
    spi_transmit_color(color, size);
}

// Настройка регистров LCD для инициализации
static void lcd_configuration(void)
{
    //power control A
	lcd_cmd_tx(0xCB);
	lcd_data_tx(0x39);
	lcd_data_tx(0x2C);
	lcd_data_tx(0x00);
	lcd_data_tx(0x34);
	lcd_data_tx(0x02);

	//power control B
	lcd_cmd_tx(0xCF);
	lcd_data_tx(0x00);
	lcd_data_tx(0xC1);
	lcd_data_tx(0x30);

	//driver timing control A
	lcd_cmd_tx(0xE8);
	lcd_data_tx(0x85);
	lcd_data_tx(0x00);
	lcd_data_tx(0x78);

	//driver timing control B
	lcd_cmd_tx(0xEA);
	lcd_data_tx(0x00);
	lcd_data_tx(0x00);

	//power on sequence control
	lcd_cmd_tx(0xED);
	lcd_data_tx(0x64);
	lcd_data_tx(0x03);
	lcd_data_tx(0x12);
	lcd_data_tx(0x81);

	//pump ratio control
	lcd_cmd_tx(0xF7);
	lcd_data_tx(0x20);

	//power control,VRH[5:0]
	lcd_cmd_tx(0xC0);
	lcd_data_tx(0x23);

	//Power control,SAP[2:0];BT[3:0]
	lcd_cmd_tx(0xC1);
	lcd_data_tx(0x10);

	//vcm control
	lcd_cmd_tx(0xC5);
	lcd_data_tx(0x3E);
	lcd_data_tx(0x28);

	//vcm control 2
	lcd_cmd_tx(0xC7);
	lcd_data_tx(0x86);

	//memory access control
	lcd_cmd_tx(0x36);
	lcd_data_tx(0x48);

	//pixel format
	lcd_cmd_tx(0x3A);
	lcd_data_tx(0x55);

	//frameration control,normal mode full colours
	lcd_cmd_tx(0xB1);
	lcd_data_tx(0x00);
	lcd_data_tx(0x18);

	//display function control
	lcd_cmd_tx(0xB6);
	lcd_data_tx(0x08);
	lcd_data_tx(0x82);
	lcd_data_tx(0x27);

	//3gamma function disable
	lcd_cmd_tx(0xF2);
	lcd_data_tx(0x00);

	//gamma curve selected
	lcd_cmd_tx(0x26);
	lcd_data_tx(0x01);

	//set positive gamma correction
	lcd_cmd_tx(0xE0);
	lcd_data_tx(0x0F);
	lcd_data_tx(0x31);
	lcd_data_tx(0x2B);
	lcd_data_tx(0x0C);
	lcd_data_tx(0x0E);
	lcd_data_tx(0x08);
	lcd_data_tx(0x4E);
	lcd_data_tx(0xF1);
	lcd_data_tx(0x37);
	lcd_data_tx(0x07);
	lcd_data_tx(0x10);
	lcd_data_tx(0x03);
	lcd_data_tx(0x0E);
	lcd_data_tx(0x09);
	lcd_data_tx(0x00);

	//set negative gamma correction
	lcd_cmd_tx(0xE1);
	lcd_data_tx(0x00);
	lcd_data_tx(0x0E);
	lcd_data_tx(0x14);
	lcd_data_tx(0x03);
	lcd_data_tx(0x11);
	lcd_data_tx(0x07);
	lcd_data_tx(0x31);
	lcd_data_tx(0xC1);
	lcd_data_tx(0x48);
	lcd_data_tx(0x08);
	lcd_data_tx(0x0F);
	lcd_data_tx(0x0C);
	lcd_data_tx(0x31);
	lcd_data_tx(0x36);
	lcd_data_tx(0x0F);
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
    
    // TODO: Убрать отрисовку
    list_insert(&lcd_cmd_init_list, &lcd_set_image_event.item);                 // Set image
    
    // Запуск таймера для задержки отправки следующей команды
    lcd_delay_timer_start(LCD_TIME_DELAY_50MS);
}

// TODO: переименовать void lcd_draw_image()
void lcd_image_set(const lcd_position_t *position, const uint16_t color)
{
    // Отправка координат X
    lcd_cmd_tx(LCD_CMD_COLLUM_SET);
    lcd_data_tx(position->x1 >> 8);
    lcd_data_tx(position->x1);
    lcd_data_tx(position->x2 >> 8);
    lcd_data_tx(position->x2);
    
    // Отправка координат Y
    lcd_cmd_tx(LCD_CMD_LINE_SET);
    lcd_data_tx(position->y1 >> 8);
    lcd_data_tx(position->y1);
    lcd_data_tx(position->y2 >> 8);
    lcd_data_tx(position->y2);
    
    // Заливка области одним цветом
    lcd_cmd_tx(LCD_CMD_MEMORY_SET);
    
    // Размер области
    uint32_t size = (uint32_t)((position->x2 - position->x1) * (position->y2 - position->y1));
    
    // Отправка цвета области
    lcd_color_tx(color, size);
}
