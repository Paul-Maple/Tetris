#ifndef __IO_H
#define __IO_H

#include "mcu.h"

/***
        �������� I/O:
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
  
  idr     -   ���� ������� ������ (�������� ������ ��� ������ !!!!!!)
  
  ���������� uint32_t afrl � afrh ��� ������ � �������� AFR[0] � AFR[1] �������� �� uint64_t af
  ������� af ������ ��������� ����� 2-� ���������
  ��� ������ � �������� AFR[0] � AFR[1] ���������� ���� af � uint32_t � ����������� �������
***/

/*------------------------------------------------------------------------------*
 *      ���� ��� LCD:                                                           *
 *  � ������ Alternate function (AF5):                                          *
 *  SCL (CLK)  -  Serial Clock Line (������������)                              *
 *  SDA (MOSI) -  Serial Data (������ �� 8 ���)                                 *
 *  CSX (NSS)  -  Chip Select (����� ����������, ������� ��� "0")               *
 *  � ������ OUT:                                                               *
 *  DCRS  -  Data/Command Register Select (������ - "1" / ������� - "0")        *
 *  LED   -  ��������� (������������ ���)                                       *
 * ---------------------------------------------------------------------------- *
 *      ���� ��� �������:                                                       *
 * �������� � ������ �������������� ������� (AF0)                               *
 * JTMS - JTAG Mode Select                                                      *
 * JTCK - JTAG Test Clock                                                       *
 * JTDI - JTAG Data In                                                          *
 * ---------------------------------------------------------------------------- *
 *      ���� ������:                                                            *
 * TODO: ����������������� �������������� �������� ������ ������ button_1, � �� *
 */

// ����� ����� � ����������� 
#define IO_SHIFT_LEFT(type, data, shift)        (((type)(data)) << (shift))

        /*** ����������� ������� �������������� ����� ***/
    /* --- ���� --- */
// C�������� ��� �����
#define IO_LED_PIN      13

// ���� LCD
#define IO_LCD_SCL_PIN      1                                                   /*** SCL,   Port A ***/
#define IO_LCD_SDA_PIN      7                                                   /*** SDA,   Port A ***/
#define IO_LCD_CSX_PIN      4                                                   /*** CSX,   Port A ***/
#define IO_LCD_DCRS_PIN     3                                                   /*** DC/RS, Port A ***/
// TODO: ����������� ��������� �� ����
#define IO_LCD_LED_PIN      2                                                   /*** LED    ***/

// ���� ��� ������� (����� AF0)
#define IO_JTMS_PIN     13                                                      /*** JTAG pin ***/
#define IO_JTCK_PIN     14                                                      /*** JTAG pin ***/  
#define IO_JTDI_PIN     15                                                      /*** JTAG pin ***/ 

// ���� ������ (��� ���� �)
#define IO_BUTTON_0     0                                                       /***  ***/
#define IO_BUTTON_1     1                                                       /***  ***/
#define IO_BUTTON_2     2                                                       /***  ***/
#define IO_BUTTON_3     3                                                       /***  ***/

       /*** ����� � ��������� �������� � ��������� I/O ***/
// ����� ��������� I/O
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

// ���������� �������� � ��������� I/O
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

        /*** ����� ��� ������ � �������� I/O ***/
// ������ ������ �������
#define IO_INPUT_MODE_MASK      (0x0)                                           /*** Mode 00 ***/
#define IO_OUTPUT_MODE_MASK     (0x1)                                           /*** Mode 01 ***/
#define IO_ALTERNATE_MODE_MASK  (0x2)                                           /*** Mode 10 ***/
//#define IO_ANALOG_MODE_MASK   (0x3)                                           /*** Mode 11 ***/

// ������ �������� �������
#define IO_PULL_UP_MASK         (0x1)                                           /*** Mode 01 ***/
#define IO_PULL_DOWN_MASK       (0x2)                                           /*** Mode 10 ***/           

        /*** ��������� ������� ������ I/O (moder) ***/
// ��������� ������ Output
#define IO_OUT_MODE_SET(pin)                                                    \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_OUTPUT_MODE_MASK, (pin)*2)

// ��������� ������ Input           
#define IO_IN_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_INPUT_MODE_MASK, (pin)*2)

// ��������� ������ Alternate function            
#define IO_AF_MODE_SET(pin)                                                     \
    moder |= IO_SHIFT_LEFT(uint32_t, IO_ALTERNATE_MODE_MASK, (pin)*2) 

        /*** ��������� ������ �������� ����� (pupdr) ***/
// ��������� �������� ����
#define IO_PULL_DOWN_SET(pin)                                                   \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_DOWN_MASK, (pin)*2)

// ��������� �������� �����
#define IO_PULL_UP_SET(pin)                                                     \
    pupdr |= IO_SHIFT_LEFT(uint32_t, IO_PULL_UP_MASK, (pin)*2)

        /*** ��������� ������ �������������� ������� ***/
// ��������� ������ AF
#define IO_AF_NUMBER_SET(pin, number)                                           \
        af |=  IO_SHIFT_LEFT(uint64_t, number, (pin)*4)

        /*** ��������� ������������ ����� ***/
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

// ������������� GPIO
void io_init(void);

// ��������� ��������� ���� DCRS
void io_dcrs_set(bool state);

#endif // __GPIO_H
