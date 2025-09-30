#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H

// Стандарнтые
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

    /*** Типы данных ***/
typedef double      float64_t;

    /*** Макросы опций компилятора ***/
// Преобразование в строку значения макроса
#define STRINGIFY(x)    #x

// Опция компилятора как макрос
#define PRAGMA(msg)     _Pragma(STRINGIFY(msg))

// Установка опции компилятора через макрос
#define PRAGMA_OPTION(m, v)     PRAGMA(m = v)

    /*** Макросы опций линковщика ***/
// Секции линковщика
// #define SECTION_USED(name)      PRAGMA_OPTION(location, name)
#define SECTION_DECL(name)      PRAGMA_OPTION(section, name)

    /*** Макросы для ассертов ***/
// Проверка на нулевой указатель
#define ASSERT_NULL_PTR(sender)     (assert(sender != NULL))

    /*** Приведения ***/
// Приведение указателя на член структуры к указателю на структуру
#define CONTAINER_OF(ptr, type, member)                                         \
    ((type *)((uint8_t *)(ptr) - offsetof(type, member)))
        
#endif // __TYPEDEFS_H
