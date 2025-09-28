#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "typedefs.h"

// Структура кольцевого буфера
typedef struct
{
    uint16_t data;          // Данные
    uint16_t *haed;         // Указатель на начало записи
    uint16_t *tail;         // Указатель на начало чтения
    uint8_t size;           // Размер буфера в данный момент
} ring_buffer_t;

// Инициализация буфера
void ring_buffer_init(ring_buffer_t *buffer);

    /*** Функции взаимодействия с буфером ***/
// Возвращает признак пустого буфера 
bool ring_buffer_is_empty(const ring_buffer_t *buffer);

// Возвращает признак переполненного буфера
bool ring_buffer_is_overflow(const ring_buffer_t *buffer);

// Записывает данные в буфер
void ring_buffer_write(ring_buffer_t *ring_buffer, const uint16_t data);

// Считывает данные из буфера
void ring_buffer_read(const ring_buffer_t *ring_buffer);

#endif // __RING_BUFFER_H
