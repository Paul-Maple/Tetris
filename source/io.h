#ifndef __IO_H
#define __IO_H

#include <mcu.h>

// Сдвиг влево с приведением 
#define IO_SHIFT_LEFT(type, data, shift)        (((type)(data)) << (shift))

/***
        Регистры I/O:
  moder   -   Режим работы пина 
  otyper  -   Тип режима вывода  
  ospeedr -   Скорость вывода 
  pupdr   -   Тип подтяжки
  odr     -   Установка ODRx бит для вывода данных
  bsrr    -   Установка/Сброс ODRx бит для вывода данных
  lckr    -   Блокировка конфигурации битов порта 
  afrl    -   Настройка альтернативной функции (LOW) 
  afrh    -   Настройка альтернативной функции (HIGH)
  brr     -   Сброс ODRx бит для вывода данных
  
  idr     -   Порт входных данных (ДОСТУПЕН ТОЛЬКО ДЛЯ ЧТЕНИЯ !!!!!!)
  
  Переменные uint32_t afrl и afrh для записи в регистры AFR[0] и AFR[1] заменены на uint64_t af
  Поэтому af хранит состояние сразу 2-х регистров
  При записи в регистры AFR[0] и AFR[1] происходит каст af к uint32_t с необходимым сдвигом
***/

/*------------------------------------------------------------------------------*
 *      Пины для LCD:                                                           *
 *  В режиме Alternate function (AF5):                                          *
 *  SCL (CLK)  -  Serial Clock Line (Тактирование)                              *
 *  SDA (MOSI) -  Serial Data (Данные по 8 бит)                                 *
 *  CSX (NSS)  -  Chip Select (Входа разрешения, активен при "0")               *
 *  В режиме OUT:                                                               *
 *  DCRS  -  Data/Command Register Select (Данные - "1" / Команды - "0")        *
 *  LED   -  Подсветка (Регулируется ШИМ)                                       *
 * ---------------------------------------------------------------------------- *
 *      Пины для отладки:                                                       *
 * Работают в режиме альтернативной функции (AF0)                               *
 * JTMS - JTAG Mode Select                                                      *
 * JTCK - JTAG Test Clock                                                       *
 * JTDI - JTAG Data In                                                          *
 * ---------------------------------------------------------------------------- *
 *      Пины кнопок:                                                            *
 * TODO: Прокомментировать функциональные названия кнопок вместо button_1, и тд *
 */

        /*** Определение номеров функциональных пинов ***/
// Cветодиод для теста
#define IO_LED_PIN      13

// Пины LCD
#define IO_LCD_SCL_PIN      1                                                   /*** SCL,   Port A ***/
#define IO_LCD_SDA_PIN      7                                                   /*** SDA,   Port A ***/
#define IO_LCD_CSX_PIN      4                                                   /*** CSX,   Port A ***/
#define IO_LCD_DCRS_PIN     3                                                   /*** DC/RS, Port A ***/
#define IO_LCD_MISO_PIN     6
// TODO: Реализовать подсветку на ШИМе
#define IO_LCD_LED_PIN      2                                                   /*** LED,   Port A ***/

// Пины для отладки (Режим AF0)
#define IO_JTMS_PIN     13                                                      /*** JTAG pin ***/
#define IO_JTCK_PIN     14                                                      /*** JTAG pin ***/  
#define IO_JTDI_PIN     15                                                      /*** JTAG pin ***/ 

// Пины кнопок (Все порт С)
#define IO_BUTTON_0     0                                                       /***  ***/
#define IO_BUTTON_1     1                                                       /***  ***/
#define IO_BUTTON_2     2                                                       /***  ***/
#define IO_BUTTON_3     3                                                       /***  ***/

       /*** Сброс и установка значений в регистрах I/O ***/
