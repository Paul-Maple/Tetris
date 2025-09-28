#ifndef __IODEFS_H
#define __IODEFS_H

#include "mcu.h"

/***
        Все регистры I/O:
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
***/

// Сдвиг влево с приведением 
#define IO_SHIFT_LEFT(type, data, shift)        (((type)(data)) << (shift))

        /*** Определение номеров функциональных пинов ***/
// Пин светодиода
#define IO_LED_PIN      13
// Порт светодиода
#define IO_LED_PORT     GPIOB      

// Пины для отладки
#define IO_JTMS_PIN     13                                                      /*** JTAG Mode Select ***/
#define IO_JTCK_PIN     14                                                      /*** JTAG Test Clock  ***/  
#define IO_JTDI_PIN     15                                                      /*** JTAG Data In     ***/    

       /*** Сброс и установка значений в регистрах I/O ***/
// Сброс регистров I/O
#define IO_RESET()                                                              \
    moder = 0;                                                                  \
    pupdr = 0          

// Сохранение значений в регистрах I/O
#define IO_SAVE(x)                                                              \
    GPIO##x->MODER = moder;                                                     \
    GPIO##x->PUPDR = pupdr     
        
        /*** Маски для записи в регистры I/O ***/
// Режимы работы выводов
#define IO_INPUT_MODE_MASK      (0x0)                                           /*** Mode 00 ***/
#define IO_OUTPUT_MODE_MASK     (0x1)                                           /*** Mode 01 ***/
#define IO_ALTERNATE_MODE_MASK  (0x2)                                           /*** Mode 10 ***/
//#define IO_ANALOG_MODE_MASK   (0x3)                                           /*** Mode 11 ***/

// Режимы подтяжки выводов
#define IO_PULL_UP_MASK         (0x1)                                           /*** Mode 01 ***/
#define IO_PULL_DOWN_MASK       (0x2)                                           /*** Mode 10 ***/           
        
        /*** Установка режимов работы I/O ***/
// Установка режима Output
#define IO_OUT_MODE_SET(pin)                                                    \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_OUTPUT_MODE_MASK, (pin)*2)

// Установка режима Input           
#define IO_IN_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_INPUT_MODE_MASK, (pin)*2)
    
// Установка режима Alternate function            
#define IO_AF_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_ALTERNATE_MODE_MASK, (pin)*2) 
  
        /*** Установка режима подтяжки пинов ***/
// Установка подтяжки вниз
#define IO_PULL_DOWN_SET(pin)                                                   \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_DOWN_MASK, (pin)*2)
            
// Установка подтяжки вверх
#define IO_PULL_UP_SET(pin)                                                     \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_UP_MASK, (pin)*2)
            
        /*** Настройка конфигурации пинов ***/
// Настройка I/O в режиме Output, Pull-Down
#define IO_OUT_PD(pin)                                                          \
    IO_OUT_MODE_SET(pin);                                                       \
    IO_PULL_DOWN_SET(pin)
        
// Настройка I/O в режиме Input, Pull-Down       
#define IO_NC(pin)                                                              \
    IO_IN_MODE_SET(pin);                                                        \
    IO_PULL_DOWN_SET(pin)

// Настройка I/O в режиме Alternate function, Pull-Down            
#define IO_AF_PD(pin)                                                           \
    IO_AF_MODE_SET(pin);                                                        \
    IO_PULL_DOWN_SET(pin)    
    
            /*** Установка низкого/высокого уровня на I/O ***/ 
// TODO: Добавить void io_set_out(void)

#endif // __GPIODEFS_H
