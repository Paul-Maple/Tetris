#include "timer.h"
#include "event.h"
#include "nvic.h"
#include "clk.h"

    /*** Для расчёта тиков и интервалов таймера ***/
// Регистр аппаратного таймера
typedef uint16_t timer_register_t;

// Минимальное время завода аппаратного таймера
#define TIMER_INTERVAL_MIN      10

// Максимальное значение регистра аппаратного таймера (uint16_t)
#define TIMER_REGISTER_MAX      ((timer_register_t)-1)

// Верхнее значение интервала тиков таймера (uint32_t)
#define TIMER_INTERVAL_TOP      ((timer_interval_t)-1)

// Значение интервала переполнения таймера
#define TIMER_INTERVAL_OVF      (TIMER_INTERVAL_TOP - (timer_interval_t)TIMER_REGISTER_MAX)

    /*** Аппаратный таймер LPTIM1 ***/     
// Оповещение модуля led о смене источника тактирования
void timer_clk_notice(void)
{
    // Сбросить источник тактирования таймера
    RCC->CCIPR &= ~(RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1); 
    
    switch (clk_lf_src)
    {
        case CLK_LF_SRC_LSI:
            // Выбор LSI в качестве источника тактирования для таймера LPTIM1
            RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0;
            break;
            
        case CLK_LF_SRC_LSE:
            // Выбор LSE в качестве источника тактирования для таймера LPTIM1
            RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1;
            break;
            
        default:
            assert(false);
    }
}

    /*** Инициализация модуля и таймеров ***/
// Событие обработки таймеров
static event_t timer_event_raise;

// Список таймеров
static list_t timer_list = 
{
    .head = NULL,
    .last = NULL,
};

