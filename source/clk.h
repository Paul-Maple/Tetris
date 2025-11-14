#ifndef __CLK_H
#define __CLK_H

// Перечисление источников тактирования
typedef enum 
{
    CLK_LF_SRC_LSI,
    CLK_LF_SRC_LSE,
} clk_lf_src_t;

// Текущий источник НЧ (только чтение)
extern clk_lf_src_t clk_lf_src;

// Инициализация модуля
void clk_init(void);

// Обработчик сбоя LSE
void clk_lse_fail(void);
// Обработчик готовности LSE
void clk_lse_ready(void);

// Обработчик прерывания сбоя LSE
void clk_lse_fail_isr(void);
// Обработчик прерывания стабилизации LSE
void clk_lse_ready_isr(void);

#endif // __CLK_H
