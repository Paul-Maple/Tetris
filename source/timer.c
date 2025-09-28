#include "timer.h"
#include "event.h"
#include "nvic.h"
#include "clk.h"

    /*** ��� ������� ����� � ���������� ������� ***/
// ������� ����������� �������
typedef uint16_t timer_register_t;

// ����������� ����� ������ ����������� �������
#define TIMER_INTERVAL_MIN      10

// ������������ �������� �������� ����������� ������� (uint16_t)
#define TIMER_REGISTER_MAX      ((timer_register_t)-1)

// ������� �������� ��������� ����� ������� (uint32_t)
#define TIMER_INTERVAL_TOP      ((timer_interval_t)-1)

// �������� ��������� ������������ �������
#define TIMER_INTERVAL_OVF      (TIMER_INTERVAL_TOP - (timer_interval_t)TIMER_REGISTER_MAX)

    /*** ���������� ������ LPTIM1 ***/     
// ���������� ������ led � ����� ��������� ������������
void timer_clk_notice(void)
{
    // �������� �������� ������������ �������
    RCC->CCIPR &= ~(RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1); 
    
    switch (clk_lf_src)
    {
        case CLK_LF_SRC_LSI:
            // ����� LSI � �������� ��������� ������������ ��� ������� LPTIM1
            RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0;
            break;
            
        case CLK_LF_SRC_LSE:
            // ����� LSE � �������� ��������� ������������ ��� ������� LPTIM1
            RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1;
            break;
            
        default:
            assert(false);
    }
}

    /*** ������������� ������ � �������� ***/
// ������� ��������� ��������
static event_t timer_event_raise;

// ������ ��������
static list_t timer_list = 
{
    .head = NULL,
    .last = NULL,
};

void timer_module_init(void)
{
    // ������������� ������� ��������� ��������
    event_init(&timer_event_raise, timer_processing_raised);
    
    // ������������� ����������� ������� LPTIM1
    {
        // ����� LSI � �������� ��������� ������������ 
        timer_clk_notice();
        
        /* CFGR � IER ����� ���� ����������� ������ ����� ���������� LPTIM1_CR_ENABLE = 0          *
         * ARR, SNGSTRT � CMP ����� ���� ����������� ������ ����� ���������� LPTIM1_CR_ENABLE = 1  *
         * ������� ������� ������ � �������� ������� �� ������ !!!!!!!                             */
        
        LPTIM1->CFGR &=  ~(LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2);       // �������� ��������
        LPTIM1->IER   =  LPTIM_IER_ARRMIE | LPTIM_IER_CMPMIE;                                   // ��������� ���������� �� ���������� � ARR ��� CMP
        LPTIM1->CR    =  LPTIM_CR_ENABLE;                                                       // ��������� ������
        LPTIM1->ARR   =  (timer_register_t)-1;                                                  // ����������� �����
        LPTIM1->CR   |=  LPTIM_CR_CNTSTRT;                                                      // ����� ������������ ����� 
        
        // ��������� ���������� � �������
        nvic_irq_enable(LPTIM1_IRQn);
    }
}

void timer_init(timer_t *timer, timer_mode_t mode, timer_handler_ptr handler)
{
    ASSERT_NULL_PTR(timer);
    ASSERT_NULL_PTR(handler);
    
    list_item_init(&timer->item);
    
    timer->init.mode = mode;
    timer->init.handler = handler;
    
    timer->data.reload = 0;
    timer->data.rasied = false;
}

    /*** ������/��������� ����������� �������� ***/
// ���� ��������� �������
static bool timer_was_stopped = false;
// ���������� �������� �������� ����������� �������
static uint16_t timer_register_last = 0;

// �������� ���������� �������� �������� �������� 
static uint32_t timer_counter_get(volatile const uint32_t *reg)
{
    uint32_t counter;
    
    do 
    {
        counter = *reg;
    } while (counter != *reg);
    
    return counter;
}

// ��������� �������� �������� ��������� ����������� �������
static void timer_compare_set(uint16_t value)
{
    LPTIM1->CMP = value;
}

// ������������ �������� ��������� � ������������
static uint32_t timer_interval_normalize(uint32_t interval)
{
    return interval < TIMER_INTERVAL_MIN ? 
    TIMER_INTERVAL_MIN : interval;
}

