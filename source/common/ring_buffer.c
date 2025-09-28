#include "ring_buffer.h"

void ring_buffer_init(ring_buffer_t *buffer)
{
    buffer->data = NULL;
    buffer->haed = NULL;
    buffer->tail = NULL;
    buffer->size = 0;
}

bool ring_buffer_is_empty(const ring_buffer_t *buffer)
{
    
}

bool ring_buffer_is_overflow(const ring_buffer_t *buffer)
{
    
}

void ring_buffer_write(ring_buffer_t *ring_buffer, const uint16_t data)
{
    
}

void ring_buffer_read(const ring_buffer_t *ring_buffer)
{
    
}
