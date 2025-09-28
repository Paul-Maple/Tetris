#ifndef __EVENT_H
#define __EVENT_H

#include <list.h>

// �������� �allback �������
typedef void (*event_cb_ptr) (void);

// ��������� �������
typedef struct
{
    // ������� ������
    list_item_t item;
    // ���������� �������
    event_cb_ptr cb;
} event_t;

// ������������� �������
void event_init(event_t *event, event_cb_ptr cb);

// ��������� �������
void event_raise(event_t *event);

// ��������� �������
void event_process(void);

#endif // __EVENT_H
