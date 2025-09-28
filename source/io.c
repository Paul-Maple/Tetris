#include "io.h"

// ������������� GPIO
void io_init(void)
{
    // ������ ��� �������� ������������� �������� ���������
    uint32_t moder, otyper, ospeedr, pupdr, odr, bsrr, lckr, brr;
    uint64_t af;
    
    /*** ���� � ***/
    IO_RESET();
        IO_NC(0);
        IO_AF_PD(IO_LCD_SCL_PIN, 5);                                            // ������������ SPI
        IO_OUT_PU(IO_LCD_LED_PIN);                                              // ��������� �������
        IO_OUT_PD(IO_LCD_DCRS_PIN);                                             // ��� ������ �������/������
        IO_AF_PD(IO_LCD_CSX_PIN, 5);                                            // ��� ������ slave-����������
        IO_NC(5);
        IO_NC(6);
        IO_AF_PD(IO_LCD_SDA_PIN, 5);                                            // MOSI
        IO_NC(8);
        IO_NC(9);
        IO_NC(10);
        IO_NC(11);
        IO_NC(12);
        IO_AF_PD(IO_JTMS_PIN, 0);                                               // ��� ������� //
        IO_AF_PD(IO_JTCK_PIN, 0);                                               // ��� ������� //
        IO_AF_PD(IO_JTDI_PIN, 0);                                               // ��� ������� //
    IO_SAVE(A);
    
    /*** ���� B ***/
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
        IO_OUT_PD(IO_LED_PIN);                                                  // �������� ���������
        IO_NC(14);
        IO_NC(15);
    IO_SAVE(B); 
    
    /*** ���� C ***/
    IO_RESET();
        IO_IN_PD(IO_BUTTON_0);                                                  // ������ //
        IO_IN_PD(IO_BUTTON_1);                                                  // ������ //
        IO_IN_PD(IO_BUTTON_2);                                                  // ������ //
        IO_IN_PD(IO_BUTTON_3);                                                  // ������ //
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
    
    /*** ���� H ***/
    IO_RESET();
        IO_NC(0); 
        IO_NC(1);
        IO_NC(3);
    IO_SAVE(H);
}

void io_dcrs_set(bool state)
{
    GPIOA->ODR = IO_SHIFT_LEFT(uint32_t, state, IO_LCD_DCRS_PIN);
}
