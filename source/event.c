#include "event.h"
#include "mcu.h"

// Список событий
static list_t event_list = 
{
    .head = NULL,
    .last = NULL,
};

// Инициализация события
void event_init(event_t *event, event_cb_ptr cb)
{
    ASSERT_NULL_PTR(event);
    ASSERT_NULL_PTR(cb);
    
    event->cb = cb;
    list_item_init(&event->item);
}

// Генерация события
void event_raise(event_t *event)
{
    ASSERT_NULL_PTR(event);
    ASSERT_NULL_PTR(event->cb);
    
    MCU_IQR_SAFE_ENTER();
        
        // Если событие уже в списке - выход сразу
        if (list_contains(&event_list, &event->item))
        {
            MCU_IQR_SAFE_LEAVE();
            return;
        }
        
        // Добавить событие в очередь на обработку
        list_insert(&event_list, &event->item);
        
    MCU_IQR_SAFE_LEAVE();
}

void event_process(void)
{
    MCU_IQR_DISABLE();
    
        // Обработка очереди
        while(!list_empty(&event_list))
        {
            // Получаем первый элемент списка
            event_t * const temp_event = (event_t *)event_list.head;
            
            ASSERT_NULL_PTR(temp_event);
            ASSERT_NULL_PTR(temp_event->cb);
            
            // Вызов события
            MCU_IQR_ENABLE();
                temp_event->cb();
            MCU_IQR_DISABLE();
                
            // Удаляем событие из списка
            list_remove(&event_list, &temp_event->item);
        }
        
        MCU_WFI_ENTER();       
    MCU_IQR_ENABLE();
}