// Сброс регистров I/O
#define IO_RESET()                                                              \
    moder   = 0;                                                                \
    otyper  = 0;                                                                \
    ospeedr = 0;                                                                \
    pupdr   = 0;                                                                \
    odr     = 0;                                                                \
    bsrr    = 0;                                                                \
    lckr    = 0;                                                                \
    af      = 0;                                                                \
    brr     = 0

// Сохранение значений в регистрах I/O
#define IO_SAVE(x)                                                              \
    GPIO##x->MODER   = moder;                                                   \
    GPIO##x->OTYPER  = otyper;                                                  \
    GPIO##x->OSPEEDR = ospeedr;                                                 \
    GPIO##x->PUPDR   = pupdr;                                                   \
    GPIO##x->ODR     = odr;                                                     \
    GPIO##x->BSRR    = bsrr;                                                    \
    GPIO##x->LCKR    = lckr;                                                    \
    GPIO##x->AFR[0]  = ((uint32_t)af);                                          \
    GPIO##x->AFR[1]  = ((uint32_t)(af >> 32));                                  \
    GPIO##x->BRR     = brr

        /*** Маски для записи в регистры I/O ***/
// Режимы работы выводов
#define IO_INPUT_MODE_MASK      (0x0)                                           /*** Mode 00 ***/
#define IO_OUTPUT_MODE_MASK     (0x1)                                           /*** Mode 01 ***/
#define IO_ALTERNATE_MODE_MASK  (0x2)                                           /*** Mode 10 ***/
//#define IO_ANALOG_MODE_MASK   (0x3)                                           /*** Mode 11 ***/

// Режимы подтяжки выводов
#define IO_NO_PULL_MASK         (0x0)                                           /*** Mode 00 ***/
#define IO_PULL_UP_MASK         (0x1)                                           /*** Mode 01 ***/
#define IO_PULL_DOWN_MASK       (0x2)                                           /*** Mode 10 ***/           

        /*** Установка режимов работы I/O (moder) ***/
// Установка режима Output
#define IO_OUT_MODE_SET(pin)                                                    \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_OUTPUT_MODE_MASK, (pin)*2)

// Установка режима Input           
#define IO_IN_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_INPUT_MODE_MASK, (pin)*2)

// Установка режима Alternate function            
#define IO_AF_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_ALTERNATE_MODE_MASK, (pin)*2) 

        /*** Установка режима подтяжки пинов (pupdr) ***/
// Установка подтяжки вниз
#define IO_PULL_DOWN_SET(pin)                                                   \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_DOWN_MASK, (pin)*2)

// Установка подтяжки вверх
#define IO_PULL_UP_SET(pin)                                                     \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_UP_MASK, (pin)*2)

        /*** Установка номера альтернативной функции ***/
// Установка номера AF
#define IO_AF_NUMBER_SET(pin, number)                                           \
        af |=  IO_SHIFT_LEFT(uint64_t, number, (pin)*4)

        /*** Установка конфигурации пинов ***/
// Non connected pin
#define IO_NC(pin)                                                              \
    IO_IN_MODE_SET(pin);                                                        \
    IO_PULL_DOWN_SET(pin)

// Input, Pull-Down
#define IO_IN_PD(pin)        IO_NC(pin)
        
// Output, Pull-Down
#define IO_OUT_PD(pin)                                                          \
    IO_OUT_MODE_SET(pin);                                                       \
    IO_PULL_DOWN_SET(pin)

// Output, Pull-Up
#define IO_OUT_PU(pin)                                                          \
    IO_OUT_MODE_SET(pin);                                                       \
    IO_PULL_UP_SET(pin)
        
// Alternate function, Pull-Down            
#define IO_AF_PD(pin, number)                                                   \
    IO_AF_MODE_SET(pin);                                                        \
    IO_AF_NUMBER_SET(pin, number);                                              \
    IO_PULL_DOWN_SET(pin)

// Инициализация GPIO
void io_init(void);

// Установка состояния пина DCRS
void io_dcrs_set(bool state);

// Включить подсветку дисплея
void io_led_on();

#endif // __GPIO_H
