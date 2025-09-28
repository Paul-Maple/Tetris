#ifndef __IODEFS_H
#define __IODEFS_H

#include "mcu.h"

/***
        ��� �������� I/O:
  moder   -   ����� ������ ���� 
  otyper  -   ��� ������ ������  
  ospeedr -   �������� ������ 
  pupdr   -   ��� ��������
  odr     -   ��������� ODRx ��� ��� ������ ������
  bsrr    -   ���������/����� ODRx ��� ��� ������ ������
  lckr    -   ���������� ������������ ����� ����� 
  afrl    -   ��������� �������������� ������� (LOW) 
  afrh    -   ��������� �������������� ������� (HIGH)
  brr     -   ����� ODRx ��� ��� ������ ������
***/

// ����� ����� � ����������� 
#define IO_SHIFT_LEFT(type, data, shift)        (((type)(data)) << (shift))

        /*** ����������� ������� �������������� ����� ***/
// ��� ����������
#define IO_LED_PIN      13
// ���� ����������
#define IO_LED_PORT     GPIOB      

// ���� ��� �������
#define IO_JTMS_PIN     13                                                      /*** JTAG Mode Select ***/
#define IO_JTCK_PIN     14                                                      /*** JTAG Test Clock  ***/  
#define IO_JTDI_PIN     15                                                      /*** JTAG Data In     ***/    

       /*** ����� � ��������� �������� � ��������� I/O ***/
// ����� ��������� I/O
#define IO_RESET()                                                              \
    moder = 0;                                                                  \
    pupdr = 0          

// ���������� �������� � ��������� I/O
#define IO_SAVE(x)                                                              \
    GPIO##x->MODER = moder;                                                     \
    GPIO##x->PUPDR = pupdr     
        
        /*** ����� ��� ������ � �������� I/O ***/
// ������ ������ �������
#define IO_INPUT_MODE_MASK      (0x0)                                           /*** Mode 00 ***/
#define IO_OUTPUT_MODE_MASK     (0x1)                                           /*** Mode 01 ***/
#define IO_ALTERNATE_MODE_MASK  (0x2)                                           /*** Mode 10 ***/
//#define IO_ANALOG_MODE_MASK   (0x3)                                           /*** Mode 11 ***/

// ������ �������� �������
#define IO_PULL_UP_MASK         (0x1)                                           /*** Mode 01 ***/
#define IO_PULL_DOWN_MASK       (0x2)                                           /*** Mode 10 ***/           
        
        /*** ��������� ������� ������ I/O ***/
// ��������� ������ Output
#define IO_OUT_MODE_SET(pin)                                                    \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_OUTPUT_MODE_MASK, (pin)*2)

// ��������� ������ Input           
#define IO_IN_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_INPUT_MODE_MASK, (pin)*2)
    
// ��������� ������ Alternate function            
#define IO_AF_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_ALTERNATE_MODE_MASK, (pin)*2) 
  
        /*** ��������� ������ �������� ����� ***/
// ��������� �������� ����
#define IO_PULL_DOWN_SET(pin)                                                   \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_DOWN_MASK, (pin)*2)
            
// ��������� �������� �����
#define IO_PULL_UP_SET(pin)                                                     \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_UP_MASK, (pin)*2)
            
        /*** ��������� ������������ ����� ***/
// ��������� I/O � ������ Output, Pull-Down
#define IO_OUT_PD(pin)                                                          \
    IO_OUT_MODE_SET(pin);                                                       \
    IO_PULL_DOWN_SET(pin)
        
// ��������� I/O � ������ Input, Pull-Down       
#define IO_NC(pin)                                                              \
    IO_IN_MODE_SET(pin);                                                        \
    IO_PULL_DOWN_SET(pin)

// ��������� I/O � ������ Alternate function, Pull-Down            
#define IO_AF_PD(pin)                                                           \
    IO_AF_MODE_SET(pin);                                                        \
    IO_PULL_DOWN_SET(pin)    
    
            /*** ��������� �������/�������� ������ �� I/O ***/ 
// TODO: �������� void io_set_out(void)

#endif // __GPIODEFS_H
