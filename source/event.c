#include "event.h"
#include "mcu.h"

// ������ �������
static list_t event_list = 
{
    .head = NULL,
    .last = NULL,
};

// ������������� �������
void event_init(event_t *event, event_cb_ptr cb)
{
    ASSERT_NULL_PTR(event);
    ASSERT_NULL_PTR(cb);
    
    event->cb = cb;
    list_item_init(&event->item);
}

// ��������� �������
void event_raise(event_t *event)
{
    ASSERT_NULL_PTR(event);
    ASSERT_NULL_PTR(event->cb);
    
    MCU_IQR_SAFE_ENTER();
        
        // ���� ������� ��� � ������ - ����� �����
        if (list_contains(&event_list, &event->item))
        {
            MCU_IQR_SAFE_LEAVE();
            return;
        }
        
        // �������� ������� � ������� �� ���������
        list_insert(&event_list, &event->item);
        
    MCU_IQR_SAFE_LEAVE();
}

void event_process(void)
{
    MCU_IQR_DISABLE();
    
        // ��������� �������
        while(!list_empty(&event_list))
        {
            // �������� ������ ������� ������
            event_t * const temp_event = (event_t *)event_list.head;
            
            ASSERT_NULL_PTR(temp_event);
            ASSERT_NULL_PTR(temp_event->cb);
            
            // ����� �������
            MCU_IQR_ENABLE();
                temp_event->cb();
            MCU_IQR_DISABLE();
                
            // ������� ������� �� ������
            list_remove(&event_list, &temp_event->item);
        }
        
        MCU_WFI_ENTER();       
    MCU_IQR_ENABLE();
}
