#include "tetris.h"
#include <timer.h>
#include <lcd.h>
#include <rng.h>

// Фигуры:
// L, O, Z, I, T

// Размеры игровой области c учётом рамки
#define TETRIS_FIELD_WIDTH      160U
#define TETRIS_FIELD_HEIGHT     300U

// Количество фигур
#define TETRIS_FIGURE_NUMBER        5U
// Количество цветов фигур
#define TETRIS_FIGURE_COLORS_NUMBER     8U

// Перечисление типов фигур
enum
{
    TETRIS_FIGURE_TYPE_O,
    TETRIS_FIGURE_TYPE_L,
    TETRIS_FIGURE_TYPE_Z,
    TETRIS_FIGURE_TYPE_I,
    TETRIS_FIGURE_TYPE_T
};

// Массив типов фигур
static uint16_t tetris_figure_type[TETRIS_FIGURE_NUMBER] = 
{
    TETRIS_FIGURE_TYPE_O,
    TETRIS_FIGURE_TYPE_L,
    TETRIS_FIGURE_TYPE_Z,
    TETRIS_FIGURE_TYPE_I,
    TETRIS_FIGURE_TYPE_T,
};

// Массив цветов фигур
static uint16_t tetris_colors[TETRIS_FIGURE_COLORS_NUMBER] = 
{
    LCD_COLOR_RED,
    LCD_COLOR_GREEN,
    LCD_COLOR_BLUE,
    LCD_COLOR_YELLOW,
    LCD_COLOR_ORANGE,
    LCD_COLOR_CYAN,
    LCD_COLOR_PURPLE,
    LCD_COLOR_PINK,
};

// Перечисление направлений смещения фигуры
typedef enum
{
    TETRIS_OFFSET_NON,
    TETRIS_OFFSET_DOWN,
    TETRIS_OFFSET_LEFT,
    TETRIS_OFFSET_RIGHT,
        
} tetris_offset_t;

// Тип смещения активной фигуры
tetris_offset_t tetris_offset_figure = TETRIS_OFFSET_NON;

// Структура текущей фигуры для отрисовки
typedef struct
{
    // Цвет фигуры
    uint16_t color;
    // Тип фигуры
    uint8_t type;
    // Текущие координаты фигуры
    uint16_t x;
    uint16_t y;
    // Размер фигуры
    uint8_t size_i;
    uint8_t size_j;
    
} tetris_figure_t;

// Текущая активная фигура
tetris_figure_t tetris_figure;

// Массив с формой  текущей фигуры фигуры
bool tetris_shape[5][5] = 
{
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
};

// Постоянное смещение фигуры по таймеру
#define TETRIS_TIMER_FIGURE_OFFSET      1
// Cмещение фигуры по кнопке
#define TETRIS_BUTTON_FIGURE_OFFSET_Y     TETRIS_TIMER_FIGURE_OFFSET
#define TETRIS_BUTTON_FIGURE_OFFSET_X     10

// Флаг необходимости выбора новой фигуры
static bool tetris_get_new_figure_needed = true;

// Предварительное объявление функции проверки состояния кнопок
static void tetris_offset(timer_t *timer);

// Таймер для проверки состояния кнопок смещения
static timer_t tetris_check_offset_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, tetris_offset);

// Флаги состояний кнопок
static bool tetris_button_down_pressed   = false;
static bool tetris_button_right_pressed  = false;
static bool tetris_button_left_pressed   = false;
static bool tetris_button_rotate_pressed = false;

// Проверка коллизий фигуры с нижней границей
static bool tetris_check_collision(tetris_offset_t offset)
{
    // Цвет пикселя для чтения
    uint16_t color;
    // Координаты пикселя для чтения из памяти LCD
    lcd_position_t pixel;
    
    switch(offset)
    {
        case TETRIS_OFFSET_DOWN:
            pixel.x1 = tetris_figure.x;
            pixel.y1 = tetris_figure.y - 1;
            break;
        
        case TETRIS_OFFSET_RIGHT:
            pixel.x1 = tetris_figure.x + 10;
            pixel.y1 = tetris_figure.y;
            break;
        
        case TETRIS_OFFSET_LEFT:
            pixel.x1 = tetris_figure.x - 10;
            pixel.y1 = tetris_figure.y;
            break;
        
        case TETRIS_OFFSET_NON:
            (assert(false));
        
        default:
            (assert(false));
    }
    
    // Назначение координат одного пикселя
    pixel.x2 = pixel.x1;
    pixel.y2 = pixel.y1;
    // Чтение цвета соседнего пикселя
    lcd_read_color(pixel, &color);
    
    // Если произошла коллизия с нижней границей
    if ((color != LCD_COLOR_WHITE) && (offset == TETRIS_OFFSET_DOWN))
    {
        // Установка флага необходимости создания новой фигуры
        tetris_get_new_figure_needed = true;
        return true;
    }
    
    // Если произошла коллизия с боковыми границами
    if ((color != LCD_COLOR_WHITE) && ((offset == TETRIS_OFFSET_LEFT) | (offset == TETRIS_OFFSET_RIGHT)))
        return true;
    
    return false;
}

