#include "io.h"

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
 *      В режиме Alternate function (AF5):                                      *
 *  SCL (CLK)  -  Serial Clock Line (Тактирование)                              *
 *  SDA (MOSI) -  Serial Data Tx                                                *
 *  CSX (NSS)  -  Chip Select (Входа разрешения, активен при "0")               *
 *      В режиме OUT:                                                           *
 *  DCRS  -  Data/Command Register Select (Данные - "1" / Команды - "0")        *
 *  LED   -  Подсветка дисплея                                                  *
 * ---------------------------------------------------------------------------- *
 *      Пины для отладки:                                                       *
 *      В режиме альтернативной функции (AF0)                                   *
 * JTMS - JTAG Mode Select                                                      *
 * JTCK - JTAG Test Clock                                                       *
 * JTDI - JTAG Data In                                                          *
 * ---------------------------------------------------------------------------- *
 *      Пины кнопок:                                                            *
 * TODO: Прокомментировать функциональные названия кнопок вместо button_1, и тд *
 */

        /*** Определение номеров функциональных пинов ***/
// Cветодиод для теста
#define IO_LED_PIN          13

// Пин для измерения частоты
#define IO_MCO              8                                                   /*** MCO,   Port A ***/

// Пины LCD (Режим AF5)
#define IO_LCD_SCL_PIN      1                                                   /*** SCL,   Port A ***/
#define IO_LCD_CSX_PIN      4                                                   /*** CSX,   Port A ***/
#define IO_LCD_SDA_PIN      7                                                   /*** SDA,   Port A ***/

// Пины LCD (Режим OUT)
#define IO_LCD_RESX_PIN     0                                                   /*** RESET, Port A ***/
#define IO_LCD_DCRS_PIN     3                                                   /*** DC/RS, Port A ***/
// TODO: Реализовать подсветку на ШИМе
#define IO_LCD_LED_PIN      2                                                   /*** LED,   Port A ***/

// Пины для отладки (Режим AF0)
#define IO_JTMS_PIN         13                                                  /*** JTAG pin ***/
#define IO_JTCK_PIN         14                                                  /*** JTAG pin ***/
#define IO_JTDI_PIN         15                                                  /*** JTAG pin ***/

// Пины кнопок (Все порт С)
#define IO_BUTTON_0         0                                                   /***  ***/
#define IO_BUTTON_1         1                                                   /***  ***/
#define IO_BUTTON_2         2                                                   /***  ***/
#define IO_BUTTON_3         3                                                   /***  ***/

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
        
// Скорость работы GPIO
#define IO_VERY_HIGH_MODE_MASK  (0x3)                                           /*** Mode 11 ***/

// Режимы подтяжки выводов
#define IO_PULL_UP_MASK         (0x1)                                           /*** Mode 01 ***/
#define IO_PULL_DOWN_MASK       (0x2)                                           /*** Mode 10 ***/  

        /*** Установка скорости работы I/O (ospeedr) ***/
// Установка Very high speed
#define IO_SET_VH_SPEED(pin)                                                    \
    ospeedr |= IO_SHIFT_LEFT(uint32_t, IO_VERY_HIGH_MODE_MASK, (pin)*2)

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

        /*** Установка уровня сигнала на выводе ***/
// Установка высокого уровень сигнала
#define IO_OUT_HIGH_SET(pin)                                                    \
    odr |= IO_SHIFT_LEFT(uint32_t, 1, pin)

// Установка низкого уровень сигнала
#define IO_OUT_LOW_SET(pin)                                                     \
    odr &= ~IO_SHIFT_LEFT(uint32_t, 1, pin)

        /*** Установка итоговой конфигурации пинов ***/
// Non connected pin
#define IO_NC(pin)        IO_IN_PD(pin)

        /* --- Input --- */
// Input, Pull-Down
#define IO_IN_PD(pin)                                                           \
    IO_IN_MODE_SET(pin);                                                        \
    IO_PULL_DOWN_SET(pin)

        /* --- Output --- */
// Output, Pull-Down
#define IO_OUT_PD(pin)                                                          \
    IO_OUT_MODE_SET(pin);                                                       \
    IO_PULL_DOWN_SET(pin)

// Output, Pull-Up
#define IO_OUT_PU(pin)                                                          \
    IO_OUT_MODE_SET(pin);                                                       \
    IO_PULL_UP_SET(pin)

// Output LOW, Pull-Up
#define IO_OUT_LOW_PD(pin)                                                      \
    IO_OUT_PD(pin);                                                             \
    IO_OUT_LOW_SET(pin)

