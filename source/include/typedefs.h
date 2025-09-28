#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H

// �����������
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

    /*** ���� ������ ***/
typedef double      float64_t;

    /*** ������� ����� ����������� ***/
// �������������� � ������ �������� �������
#define STRINGIFY(x)    #x

// ����� ����������� ��� ������
#define PRAGMA(msg)     _Pragma(STRINGIFY(msg))

// ��������� ����� ����������� ����� ������
#define PRAGMA_OPTION(m, v)     PRAGMA(m = v)

    /*** ������� ����� ���������� ***/
// ������ ����������
// #define SECTION_USED(name)      PRAGMA_OPTION(location, name)
#define SECTION_DECL(name)      PRAGMA_OPTION(section, name)

    /*** ������� ��� �������� ***/
// �������� �� ������� ���������
#define ASSERT_NULL_PTR(sender)     (assert(sender != NULL))

#endif // __TYPEDEFS_H