void timer_start(timer_t *timer, uint32_t ticks)
{
    ASSERT_NULL_PTR(timer);
    assert(ticks < TIMER_INTERVAL_OVF);
    
    ticks = timer_interval_normalize(ticks);
    
    if (!list_contains(&timer_list, &timer->item))
        list_insert(&timer_list, &timer->item);
    
    timer->data.reload = ticks;
    timer->data.rasied = false;
    
    // ��������� �������� �� ������������ 
    {
        const uint32_t time = timer_counter_get(&LPTIM1->CNT);
        const uint32_t delta = time - timer_register_last;    
        
        // ������� �������� � ������� ��������� ����������� �������
        timer_compare_set(time + TIMER_INTERVAL_MIN);
        
        // ��������� ������� �� ������������
        timer->data.remain = ticks + delta;
    }
}

void timer_stop(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // ���� ������ �� ������� - ����� �����
    if (!list_contains(&timer_list, &timer->item))
        return;
    
    // ������� �������� �� ��������� �������
    timer_was_stopped |= timer->item.next != NULL;
    
    // �������� ������� �� ������
    list_remove(&timer_list, &timer->item);
    // ����� ����� ������������ �������
    timer->data.rasied = false;
}

    /*** �������� �������� ***/
// ���������, ��� ������ �� �������
static bool timer_inactive(const timer_t *timer)
{
    return timer->data.remain == 0 || timer->data.remain >= TIMER_INTERVAL_OVF;
}

// ��������� ������ ��������
/* ���������� � ���������� ����������� �������.         *
 * ����������, ����� �� ��������� ����������� ��������  */
static bool timer_processing(void)
{
    // ���� ������� ����������� ��������
    bool event_raise = false;
    
    uint16_t time_min = TIMER_REGISTER_MAX;
    uint16_t time_min_reg;
    
    // ����������� ���������� �������
    uint16_t time_delta = timer_counter_get(&LPTIM1->CNT);
    time_delta -= timer_register_last;
    timer_register_last += time_delta;
    
    // ����� ������ ��������
    for (list_item_t *temp_item = timer_list.head; temp_item != NULL; temp_item = temp_item->next)
    {
        // ������� ������ (���������� item � timer)
        timer_t * const timer = (timer_t *)temp_item;
        
        // ���� ������ �� ������� - ������� ���������
        if (timer_inactive(timer))
            continue;
        
        // ��������� �������� �� ������������
        timer->data.remain -= time_delta;
        
        // ���� ����� ���������� remain ������ �� �������
        if (timer_inactive(timer))
        {
            event_raise = true;
            timer->data.rasied = true;
            
            // �������� ������ ������ �������
            if (timer->init.mode == TIMER_MODE_ONE_SHOT)
                continue;
            
            // ���������� ��������� 
            do
            {
                timer->data.remain += timer->data.reload;
            } while (timer_inactive(timer));  
        } 
        
        // ��������� ����������� ����� ������������
        if (time_min > timer->data.remain)
            time_min = timer->data.remain;
    }
    
    // ����������� �� ����������� ����� ���������� ������������ 
    time_min_reg = timer_interval_normalize(time_min);
    // ��������� ������ �������� �����������
    timer_compare_set(timer_counter_get(&LPTIM1->CNT) + time_min_reg);
    
    return event_raise;
}

// ��������� ����������� ��������
/* ���������� ��� callback ������ event_process() � ������, *
 * ���� ������� ��������� �������� ���������� � �������.    *   
 * �������� callback � ����������� ��������.                */
void timer_processing_raised(void)
{
    // ���������� �����, ���� �� ����� ��������� ������ ��������
    do
    {
        timer_was_stopped = false;
        
        // ����� ������ ��������
        for (list_item_t *temp_item = timer_list.head; temp_item != NULL; temp_item = temp_item->next)
        {
            // ������� ������ (���������� item_t � timer_t)
            timer_t * const timer = (timer_t *)temp_item;
            
            // ���� ������� ������ ��� �� �������� - ������� � ���������� �����
            if (!timer->data.rasied)
                continue;
            
            // ����� ����� ������������ �������
            timer->data.rasied = false;
            
            // ���� ������ �� ������� - �������� ��� �� ������
            if(timer_inactive(timer))
                list_remove(&timer_list, &timer->item);
            
            // ����� �����������
            ASSERT_NULL_PTR(timer->init.handler);
            
            timer->init.handler(timer);
            
            // ���� �����-�� ������ ��� ����� - ����� � ������
            if(timer_was_stopped)
                break;
        }
    } while (timer_was_stopped);
}

    /*** ���������� ���������� ������������ ������� ***/
void timer_lptim1_isr(void)
{
    // ����� ������ ���������� (��� ������ "1")
    LPTIM1->ICR |= LPTIM_ICR_ARRMCF | LPTIM_ICR_CMPMCF;
    
    // ���� ����� ��������� ����������� �������� 
    if (timer_processing())
        // �������� ������� � ������� �� ���������
        event_raise(&timer_event_raise); 
}