// Output HIGH, Pull-Up
#define IO_OUT_HIGH_PU(pin)                                                     \
    IO_OUT_PU(pin);                                                             \
    IO_OUT_HIGH_SET(pin)

        /* --- Alternate function --- */
// Alternate function           
#define IO_AF(pin, number)                                                      \
    IO_AF_MODE_SET(pin);                                                        \
    IO_AF_NUMBER_SET(pin, number)

// Alternate function, Pull-Down            
#define IO_AF_PD(pin, number)                                                   \
    IO_AF(pin, number);                                                         \
    IO_PULL_DOWN_SET(pin)

// Alternate function, Very high speed, Pull-Down
#define IO_AF_VH_PD(pin, number)                                                \
    IO_AF(pin, number);                                                         \
    IO_PULL_DOWN_SET(pin);                                                      \
    IO_SET_VH_SPEED(pin)
        
// Alternate function, Pull-Up          
#define IO_AF_PU(pin, number)                                                   \
    IO_AF(pin, number);                                                         \
    IO_PULL_UP_SET(pin)
        
// Инициализация GPIO
void io_init(void)
{
    // Буферы для хранения промежуточных значений регистров
    uint32_t moder, otyper, ospeedr, pupdr, odr, bsrr, lckr, brr;
    uint64_t af;
    
    /*** Порт А ***/
    IO_RESET();
        IO_OUT_PD(IO_LCD_RESX_PIN);                                             // Пин для аппаратного сброса     
        IO_AF_PD(IO_LCD_SCL_PIN, 5);                                            // Пин тактирования SPI
        IO_OUT_LOW_PD(IO_LCD_LED_PIN);                                          // Пин подсветки дисплея
        IO_OUT_LOW_PD(IO_LCD_DCRS_PIN);                                         // Пин выбора команды/данных
        IO_AF_PU(IO_LCD_CSX_PIN, 5);                                            // Пин выбора slave-устройства                                           
        IO_NC(5);
        IO_NC(6);
        IO_AF_PD(IO_LCD_SDA_PIN, 5);                                            // Пин для передачи данных
        //IO_AF_VH_PD(IO_MCO, 0);                                                    // Пин для измерения частоты
        IO_NC(8);
        IO_NC(9);
        IO_NC(10);
        IO_NC(11);
        IO_NC(12);
        IO_AF_PD(IO_JTMS_PIN, 0);                                               // Для отладки
        IO_AF_PD(IO_JTCK_PIN, 0);                                               // Для отладки
        IO_AF_PD(IO_JTDI_PIN, 0);                                               // Для отладки
    IO_SAVE(A);
    
    /*** Порт B ***/
    IO_RESET();
        IO_NC(0); 
        IO_NC(1);
        IO_NC(2);
        IO_NC(3);
        IO_NC(4);
        IO_NC(5);
        IO_NC(6);
        IO_NC(7);
        IO_NC(8);
        IO_NC(9);
        IO_NC(10);
        IO_NC(11);
        IO_NC(12);
        IO_OUT_PD(IO_LED_PIN);                                                  // Тестовый светодиод
        IO_NC(14);
        IO_NC(15);
    IO_SAVE(B); 
    
    /*** Порт C ***/
    IO_RESET();
        IO_IN_PD(IO_BUTTON_0);                                                  // Кнопка 
        IO_IN_PD(IO_BUTTON_1);                                                  // Кнопка 
        IO_IN_PD(IO_BUTTON_2);                                                  // Кнопка 
        IO_IN_PD(IO_BUTTON_3);                                                  // Кнопка 
        IO_NC(4);
        IO_NC(5);
        IO_NC(6);
        IO_NC(7);
        IO_NC(8);
        IO_NC(9);
        IO_NC(10);
        IO_NC(11);
        IO_NC(12);
        IO_NC(13);
        IO_NC(14);
        IO_NC(15);
    IO_SAVE(C);
    
    /*** Порт H ***/
    IO_RESET();
        IO_NC(0);
        IO_NC(1);
        IO_NC(3);
    IO_SAVE(H);
}

void io_lcd_hard_reset(void)
{
    GPIOA->ODR &= ~IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_RESX_PIN);
    for (uint16_t i = 0; i < 65000; i++);
    GPIOA->ODR |= IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_RESX_PIN);
}

void io_dcrs_set(const bool state)
{
    if (state)
        GPIOA->ODR |= IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_DCRS_PIN);
    else
        GPIOA->ODR &= ~IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_DCRS_PIN);
}

void io_led_on(void)
{
    GPIOA->ODR |= IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_LED_PIN);
}
