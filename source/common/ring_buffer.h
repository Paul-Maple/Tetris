#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "typedefs.h"

// ��������� ���������� ������
typedef struct
{
    uint16_t data;          // ������
    uint16_t *haed;         // ��������� �� ������ ������
    uint16_t *tail;         // ��������� �� ������ ������
    uint8_t size;           // ������ ������ � ������ ������
} ring_buffer_t;

// ������������� ������
void ring_buffer_init(ring_buffer_t *buffer);

    /*** ������� �������������� � ������� ***/
// ���������� ������� ������� ������ 
bool ring_buffer_is_empty(const ring_buffer_t *buffer);

// ���������� ������� �������������� ������
bool ring_buffer_is_overflow(const ring_buffer_t *buffer);

// ���������� ������ � �����
void ring_buffer_write(ring_buffer_t *ring_buffer, const uint16_t data);

// ��������� ������ �� ������
void ring_buffer_read(const ring_buffer_t *ring_buffer);

#endif // __RING_BUFFER_H
