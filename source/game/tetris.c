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
#define TETRIS_FIGURE_NUMBER        5
// Количество цветов фигур
#define TETRIS_FIGURE_COLORS_NUMBER     8

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
static uint16_t tetris_figures[TETRIS_FIGURE_NUMBER] = 
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

// Прототип функции для отрисовки фигуры
typedef void (* draw_figure_cb) (void);

// Структура текущей фигуры для отрисовки
typedef struct
{
    // Ширина фигуры
    uint8_t width;
    // Высота фигуры
    uint8_t height;  
    // Цвет фигуры
    uint16_t color;
    // Тип фигуры
    uint8_t type;
    // Функция отрисовки фигуры
    draw_figure_cb draw;
    // Текущие координаты фигуры
    uint16_t position_x;
    uint16_t position_y;
    
} tetris_figure_t;

// Текущая активная фигура
tetris_figure_t tetris_figure;

// Cмещение фигуры по осям
static int8_t tetris_offset_x = 0;

// Постоянное смещение фигуры по таймеру
#define TETRIS_TIMER_FIGURE_OFFSET      1
// Cмещение фигуры по кнопке
#define TETRIS_BUTTON_FIGURE_OFFSET_X     20
#define TETRIS_BUTTON_FIGURE_OFFSET_Y     1

// Флаг необходимости выбора новой фигуры
static bool tetris_get_new_figure_needed = true;

// Проверка коллизий фигуры с боковыми границами поля
static bool tetris_check_collision_x(void)
{
    // Флаг коллизии
    bool collision_flag = false;
    
    // Если произошла коллизия с боковыми границами игрового поля
    if ((tetris_figure.position_x + tetris_figure.width + tetris_offset_x > 169) ||
        (tetris_figure.position_x + tetris_offset_x < 9))
        // Коллизия произошла - выход
        collision_flag = true;
    
    // Обнулить значение смещения
    tetris_offset_x = 0;
    
    return collision_flag;
}

// Проверка коллизий фигуры с нижней границей поля
static bool tetris_check_collision_y(void)
{
    // Флаг коллизии
    bool collision_flag = false;
    
    // Если произошла коллизия с нижней границей игрового поля
    if ((tetris_figure.position_y - TETRIS_BUTTON_FIGURE_OFFSET_Y) < 9)
    {
        // Установка флага необходимости создания новой фигуры
        tetris_get_new_figure_needed = true;
        // Обновить границы коллизии
        // tetris_update_collision_edge();
        // Коллизия произошла - выход
        collision_flag = true;
    }
    
    return collision_flag;
}

// Флаги состояний кнопок
static bool tetris_button_down_pressed  = false;
static bool tetris_button_right_pressed = false;
static bool tetris_button_left_pressed  = false;
//static bool tetris_button_rotate_pressed    = false;

// Предварительное объявление функции проверки состояния кнопок
static void tetris_check_offset_x(timer_t *timer);
static void tetris_check_offset_y(timer_t *timer);

// Таймер для проверки состояния кнопок смещения по оси X
static timer_t tetris_check_offset_x_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, tetris_check_offset_x);
// Таймер для проверки состояния кнопок смещения по оси Y
static timer_t tetris_check_offset_y_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, tetris_check_offset_y);

// Проверка состояния кнопок
static void tetris_check_offset_x(timer_t *timer)
{
    // Обработка движения ВПРАВО
    if (tetris_button_right_pressed && !tetris_button_left_pressed)
    {
        tetris_offset_x += TETRIS_BUTTON_FIGURE_OFFSET_X;
        if (!tetris_check_collision_x())
        {
            tetris_figure.position_x += TETRIS_BUTTON_FIGURE_OFFSET_X;
            tetris_figure.draw();
        }
    }
    
    // Обработка движения ВЛЕВО
    if (tetris_button_left_pressed && !tetris_button_right_pressed)
    {
        tetris_offset_x -= TETRIS_BUTTON_FIGURE_OFFSET_X;
        if (!tetris_check_collision_x())
        {
            tetris_figure.position_x -= TETRIS_BUTTON_FIGURE_OFFSET_X;
            tetris_figure.draw();
        }
    }
    
    // Если какая-то из кнопок ещё нажата (кроме противоположных направлений)
    if ((tetris_button_right_pressed && !tetris_button_left_pressed) ||
        (tetris_button_left_pressed && !tetris_button_right_pressed))
            // Запуск таймера для задержки обработки непрерывно нажатой кнопки
            timer_start(&tetris_check_offset_x_timer, TIMER_TICKS_MS(150));
}

// Проверка состояния кнопок
static void tetris_check_offset_y(timer_t *timer)
{
    // Обработка движения ВНИЗ
    if (tetris_button_down_pressed)
    {
        if (!tetris_check_collision_y())
        {
            tetris_figure.position_y -= TETRIS_BUTTON_FIGURE_OFFSET_Y;
            tetris_figure.draw();
        }
        
        // Запуск таймера для задержки обработки непрерывно нажатой кнопки
        timer_start(&tetris_check_offset_y_timer, TIMER_TICKS_MS(5));
    }
}

