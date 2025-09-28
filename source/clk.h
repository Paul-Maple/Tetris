#ifndef __CLK_H
#define __CLK_H

/* ������������ ���� ������� �� ������ �� ���� ����������:  *
 * LSI - ���������� RC-��������� ( 32 000 �� )              *
 * LS� - ������� ����� ( 32768 ��)                          */

// ������������ ���������� ������������
typedef enum 
{
    CLK_LF_SRC_LSI,
    CLK_LF_SRC_LSE,
} clk_lf_src_t;

// ������� �������� �� (������ ������)
extern clk_lf_src_t clk_lf_src;

// ������������� ������
void clk_init(void);

// ���������� ���� LSE
void clk_lse_fail(void);
// ���������� ���������� LSE
void clk_lse_ready(void);

// ���������� ���������� ���� LSE
void clk_lse_fail_isr(void);
// ���������� ���������� ������������ LSE
void clk_lse_ready_isr(void);

#endif // __CLK_H