// Отрисовка одного куба фигуры 20х20
static void tetris_draw_cube(uint16_t offset_x, uint16_t offset_y, uint16_t color)
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;

    // Цветной куб 
    figure.x1 = tetris_figure.x + offset_x + 1;
    figure.x2 = figure.x1 + 8;
    figure.y1 = tetris_figure.y + offset_y + 1;
    figure.y2 = figure.y1 + 8;
    // Отрисовка фигуры
    lcd_draw_image(figure, color);
}

// Переотрисовка фигуры с учётом смещения
static void tetris_redrawing_figure(tetris_offset_t offset)
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;
    
    // Если смещаем вниз
    if (offset == TETRIS_OFFSET_DOWN)
    {
        for (uint8_t i = 0; i < 5; i++)
        {
            for (uint8_t j = 0; j < 5; j++)
            {
                if (tetris_shape[i][j] == 1)
                {
                    // Координаты смещения по осям
                    uint16_t offset_x = tetris_figure.x + i*10;
                    uint16_t offset_y = tetris_figure.y + j*10;
                    
                    // Очистить верхнюю полосу
                    figure.x1 = offset_x;
                    figure.x2 = offset_x + 10;
                    figure.y1 = figure.y2 = offset_y + 9;
                    lcd_draw_image(figure, LCD_COLOR_WHITE);
                    
                    // Цветная полоcа внизу 
                    figure.x1 = offset_x + 1;
                    figure.x2 = offset_x + 9;
                    figure.y1 = offset_y;
                    figure.y2 = offset_y ;
                    lcd_draw_image(figure, tetris_figure.color);
                }
            }
        }
        // Изменяем координату фигуры
        tetris_figure.y -= TETRIS_BUTTON_FIGURE_OFFSET_Y;
        return;
    }

    // Если смещаем вправо
    if (offset == TETRIS_OFFSET_RIGHT)
    {
        for (int8_t i = tetris_figure.size_i; i >= 0;  i--)
        {
            for (uint8_t j = 0; j < tetris_figure.size_j; j++)
            {  
                if (tetris_shape[i][j] == 1)
                {
                    // Очищаем область
                    tetris_draw_cube(i * 10, j * 10, LCD_COLOR_WHITE);
                    // Вызов отрисовки одного куба фигуры
                    tetris_draw_cube((i + 1) * 10, j * 10, tetris_figure.color);
                }  
            }
        }
        
        // Изменяем координату фигуры
        tetris_figure.x += TETRIS_BUTTON_FIGURE_OFFSET_X;
        return;
    }
    
    // Если смещаем влево
    if (offset == TETRIS_OFFSET_LEFT)
    {
        // Изменяем координату фигуры
        tetris_figure.x -= TETRIS_BUTTON_FIGURE_OFFSET_X;
        
        for (uint8_t i = 0; i < tetris_figure.size_i;  i++)
        {
            for (uint8_t j = 0; j < tetris_figure.size_j; j++)
            {  
                if (tetris_shape[i][j] == 1)
                {
                    // Очищаем область
                    tetris_draw_cube((i + 1) * 10, j * 10, LCD_COLOR_WHITE);
                    // Вызов отрисовки одного куба фигуры
                    tetris_draw_cube(i * 10, j * 10, tetris_figure.color);
                }  
            }
        }
        
        return;
    }
}

