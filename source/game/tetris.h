#ifndef __TETRIS_H
#define __TETRIS_H

#include <button.h>

// Инициализация модуля
void tetris_init(void);

// Оповещение модуля о нажатии кнопки
void tetris_button_pressed_notice(button_name_t name, bool state);

#endif // __TETRIS_H
