#ifndef __TIMER_H
#define __TIMER_H

#include <list.h>
#include "mcu.h"

// Инитервал тиков таймера
typedef uint32_t timer_interval_t;

// Частота тактирования аппаратного таймера [Гц]
#define TIMER_CLK       MCU_LSE_FREQ

// Перевод мкСек в тики 
#define TIMER_TICKS_US(us)      ((timer_interval_t)MCU_US_TO_TIC(us, TIMER_CLK))
// Перевод мСек в тики 
#define TIMER_TICKS_MS(ms)      TIMER_TICKS_US((ms) * 1000.0)
// Перевод Сек в тики
#define TIMER_TICKS_SEC(sec)    TIMER_TICKS_MS((sec) * 1000.0)

// Статическая инициализация таймера
#define TIMER_STATIC_INIT(_mode, cb)                                            \
{                                                                               \
    LIST_ITEM_STATIC_INIT,                                                      \
    .init.mode = _mode,                                                         \
    .init.handler = cb,                                                         \
    .data.reload = 0,                                                           \
    .data.rasied = false                                                        \
}

// Инициализация модуля (Иициализируются аппаратные таймеры и событие)
void timer_module_init(void);

// Оповещение модуля lptim1 о смене источника тактирования
void timer_clk_notice(void);

// Предварительное объявление структуры таймера
typedef struct timer_s timer_t;

// Прототип обработчика таймера
typedef void (*timer_handler_ptr) (timer_t *timer);

// Перечисление режимов работы таймера
typedef enum
{
    // Циклический режим
    TIMER_MODE_CONTINUOUS,
    // Одиночный режим
    TIMER_MODE_ONE_SHOT
} timer_mode_t;

// Структура таймера
typedef struct timer_s
{
    // Элемент списка 
    list_item_t item;
    
    // Данные инициализации 
    struct
    {
        // Режим работы 
        timer_mode_t mode;
        // Обработчик события таймера
        timer_handler_ptr handler;
    } init;
    
    // Данные для обработки 
    struct
    {
        // Интервал до перезагрузки 
        uint32_t reload;
        // Интервал до срабатывания
        uint32_t remain;
        // Флаг срабатывания таймера 
        bool rasied;
    } data;
    
} timer_s;

// Инициализация программного таймера
/* Вызывется один раз для каждого программного таймера в том модуле, где он используется */
void timer_init(timer_t *timer, timer_mode_t mode, timer_handler_ptr handler);

// Запуск программного таймера
void timer_start(timer_t *timer, uint32_t ticks);

// Остановка программного таймера
void timer_stop(timer_t *timer);

// Обработка сработавших таймеров
void timer_processing_raised(void);

// Обработчик прерывания аппаратного таймера LPTIM1
void timer_lptim1_isr(void);

#endif // __TIMER_H
