#include "tetris.h"
#include "timer.h"
#include "lcd.h"
#include "rng.h"

// Фигуры: L, O, Z, I, T

// Размеры игровой области c учётом рамки: 16 x 30 кубиков
#define TETRIS_FIELD_WIDTH      16
#define TETRIS_FIELD_HEIGHT     30

// Перечисление типов фигур
enum
{
    TETRIS_FIGURE_TYPE_O,
    TETRIS_FIGURE_TYPE_L,
    TETRIS_FIGURE_TYPE_Z,
    TETRIS_FIGURE_TYPE_I,
    TETRIS_FIGURE_TYPE_T
};

// Количество фигур
#define TETRIS_FIGURE_NUMBER        5

// Массив типов фигур
static uint16_t tetris_figure_type[TETRIS_FIGURE_NUMBER] = 
{
    TETRIS_FIGURE_TYPE_O,
    TETRIS_FIGURE_TYPE_L,
    TETRIS_FIGURE_TYPE_Z,
    TETRIS_FIGURE_TYPE_I,
    TETRIS_FIGURE_TYPE_T,
};

// Количество цветов фигур
#define TETRIS_FIGURE_COLORS_NUMBER     8

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
    uint8_t collum; // Столбец
    uint8_t row;    // Строка
    
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

// Массив игрового поля
static bool tetris_field[TETRIS_FIELD_HEIGHT][TETRIS_FIELD_WIDTH];

// Постоянное смещение фигуры по таймеру
#define TETRIS_TIMER_OFFSET      10
// Cмещение фигуры по кнопке
#define TETRIS_OFFSET_Y     TETRIS_TIMER_OFFSET
#define TETRIS_OFFSET_X     10

// Предварительное объявление функции проверки состояния кнопок
static void tetris_offset_timer_cb(timer_t *timer);

// Таймер для проверки состояния кнопок смещения
static timer_t tetris_offset_timer_cb_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, tetris_offset_timer_cb);

// Перечисление направлений смещения фигуры
typedef enum
{
    TETRIS_OFFSET_DOWN,
    TETRIS_OFFSET_LEFT,
    TETRIS_OFFSET_RIGHT,
    
} tetris_offset_t;

// Флаги состояния кнопок
static bool tetris_key[4] = {false, false, false, false};

// Отрисовка одного куба фигуры 10х10
static void tetris_draw_cube(uint16_t offset_y, uint16_t offset_x, uint16_t color)
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

// Смещение фигуры по осям с учётом коллизий
static void tetris_offset_timer_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Если нажаты ВПРАВО и ВЛЕВО одновременно - выход без обработки нажатия
    //if (tetris_key[KEY_NAME_RIGHT] | tetris_key[KEY_NAME_LEFT])
    
    // Если при смещении произошла коллизия - выход
    //if (tetris_check_collision())
        //return;
}

void tetris_key_notice(key_name_t kay, bool state)
{
    // Изменение состояния кнопок
    tetris_key[kay] = state;
    
    switch (kay)
    {
        case KEY_NAME_ROTATE:
            //tetris_rotate_figure();
            break;
        
        case KEY_NAME_DOWN:
        case KEY_NAME_RIGHT:
        case KEY_NAME_LEFT:
            tetris_offset_timer_cb(&tetris_offset_timer_cb_timer);
            break;
    }
}
/*
массив[строка][столбец]
    {1, 1, 1},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0}
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
            tetris_figure.collum = 2;
            tetris_figure.row = 2;
            tetris_shape[0][0] = 1;
            tetris_shape[0][1] = 1;
            tetris_shape[1][0] = 1;
            tetris_shape[1][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_Z:
            tetris_figure.collum = 3;
            tetris_figure.row = 2;
            tetris_shape[0][0] = 1;
            tetris_shape[0][1] = 1;
            tetris_shape[1][1] = 1;
            tetris_shape[1][2] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_L:
            tetris_figure.collum = 2;
            tetris_figure.row = 3;
            tetris_shape[0][0] = 1;
            tetris_shape[1][0] = 1;
            tetris_shape[2][0] = 1;
            tetris_shape[0][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_T:
            tetris_figure.collum = 3;
            tetris_figure.row = 2;
            tetris_shape[0][0] = 1;
            tetris_shape[0][1] = 1;
            tetris_shape[0][2] = 1;
            tetris_shape[1][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_I:
            tetris_figure.collum = 4;
            tetris_figure.row = 3;
            tetris_shape[1][0] = 1;
            tetris_shape[1][1] = 1;
            tetris_shape[1][2] = 1;
            tetris_shape[1][3] = 1;
            break;
            
        default:
            (assert(false));
    }
}

// Функция для отрисовки фигуры по таймеру
static void tetris_draw_new_figure(void)
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
    
    // Установка начального положения новой фигуры
    tetris_figure.x = 69;
    tetris_figure.y = 209; // 308
    
    // Отрисовка всеех секций фигуры 
    /* Происходит один раз при создании новой фигуры            *
     * далее происходит только перерисовка изменяющихся частей, *
     * а не фигуры целиком                                      */
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строка
        for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбец
            if (tetris_shape[i][j] == 1)
                // Вызов отрисовки одного куба фигуры
                tetris_draw_cube(i * 10, j * 10, tetris_figure.color);
}

// Таймер для непрерывной отрисовки движущейся фигуры
static timer_t tetris_offset_down_timer = TIMER_STATIC_INIT(TIMER_MODE_CONTINUOUS, tetris_offset_timer_cb);

void tetris_init(void)
{
    // Запуск таймера
    //timer_start(&tetris_offset_down_timer, TIMER_TICKS_MS(100));
    
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
    
    // Отрисовать стартовую фигуру
    tetris_draw_new_figure();
}
