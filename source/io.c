#include "io.h"

// Инициализация GPIO
void io_init(void)
{
    // Буферы для хранения промежуточных значений регистров
    uint32_t moder, otyper, ospeedr, pupdr, odr, bsrr, lckr, brr;
    uint64_t af;
    
    /*** Порт А ***/
    IO_RESET();
        IO_OUT_HIGH_PU(IO_LCD_RESX_PIN);                                        // Пин для аппаратного сброса     
        IO_AF_PD(IO_LCD_SCL_PIN, 5);                                            // Пин тактирования SPI
        IO_OUT_LOW_PD(IO_LCD_LED_PIN);                                              // Пин подсветки дисплея
        IO_OUT_LOW_PD(IO_LCD_DCRS_PIN);                                             // Пин выбора команды/данных
       IO_AF_PU(4, 5);                                                         // Fake Chip select pin                                      
       IO_OUT_HIGH_PU(IO_LCD_CSX_PIN);                                              // Пин выбора slave-устройства
        IO_NC(6);                                                               
        IO_AF_PD(IO_LCD_SDA_PIN, 5);                                            // Пин для передачи данных
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

void io_reset_gpioa_pin(uint8_t pin)
{
    GPIOA->ODR &= ~IO_SHIFT_LEFT(uint32_t, 1, pin);
}

void io_lcd_hard_reset(void)
{
    GPIOA->ODR &= ~IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_RESX_PIN);
    for (uint16_t i = 0; i < 65000; i++);
    GPIOA->ODR |= IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_RESX_PIN);
}

void io_nss_set(const bool state)
{
    if (state)
        GPIOA->ODR |= IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_CSX_PIN);
    else
        GPIOA->ODR &= ~IO_SHIFT_LEFT(uint32_t, 1, IO_LCD_CSX_PIN);
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