// Смещение фигуры по осям с учётом коллизий
static void tetris_offset(timer_t *timer)
{
    // Ассерт на нулевой указатель таймера
    ASSERT_NULL_PTR(timer);
    
    // Если кнопка ВНИЗ нажата
    if (tetris_button_down_pressed)
    {
        // Если не произошла коллизия
        //if (!tetris_check_collision(TETRIS_OFFSET_DOWN))
        {
            // Перерисовываем фигуру
            tetris_redrawing_figure(TETRIS_OFFSET_DOWN);
            // Запуск таймера для задержки обработки непрерывно нажатой кнопки
            timer_start(&tetris_check_offset_timer, TIMER_TICKS_MS(5));
        }
    }
    
    // Если нажаты обе кнопки - выход без обработки их одновременного нажатия
    if (tetris_button_left_pressed & tetris_button_right_pressed)
        return;
    
    // Если кнопка ВЛЕВО нажата
    if (tetris_button_left_pressed)
    {
        // Если не произошла коллизия
        //if (!tetris_check_collision(TETRIS_OFFSET_LEFT))
        {
            // Перерисовываем фигуру
            tetris_redrawing_figure(TETRIS_OFFSET_LEFT);
            // Запуск таймера для задержки обработки непрерывно нажатой кнопки
            timer_start(&tetris_check_offset_timer, TIMER_TICKS_MS(120));
        }
    }
    
    // Если кнопка ВПРАВО нажата
    if (tetris_button_right_pressed)
    {
        // Если не произошла коллизия
        //if (!tetris_check_collision(TETRIS_OFFSET_RIGHT))
        {
            // Перерисовываем фигуру
            tetris_redrawing_figure(TETRIS_OFFSET_RIGHT);
            // Запуск таймера для задержки обработки непрерывно нажатой кнопки
            timer_start(&tetris_check_offset_timer, TIMER_TICKS_MS(120));
        }
    }
}

// Вращение фигуры
static void tetris_rotate_figure(void)
{
    // Если кнопка не нажата - выход сразу
    if (!tetris_button_rotate_pressed)
        return;
    /*
    // Если при повороте произошла коллизия - выход без поворота фигуры
    if (tetris_check_collision(TETRIS_OFFSET_ROTATE))
        return;
    */
    // Временный массив формы фигуры
    bool temp_shape[5][5];
    
    // Копируем значения во временный массив и очищаем старую фигуру
    for (uint8_t i = 0; i < 5; i++)
    {
        for (uint8_t j = 0; j < 5; j++)
        {
            // Копирование
            temp_shape[i][j] = tetris_shape[i][j];
            // Очистка
            if (temp_shape[i][j] == 1)
                tetris_draw_cube(i * 10, j * 10, LCD_COLOR_WHITE);
        }
    }
    
    // Определяем размер для поворота (используем максимальный размер)
    uint8_t size = (tetris_figure.size_i > tetris_figure.size_j) ? tetris_figure.size_i : tetris_figure.size_j;    
    
    // Поворачиваем фигуру на 90 градусов по часовой стрелке
    for (uint8_t i = 0; i < size; i++)
    {
        for (uint8_t j = 0; j < size; j++)
        {
            tetris_shape[j][size - 1 - i] = temp_shape[i][j];
            
            // Перерисовываем повернутую фигуру
            if (tetris_shape[j][size - 1 - i] == 1)
            {
                tetris_draw_cube(j * 10, (size - 1 - i) * 10, tetris_figure.color);
            }
        }
    }
    
    // Обновляем размеры после поворота
    uint8_t temp_size = tetris_figure.size_i;
    tetris_figure.size_i = tetris_figure.size_j;
    tetris_figure.size_j = temp_size;
}

