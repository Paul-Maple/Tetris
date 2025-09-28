#ifndef __LPTIM1_H
#define __LPTIM1_H

// Оповещение модуля c lptim1 о смене источника тактирования
void lptim1_clk_notice(void);

// Инициализация таймера LPTIM1
void lptim1_init(void);

#endif // __LPTIM1_H