void timer_module_init(void)
{
    // Инициализация события обработки таймеров
    event_init(&timer_event_raise, timer_processing_raised);
    
    // Инициализация аппаратного таймера LPTIM1
    {
        // Выбор LSI в качестве источника тактирования 
        timer_clk_notice();
        
        /* CFGR и IER могут быть установлены ТОЛЬКО когда установлен LPTIM1_CR_ENABLE = 0          *
         * ARR, SNGSTRT и CMP могут быть установлены ТОЛЬКО когда установлен LPTIM1_CR_ENABLE = 1  *
         * ПОЭТОМУ ПОРЯДОК ЗАПИСИ В РЕГИСТРЫ ТАЙМЕРА НЕ МЕНЯТЬ !!!!!!!                             */
        
        LPTIM1->CFGR &=  ~(LPTIM_CFGR_PRESC_0 | LPTIM_CFGR_PRESC_1 | LPTIM_CFGR_PRESC_2);       // Устновка делителя
        LPTIM1->IER   =  LPTIM_IER_ARRMIE | LPTIM_IER_CMPMIE;                                   // Разрешить прерывание по совпадению с ARR или CMP
        LPTIM1->CR    =  LPTIM_CR_ENABLE;                                                       // Запустить таймер
        LPTIM1->ARR   =  (timer_register_t)-1;                                                  // Ограничение счёта
        LPTIM1->CR   |=  LPTIM_CR_CNTSTRT;                                                      // Режим непрерывного счёта 
        
        // Разрешить прерывания у таймера
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

    /*** Запуск/Остановка программных таймеров ***/
// Флаг остановки таймера
static bool timer_was_stopped = false;
// Предыдущее значение регистра аппаратного таймера
static uint16_t timer_register_last = 0;

// Получает актуальное значение регистра счётчика 
static uint32_t timer_counter_get(volatile const uint32_t *reg)
{
    uint32_t counter;
    
    do 
    {
        counter = *reg;
    } while (counter != *reg);
    
    return counter;
}

// Установка значения регистра сравнения аппаратного таймера
static void timer_compare_set(uint16_t value)
{
    LPTIM1->CMP = value;
}

// Нормализация значения интервала к минимальному
static uint32_t timer_interval_normalize(uint32_t interval)
{
    return interval < TIMER_INTERVAL_MIN ? 
    TIMER_INTERVAL_MIN : interval;
}

void timer_start(timer_t *timer, timer_interval_t ticks)
{
    ASSERT_NULL_PTR(timer);
    assert(ticks < TIMER_INTERVAL_OVF);
    
    ticks = timer_interval_normalize(ticks);
    
    if (!list_contains(&timer_list, &timer->item))
        list_insert(&timer_list, &timer->item);
    
    timer->data.reload = ticks;
    timer->data.rasied = false;
    
    // Установка задержки до срабатывания 
    {
        const uint32_t time = timer_counter_get(&LPTIM1->CNT);
        const uint32_t delta = time - timer_register_last;    
        
        // Задание значения в регистр сравнения аппаратного таймера
        timer_compare_set(time + TIMER_INTERVAL_MIN);
        
        // Установка времени до срабатывания
        timer->data.remain = ticks + delta;
    }
}

void timer_stop(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Если таймер не запущен - выход сразу
    if (!list_contains(&timer_list, &timer->item))
        return;
    
    // Признак удаления не посленего таймера
    timer_was_stopped |= timer->item.next != NULL;
    
    // Удаление таймера из списка
    list_remove(&timer_list, &timer->item);
    // Сброс флага срабатывания таймера
    timer->data.rasied = false;
}

    /*** Обрабока таймеров ***/
// Проверяет, что таймер не активен
static bool timer_inactive(const timer_t *timer)
{
    return timer->data.remain == 0 || timer->data.remain >= TIMER_INTERVAL_OVF;
}

// Обработка списка таймеров
/* Вызывается в прерывании аппаратного таймера.         *
 * Возвращает, нужна ли обработка сработавших таймеров  */
static bool timer_processing(void)
{
    // Флаг наличия сработавших таймеров
    bool event_raise = false;
    
    uint16_t time_min = TIMER_REGISTER_MAX;
    uint16_t time_min_reg;
    
    // Определение прошедшего времени
    uint16_t time_delta = timer_counter_get(&LPTIM1->CNT);
    time_delta -= timer_register_last;
    timer_register_last += time_delta;
    
    // Обход списка таймеров
    for (list_item_t *temp_item = timer_list.head; temp_item != NULL; temp_item = temp_item->next)
    {
        // Текущий таймер (приведение item к timer)
        timer_t * const timer = (timer_t *)temp_item;
        
        // Если таймер не активен - пропуск интерации
        if (timer_inactive(timer))
            continue;
        
        // Обновляем значение до срабатывания
        timer->data.remain -= time_delta;
        
        // Если после обновления remain таймер не активен
        if (timer_inactive(timer))
        {
            event_raise = true;
            timer->data.rasied = true;
            
            // Проверка режима работы таймера
            if (timer->init.mode == TIMER_MODE_ONE_SHOT)
                continue;
            
            // Обновление интервала 
            do
            {
                timer->data.remain += timer->data.reload;
            } while (timer_inactive(timer));  
        } 
        
        // Обновляем минимальное время срабатывания
        if (time_min > timer->data.remain)
            time_min = timer->data.remain;
    }
    
    // Ограничение на минимальное время следующего срабатывания 
    time_min_reg = timer_interval_normalize(time_min);
    // Установка нового значения срабатываня
    timer_compare_set(timer_counter_get(&LPTIM1->CNT) + time_min_reg);
    
    return event_raise;
}

// Обработка сработавших таймеров
/* Вызывается как callback внутри event_process() в случае, *
 * если событие обработки таймеров поставлено в очередь.    *   
 * Вызывает callback у сработавших таймеров.                */
void timer_processing_raised(void)
{
    // Выполнение цикла, пока не будет изменений списка таймеров
    do
    {
        timer_was_stopped = false;
        
        // Обход списка таймеров
        for (list_item_t *temp_item = timer_list.head; temp_item != NULL; temp_item = temp_item->next)
        {
            // Текущий таймер (приведение item_t к timer_t)
            timer_t * const timer = (timer_t *)temp_item;
            
            // Если текущий таймер ещё не сработал - переход к следующему сразу
            if (!timer->data.rasied)
                continue;
            
            // Сброс флага срабатывания таймера
            timer->data.rasied = false;
            
            // Если таймер не активен - удаление его из списка
            if(timer_inactive(timer))
                list_remove(&timer_list, &timer->item);
            
            // Вызов обработчика
            ASSERT_NULL_PTR(timer->init.handler);
            
            timer->init.handler(timer);
            
            // Если какой-то таймер был удалён - обход с начала
            if(timer_was_stopped)
                break;
        }
    } while (timer_was_stopped);
}

    /*** Обработчик прерывания программного таймера ***/
void timer_lptim1_isr(void)
{
    // Сброс флагов прерывания (При записи "1")
    LPTIM1->ICR |= LPTIM_ICR_ARRMCF | LPTIM_ICR_CMPMCF;
    
    // Если нужна обработка сработавших таймеров 
    if (timer_processing())
        // Добавить событие в очередь на обработку
        event_raise(&timer_event_raise); 
}
