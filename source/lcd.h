#ifndef __LCD_H
#define __LCD_H

#include <typedefs.h>

// ������������ ������ ���������� ��������
typedef enum
{    
    LCD_CMD_SOFT_RESET      = 0x01,     // ����������� �����
    LCD_CMD_SLEEP_OUT       = 0x11,     // ����� �� ������ ���       
    LCD_CMD_NORMAL_MODE     = 0x13,     // ���������� ����� �����������
    LCD_CMD_MAC_SET         = 0x36,     // ��������� ������� � ������
    LCD_CMD_DISPLAY_ON      = 0x29,     // ��������� �������
    LCD_CMD_DISPLAY_OFF     = 0x28,     // ���������� �������
    LCD_CMD_COLLUM_PTR_SET  = 0x2A,     // ��������� ������ �������
    LCD_CMD_LINE_PTR_SET    = 0x2B,     // ��������� ������ ������    
    LCD_CMD_MEMORY_SET      = 0x2C,     // �������� ������ �� �� �� �������� ������
    LCD_PIXEL_FORMAT_SET    = 0x3A,     // ��������� ������� RGB
    LCD_GAMMA_SET           = 0x26,     // ��������� �����
    LCD_CMD_COLOR_SET       = 0x2D,     // ��������� �����
    
    LCD_CMD_NOP             = 0x00      // ������ ������� (��� ���������� ��������)
    // TODO: �������� ����������� �������
} lcd_cmd_t;

// ������������ ��������� ���� DCRS 
enum
{
    LCD_CMD,    // ������� - 0
    LCD_DATA    // ������  - 1
};

// ��������� �������� ������� ������
typedef struct
{
    // ��������� �� ������
    const void *data;
    // �������
    lcd_cmd_t cmd;
    // ������ ������ (� ������)
    uint8_t size;
    
} lcd_chain_cmd_t;

// ��������� ������� �������� ��� ������
typedef struct
{
    // ��������� ������ ������
    uint16_t *collum_start;
    uint16_t *line_start;
    
    // ��������� ����� ������
    uint16_t *collum_end;
    uint16_t *line_end;
    
    // ������ ��� ������
    void *rgb_data;
    
} lcd_pixels_prt;

// ������������� �������
void lcd_init(void);

#endif // __LCD_H