void tetris_button_pressed_notice(button_name_t name, bool state)
{
    // Изменение состояния кнопок
    switch (name)
    {
        case BUTTON_NAME_ROTATE:
            tetris_button_rotate_pressed = state;
            tetris_rotate_figure();
            break;
        
        case BUTTON_NAME_DOWN:
            tetris_button_down_pressed = state;
            tetris_offset(&tetris_check_offset_timer);
            break;
        
        case BUTTON_NAME_RIGHT:
            tetris_button_right_pressed = state;
            tetris_offset(&tetris_check_offset_timer);
            break;
        
        case BUTTON_NAME_LEFT:
            tetris_button_left_pressed = state;
            tetris_offset(&tetris_check_offset_timer);
            break;
        
        default:
            assert(false);
    }
}
/*
массив[строка][столбец]
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
*/
// Получение формы фигуры
static void tetris_get_shape(uint8_t type)
{
    // Обнуление массива с формой фигуры
    for (uint8_t i = 0; i < 5; i++)
        for (uint8_t j = 0; j < 5; j++)
            tetris_shape[i][j] = 0;
    
    // Присвоение формы
    switch(type)
    {
        case TETRIS_FIGURE_TYPE_O:
            tetris_figure.size_i = 2;
            tetris_figure.size_j = 2;
            tetris_shape[0][0] = 1;
            tetris_shape[0][1] = 1;
            tetris_shape[1][0] = 1;
            tetris_shape[1][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_Z:
            tetris_figure.size_i = 3;
            tetris_figure.size_j = 2;
            tetris_shape[0][0] = 1;
            tetris_shape[1][0] = 1;
            tetris_shape[1][1] = 1;
            tetris_shape[2][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_L:
            tetris_figure.size_i = 3;
            tetris_figure.size_j = 3;
            tetris_shape[0][1] = 1;
            tetris_shape[1][1] = 1;
            tetris_shape[2][1] = 1;
            tetris_shape[2][2] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_T:
            tetris_figure.size_i = 3;
            tetris_figure.size_j = 2;
            tetris_shape[0][1] = 1;
            tetris_shape[1][1] = 1;
            tetris_shape[2][1] = 1;
            tetris_shape[1][0] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_I:
            tetris_figure.size_i = 5;
            tetris_figure.size_j = 5;
            tetris_shape[0][2] = 1;
            tetris_shape[1][2] = 1;
            tetris_shape[2][2] = 1;
            tetris_shape[3][2] = 1;
            break;
            
        default:
            (assert(false));
    }
}
// Функция для отрисовки фигуры по таймеру
static void tetris_draw_figure(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    /*
    // Если после смещения нет коллизий - смещаем фигуру
    if (!tetris_check_collision_y(TETRIS_TIMER_FIGURE_OFFSET))
        // Увеличить координату для смещения фигуры вниз на 1 пиксель
        tetris_figure.y -= TETRIS_TIMER_FIGURE_OFFSET;
     */
    // Если нужна новая фигура - выбор фигуры и её цвета
    if (tetris_get_new_figure_needed)
    {
        // Выбор типа новой фигуры
        tetris_figure.type = tetris_figure_type[rng_get_number(TETRIS_FIGURE_NUMBER - 1)];
        // Выбор нового цвета
        tetris_figure.color = tetris_colors[rng_get_number(TETRIS_FIGURE_COLORS_NUMBER - 1)];
        
        // Присвоить форму фигуры
        switch (tetris_figure.type)
        {
            case TETRIS_FIGURE_TYPE_O:
                tetris_get_shape(TETRIS_FIGURE_TYPE_O);
                break;
            
            case TETRIS_FIGURE_TYPE_L:
                tetris_get_shape(TETRIS_FIGURE_TYPE_L);
                break;
            
            case TETRIS_FIGURE_TYPE_Z:
                tetris_get_shape(TETRIS_FIGURE_TYPE_Z);
                break;
            
            case TETRIS_FIGURE_TYPE_I:
                tetris_get_shape(TETRIS_FIGURE_TYPE_I);
                break;
            
            case TETRIS_FIGURE_TYPE_T:
                tetris_get_shape(TETRIS_FIGURE_TYPE_T);
                break;
            
            default:
                (assert(false));
        }
        
        // Сброс флага необходимости создания новой фигуры
        tetris_get_new_figure_needed = false;
        
        // Установка начального положения новой фигуры
        tetris_figure.x = 69;
        tetris_figure.y = 209; // 308
        
        // Отрисовка всеех секций фигуры 
        /* Происходит один раз при создании новой фигуры            *
         * далее происходит только перерисовка изменяющихся частей, *
         * а не фигуры целиком                                      */
        for (uint8_t i = 0; i < tetris_figure.size_i; i++)
            for (uint8_t j = 0; j < tetris_figure.size_j; j++)
                if (tetris_shape[i][j] == 1)
                    // Вызов отрисовки одного куба фигуры
                    tetris_draw_cube(i * 10, j * 10, tetris_figure.color);
    }
}

// Таймер для непрерывной отрисовки движущейся фигуры
static timer_t tetris_timer = TIMER_STATIC_INIT(TIMER_MODE_CONTINUOUS, tetris_draw_figure);

void tetris_init(void)
{
    // Запуск таймера
    timer_start(&tetris_timer, TIMER_TICKS_MS(15));
    
    // Отрисовка игровой области
    lcd_position_t field;
    // Рамка
    field.x1 = 0;
    field.x2 = 239;
    field.y1 = 0;
    field.y2 = 319;
    lcd_draw_image(field, LCD_COLOR_GRAY);
    // Игровое поле
    field.x1 = 9;
    field.x2 = 169;
    field.y1 = 9;
    field.y2 = 309;
    lcd_draw_image(field, LCD_COLOR_WHITE);
    // Поле для счёта
    field.x1 = 189;
    field.x2 = 219;
    field.y1 = 269;
    field.y2 = 309;
    lcd_draw_image(field, LCD_COLOR_WHITE);
}