void tetris_button_pressed_notice(button_name_t name, bool state)
{
    // Изменение состояния кнопок
    switch (name)
    {/*
        case BUTTON_NAME_ROTATE:
            tetris_button_rotate_pressed = state;
            break;
       */ 
        case BUTTON_NAME_DOWN:
            tetris_button_down_pressed = state;
            break;
        
        case BUTTON_NAME_RIGHT:
            tetris_button_right_pressed = state;
            break;
        
        case BUTTON_NAME_LEFT:
            tetris_button_left_pressed = state;
            break;
        
        default:
            assert(false);
    }
    
    // Вызов обработки состояния кнопок и дальнейшей проверки коллизий
    tetris_check_offset_x(&tetris_check_offset_x_timer);
    tetris_check_offset_y(&tetris_check_offset_y_timer);
}

// Предварительное объявление функций для отрисовки
static void tetris_draw_figure_o(void);
//static void tetris_draw_figure_i(void);
//static void tetris_draw_figure_l(void);
//static void tetris_draw_figure_z(void);
//static void tetris_draw_figure_t(void);

// Координаты фигуры до смещения
static uint16_t tetris_previous_x = 0;
static uint16_t tetris_previous_y = 0;

// Функция для отрисовки фигуры по таймеру
static void tetris_draw_figure(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    /*
    // Смещение по таймеру
    tetris_offset_y -= TETRIS_TIMER_FIGURE_OFFSET;
    // Если после смещения нет коллизий - смещаем фигуру
    if (!tetris_check_collision_y())
        // Увеличить координату для смещения фигуры вниз на 1 пиксель
        tetris_figure.position_y -= TETRIS_TIMER_FIGURE_OFFSET;
    */
    // Если нужна новая фигура - выбор фигуры и её цвета
    if (tetris_get_new_figure_needed)
    {
        // Выбор типа новой фигуры
        tetris_figure.type = tetris_figures[rng_get_number(TETRIS_FIGURE_NUMBER - 1)];
        // Выбор нового цвета
        tetris_figure.color = tetris_colors[rng_get_number(TETRIS_FIGURE_COLORS_NUMBER - 1)];
        
        switch (tetris_figure.type)
        {
            case TETRIS_FIGURE_TYPE_O:
                tetris_figure.width = 20;
                tetris_figure.height = 20;
                tetris_figure.draw = tetris_draw_figure_o;
                break;
            
            case TETRIS_FIGURE_TYPE_L:
                tetris_figure.width = 20;
                tetris_figure.height = 60;
                tetris_figure.draw = tetris_draw_figure_o;
                break;
            
            case TETRIS_FIGURE_TYPE_Z:
                tetris_figure.width = 40;
                tetris_figure.height = 20;
                tetris_figure.draw = tetris_draw_figure_o;
                break;
            
            case TETRIS_FIGURE_TYPE_I:
                tetris_figure.width = 20;
                tetris_figure.height = 80;
                tetris_figure.draw = tetris_draw_figure_o;
                break;
            
            case TETRIS_FIGURE_TYPE_T:
                tetris_figure.width = 40;
                tetris_figure.height = 40;
                tetris_figure.draw = tetris_draw_figure_o;
                break;
            
            default:
                (assert(false));
        }
        
        // Сброс флага необходимости создания новой фигуры
        tetris_get_new_figure_needed = false;
        
        // Установка начального положения новой фигуры
        tetris_previous_x = tetris_figure.position_x = 89;
        tetris_previous_y = tetris_figure.position_y = 209 - tetris_figure.height; // 308
    }
    
    // Отрисовка фигуры
    tetris_figure.draw();
}

// Отрисовка фигуры КУБ 20х20
static void tetris_draw_figure_o(void)
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;
    
    // Очистить область где фигура находилась до сдвига
    figure.x1 = tetris_previous_x;
    figure.x2 = figure.x1 + tetris_figure.width;
    figure.y1 = tetris_previous_y;
    figure.y2 = figure.y1 + tetris_figure.height;
    lcd_draw_image(figure, LCD_COLOR_WHITE);
    
    // Отрисовка фигуры с учётом нового смещения
    figure.x1 = tetris_figure.position_x;
    figure.x2 = figure.x1 + tetris_figure.width;
    figure.y1 = tetris_figure.position_y;
    figure.y2 = figure.y1 + tetris_figure.height;
    lcd_draw_image(figure, tetris_figure.color);
    
    // Обновить предыдущие координаты фигуры
    tetris_previous_x = tetris_figure.position_x;
    tetris_previous_y = tetris_figure.position_y;    
    // Обнулить значения смещений
    //tetris_offset_x = 0;
    //tetris_offset_y = 0;
}
/*
static void tetris_draw_figure_l(void)
{
    
}

static void tetris_draw_figure_z(void)
{
    
}

static void tetris_draw_figure_i(void)
{
    
}

static void tetris_draw_figure_t(void)
{
    
}
*/
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
    // Поле для отрисовки счёта
    field.x1 = 189;
    field.x2 = 219;
    field.y1 = 269;
    field.y2 = 309;
    lcd_draw_image(field, LCD_COLOR_WHITE);
}
