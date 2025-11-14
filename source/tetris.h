#ifndef __TETRIS_H
#define __TETRIS_H

#include "key.h"

// Инициализация модуля
void tetris_init(void);

// Оповещение модуля о нажатии кнопки
void tetris_key_notice(key_name_t kay, bool state);

#endif // __TETRIS_H
