#include "lcd.h"
#include "lcd_reg.h"
#include "spi.h"
#include "io.h"

// ������� Memory access control (8-�� ������)
uint8_t lcd_mac_reg = LCD_MAC_D1 | LCD_MAC_D2;

// ����������� ������������� �������� ������� ������
#define LCD_CMD_STATIC_INIT(d, c, s)                                            \
{                                                                               \
    .data = d,                                                                  \
    .cmd  = c,                                                                  \
    .size = s                                                                   \
}

//  ������������� �������� ������� ������
#define LCD_CMD_INIT(data, cmd)     LCD_CMD_STATIC_INIT(data, cmd, sizeof(data))

// ������� ���������� ��������
#define LCD_CMD_CHAIN_END       LCD_CMD_STATIC_INIT(NULL, LCD_CMD_NOP, 0)

// �������� ������� ������ �� SPI
static void lcd_chain_cmd_tx(const lcd_chain_cmd_t *chain)
{
    // �������� SPI
    spi_enable();
    
    // ����, ���� �� ����� �� ������� ���������� �������� �������
    for ( ; chain->cmd != LCD_CMD_NOP; chain++)
    {
        // ���������� ����� DCRS ��� �������� �������
        io_dcrs_set(LCD_CMD);
        // �������� �������
        spi_transmit((uint8_t *)chain->cmd);
        
        // ���� ������� ����� ���������
        if (chain->data != NULL)
        {
            // ���������� ����� DCRS ��� �������� ������
            io_dcrs_set(LCD_DATA);

            // ������ ������ � ����� ��� ��������
            for (uint8_t i = 0; i < chain->size; i++)
            {
                uint8_t *buffer = (uint8_t *) chain->data + i;
                
                //����, ���� �� ����������� ����� �����������
                while(!(SPI1->SR & SPI_SR_TXE))
                    { }
                spi_transmit(buffer);
            }
        }
    }
    
    // ��������� SPI
    spi_disable();
}
/*
// �������� �����������
static void lcd_pixels_set()
{
    // ������ ��������� ������� ������
    static const lcd_chain_cmd_t lcd_chain_image_set[4] = 
    {
        // ���������� ����� ������
        // ���������� ����� �������
        // ������ ������
        LCD_CMD_CHAIN_END
    };
    
    // �������� 
    lcd_chain_cmd_tx(lcd_chain_image_set);    
}
*/
void lcd_init(void)
{
    // ������ ��������� ������� ������
    static const lcd_chain_cmd_t lcd_chain_display_on[] = 
    {
        LCD_CMD_STATIC_INIT(NULL, LCD_CMD_SOFT_RESET, 0),
        LCD_CMD_STATIC_INIT(NULL, LCD_CMD_SLEEP_OUT, 0),
        LCD_CMD_INIT(&lcd_mac_reg, LCD_CMD_MAC_SET),
        LCD_CMD_STATIC_INIT(NULL, LCD_CMD_DISPLAY_ON, 0),
        LCD_CMD_CHAIN_END
    };
    
    // ��������
    lcd_chain_cmd_tx(lcd_chain_display_on);    
}
