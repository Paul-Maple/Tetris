#include "tetris.h"
#include "timer.h"
#include "lcd.h"
#include "rng.h"

// Фигуры: L, O, Z, I, T

/*
массив[строка][столбец]
    {1, 1, 1},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0}
*/

// Присовение координат и отрисовка области
#define TETRIS_DRAW_FIELD(field, _x1, _x2, _y1, _y2, color)                     \
    field.x1 = _x1;                                                             \
    field.x2 = _x2;                                                             \
    field.y1 = _y1;                                                             \
    field.y2 = _y2;                                                             \
    lcd_draw_image(field, color)

    /*** Пиксели для смещения фигуры ***/
// Cмещение фигуры (В пикселях)
//#define TETRIS_OFFSET_Y     1
//#define TETRIS_OFFSET_X     10
// Размер одного куба фигуры (В пикселях)
#define TETRIS_CUBE_SIZE            10
#define TETRIS_COLOR_CUBE_SIZE      8

// Координаты для отрисовки новой фигуры
#define TETRIS_START_COORDINATE_X       7
#define TETRIS_START_COORDINATE_Y       27

// Перечисление типов фигур
typedef enum
{
    TETRIS_FIGURE_TYPE_O,
    TETRIS_FIGURE_TYPE_L,
    TETRIS_FIGURE_TYPE_Z,
    TETRIS_FIGURE_TYPE_I,
    TETRIS_FIGURE_TYPE_T,
    // TODO: Добавить ещё фигуры
} tetris_figure_type_t;

// Количество фигур
#define TETRIS_FIGURE_NUMBER        5

// Массив типов фигур
static tetris_figure_type_t tetris_figure_type[TETRIS_FIGURE_NUMBER] = 
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
static lcd_color_t tetris_colors[TETRIS_FIGURE_COLORS_NUMBER] = 
{
    LCD_COLOR_RED,
    LCD_COLOR_GREEN,
    LCD_COLOR_BLUE,
    LCD_COLOR_YELLOW,
    LCD_COLOR_ORANGE,
    LCD_COLOR_CYAN,
    LCD_COLOR_PURPLE,
    LCD_COLOR_PINK,
    // TODO: Добавить ещё цвета
};

// Размер массива с формой фигуры
#define TETRIS_SIZE_FIGURE_SAPE     5

// Структура одного кубика фигуры
typedef struct
{
    // Наличие/Отсутствие кубика
    bool state;
    // Координаты в массиве игрового поля 16 х 30
    uint8_t x;
    uint8_t y;
} tetris_cube_t;

// Структура текущей фигуры для отрисовки
typedef struct
{
    // Цвет фигуры
    lcd_color_t color;
    // Тип фигуры
    tetris_figure_type_t type;
    
    // Массив формы фигуры с кубиками (5 х 5 клеток)
    tetris_cube_t shape[TETRIS_SIZE_FIGURE_SAPE][TETRIS_SIZE_FIGURE_SAPE];
    
    // Реальный размер фигуры (Кол-вл кубиков по горизонтали и вертикали)
    uint8_t collum; // Столбец
    uint8_t row;    // Строка
    
} tetris_figure_t;

// Текущая активная фигура
static tetris_figure_t tetris_figure;

// Размеры игровой области
#define TETRIS_FIELD_WIDTH      16
#define TETRIS_FIELD_HEIGHT     30

// Структура игры
typedef struct
{
    // Массив игрового поля (16 x 30 клеток)
    bool field[TETRIS_FIELD_HEIGHT][TETRIS_FIELD_WIDTH];
    // Счёт
    uint16_t points;
    // Меню
    //menu
} tetris_t;

// Тетрис
static tetris_t tetris;

    /*** Время для таймеров ***/
// Время смещения фигуры (миллисекунды)
#define TETRIS_DOWN_OFFSET_TIME         400//30      // Смещение вниз постоянное
#define TETRIS_KEY_DOWN_OFFSET_TIME     100//5       // Смещение вниз по кнопке
#define TETRIS_KEY_SIDE_OFFSET_TIME     150     // Смещение вбок по кнопке

// Предварительное объявление callback функций таймеров смещения
static void tetris_down_offset_timer_cb (timer_t *timer);
static void tetris_key_offset_timer_cb (timer_t *timer);

// Таймер для непрерывного смещения фигуры вниз и по кнопке ВНИЗ
static timer_t tetris_down_offset_timer = TIMER_STATIC_INIT(TIMER_MODE_CONTINUOUS, tetris_down_offset_timer_cb);
// Таймер для смещения фигуры по нажатию кнопок (Влево, вправо и поворот)
static timer_t tetris_key_offset_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, tetris_key_offset_timer_cb);

// Перечисление типов изменения положения фигуры (Порядок НЕ МЕНЯТЬ !!!!!)
typedef enum
{
    TETRIS_OFFSET_DOWN,
    TETRIS_OFFSET_LEFT,
    TETRIS_OFFSET_RIGHT,
    TETRIS_OFFSET_ROTATE,
    
} tetris_offset_t;

// Количество кнопок
#define TETRIS_KEY_NUMBER       4

// Структура кнопки
typedef struct
{
    // Состояние
    bool state;
    // Тип смещения
    const tetris_offset_t offset;
    
} tetris_key_t;

// Массив кнопок (Порядок кнопок НЕ МЕНЯТЬ !!!!!!)
static tetris_key_t tetris_key[TETRIS_KEY_NUMBER] = 
{
    {.state = false, .offset = TETRIS_OFFSET_DOWN},
    {.state = false, .offset = TETRIS_OFFSET_LEFT},
    {.state = false, .offset = TETRIS_OFFSET_RIGHT},
    {.state = false, .offset = TETRIS_OFFSET_ROTATE},
};

// Отрисовка одного куба фигуры 10х10 (цветной куб - 8х8)
static void tetris_draw_cube(const tetris_cube_t cube, lcd_color_t color)
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;
    
    // Отрисовать куб 
    TETRIS_DRAW_FIELD(figure,
                      cube.x * TETRIS_CUBE_SIZE + 1,
                      cube.x * TETRIS_CUBE_SIZE + TETRIS_COLOR_CUBE_SIZE + 1,
                      cube.y * TETRIS_CUBE_SIZE + 1,
                      cube.y * TETRIS_CUBE_SIZE + TETRIS_COLOR_CUBE_SIZE + 1,
                      color);
}

// Оповещение модуля о состоянии кнопки
void tetris_key_notice(const key_name_t key, const bool state)
{
    // Изменение состояния кнопки
    tetris_key[key].state = state;
    
    // Обрабатываем состояние кнопки
    switch (key) 
    {
        case KEY_NAME_DOWN:
            // В зависимости от состояния кнопки меняется время срабатывания таймера
            state ?
                timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_DOWN_OFFSET_TIME)) :   // Срабатывание чаще
                timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_DOWN_OFFSET_TIME));        // Срабатывание реже
            break;
        case KEY_NAME_RIGHT:
        case KEY_NAME_LEFT:
        case KEY_NAME_ROTATE:
            // Отложенная обработка нажатия
            if (state)
                timer_start(&tetris_key_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_DOWN_OFFSET_TIME));
            break;
            
        default:
            assert(false);
    }
}

// Перечесление действий при вызове функции смещения
typedef enum
{
    // Сместить
    TETRIS_ACTION_OFFSET,
    // Только проверить коллизию, без смещения
    TETRIS_ACTION_CHECK_COLLISION
        
} tetris_action_t;

// Проверка коллизий
static bool tetris_check_collision(tetris_cube_t cube, const tetris_offset_t offset)
{
    
    return false;
}

// Отрисовка фигуры целиком
static void tetris_draw_figure(void)
{
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строка
        for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбец
            if (tetris_figure.shape[i][j].state)
                // Вызов отрисовки одного куба фигуры
                tetris_draw_cube(tetris_figure.shape[i][j], tetris_figure.color);
}

// Обработка вращения фигуры
static void tetris_processing_rotate_figure(void)
{
  // Временный массив формы фигуры
    tetris_cube_t temp_shape[TETRIS_SIZE_FIGURE_SAPE][TETRIS_SIZE_FIGURE_SAPE];
    // Минимальные координаты для сохранения положения фигуры при вращении
    uint8_t min_x = 0xFF, min_y = 0xFF;
    
    // Копирование и стирание старой фигуры
    for (uint8_t i = 0; i < TETRIS_SIZE_FIGURE_SAPE; i++)
        for (uint8_t j = 0; j < TETRIS_SIZE_FIGURE_SAPE; j++)
        {
            temp_shape[i][j] = tetris_figure.shape[i][j];
            if (temp_shape[i][j].state) 
            {
              // Обновляем минимальные координаты
                if (temp_shape[i][j].x < min_x) min_x = temp_shape[i][j].x;
                if (temp_shape[i][j].y < min_y) min_y = temp_shape[i][j].y;
                // Очистка оригинальной фигуры
                tetris_figure.shape[i][j].state = 0;
                tetris_draw_cube(tetris_figure.shape[i][j], LCD_COLOR_WHITE);
            }
        }
    
    // Поворот фигуры
    for (uint8_t i = 0; i < tetris_figure.row; i++) 
        for (uint8_t j = 0; j < tetris_figure.collum; j++) 
            if (temp_shape[i][j].state)
            {
                tetris_figure.shape[j][tetris_figure.row - 1 - i].state = 1;
                tetris_figure.shape[j][tetris_figure.row - 1 - i].x = min_x + tetris_figure.row - 1 - i;
                tetris_figure.shape[j][tetris_figure.row - 1 - i].y = min_y + j;
            }
    
    // Обновляем размеры после поворота
    uint8_t temp_size = tetris_figure.collum;
    tetris_figure.collum = tetris_figure.row;
    tetris_figure.row = temp_size;
    
    // Отрисовать повёрнутую фигуру
    tetris_draw_figure();
}

// Обработка смещения фигуры
static void tetris_processing_offset(const tetris_action_t action, const tetris_offset_t offset)
{
    // Если нужен поворот
    if (offset == TETRIS_OFFSET_ROTATE)
    {
        // Обработать поворот
        tetris_processing_rotate_figure();
        // Выход сразу
        return;
    }
    
    // Флаг коллизий
    bool collision_flag = false;
    // Флаг пропуска строки
    bool skip_row_flag;
    
    // Пройтись по массиву формы фигуры 
    // Cнизу вверх
    for (int8_t i = tetris_figure.row - 1; i >= 0; i--)
    {
        // Если коллизия произошла - далее проверять не нужно, выход из цикла сразу
        if (collision_flag)
            break;
        // Сбросить флаг пропуска строки
        skip_row_flag = false;
        
        // Слево направо
        for (uint8_t j = 0; j < tetris_figure.collum; j++)
        {
            if (tetris_figure.shape[i][j].state)
            {
                // Только проверка коллизии
                if (action == TETRIS_ACTION_CHECK_COLLISION)
                {
                    // Проверить коллизию
                    collision_flag |= tetris_check_collision(tetris_figure.shape[i][j], offset);
                    // Пропустить проверку оставшихся кубиков в строке
                    skip_row_flag = true;
                }
                
                // Только очистка старой фигуры и изменение координат
                else
                    switch (offset)
                    {
                        case TETRIS_OFFSET_DOWN:
                            // Очистить текущий кубик
                            tetris_draw_cube(tetris_figure.shape[i][j], LCD_COLOR_WHITE);
                            // Изменить координату
                            tetris_figure.shape[i][j].y -= 1;
                            break;
                        case TETRIS_OFFSET_LEFT:
                            // Очистить текущий кубик
                            tetris_draw_cube(tetris_figure.shape[i][j], LCD_COLOR_WHITE);
                            // Изменить координату
                            tetris_figure.shape[i][j].x -= 1;
                            break;
                        case TETRIS_OFFSET_RIGHT:
                            // Очистить текущий кубик
                            tetris_draw_cube(tetris_figure.shape[i][j], LCD_COLOR_WHITE);
                            // Изменить координату
                            tetris_figure.shape[i][j].x += 1;
                            break;
                        case TETRIS_OFFSET_ROTATE:
                            // Обработка вращения в отдельной функции
                            assert (false);
                            break;
                    }
            }
            
            // Если крайний кубик в строке проверен - пропустить эту строку
            if (skip_row_flag)    
                break;
        }
    }
    
    // Обработка цели вызова функции
    switch (action)
    {
        case TETRIS_ACTION_OFFSET:
            // Отрисовать фигуру
            tetris_draw_figure();
            break;
        case TETRIS_ACTION_CHECK_COLLISION:
             // Если коллизий не было обнаружено
            if (!collision_flag)
                // Вызвать эту же функцию, но для смещения фигуры
                tetris_processing_offset(TETRIS_ACTION_OFFSET, offset);
            break;
    }
}

    /*** Обработчик события отработки таймера смещения по кнопке ***/
// Смещение фигуры по кнопкам с проверкой коллизий
static void tetris_key_offset_timer_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Начальный индекс для итерации по массиву кнопок (Начинаем проверять с кнопки ВЛЕВО)
    uint8_t start_index = TETRIS_OFFSET_LEFT;
    // Если нажаты кнопи ВПРАВО и ВЛЕВО одновременно
    if (tetris_key[TETRIS_OFFSET_LEFT].state & tetris_key[TETRIS_OFFSET_RIGHT].state)
        // Проверить только кнопку поворота
        start_index = TETRIS_OFFSET_ROTATE;
    
    // Проверить только 1 или 3 кнопки (Кнопка ВНИЗ проверяется отдельно)
    for (uint8_t i = start_index; i < TETRIS_KEY_NUMBER; i++)
        // Если кнопка нажата - проверить коллизию
        if (tetris_key[i].state)
            // Обработать смещение с проверкой коллизий
            tetris_processing_offset(TETRIS_ACTION_CHECK_COLLISION, tetris_key[i].offset);
    
    // Определить нажата ли хотя бы одна кнопка
    bool state_key = tetris_key[TETRIS_OFFSET_LEFT].state   |
                     tetris_key[TETRIS_OFFSET_RIGHT].state;
    
    // Если нажата ли хотя бы одна кнопка смещения вбок - запуск таймера заново
    // Кнопка поворота по удержанию работать не должна
    if (state_key)
        timer_start(&tetris_key_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_SIDE_OFFSET_TIME));
}

    /*** Обработчик события отработки таймера постоянного смещения ***/
// Смещение фигуры вниз с проверкой коллизий
static void tetris_down_offset_timer_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    tetris_processing_offset(TETRIS_ACTION_CHECK_COLLISION, TETRIS_OFFSET_DOWN);
}

// Инициализация формы фигуры
static void tetris_init_shape(const tetris_figure_type_t type)
{
    // Обнуление массива с формой фигуры
    for (uint8_t i = 0; i < TETRIS_SIZE_FIGURE_SAPE; i++)
        for (uint8_t j = 0; j < TETRIS_SIZE_FIGURE_SAPE; j++)
            tetris_figure.shape[i][j].state = 0;
    
    // Присвоение формы
    switch(type)
    {
        case TETRIS_FIGURE_TYPE_O:
            tetris_figure.collum = 2;
            tetris_figure.row = 2;
            tetris_figure.shape[0][0].state = 1;
            tetris_figure.shape[0][1].state = 1;
            tetris_figure.shape[1][0].state = 1;
            tetris_figure.shape[1][1].state = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_Z:
            tetris_figure.collum = 3;
            tetris_figure.row = 2;
            tetris_figure.shape[0][0].state = 1;
            tetris_figure.shape[0][1].state = 1;
            tetris_figure.shape[1][1].state = 1;
            tetris_figure.shape[1][2].state = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_L:
            tetris_figure.collum = 2;
            tetris_figure.row = 3;
            tetris_figure.shape[0][0].state = 1;
            tetris_figure.shape[1][0].state = 1;
            tetris_figure.shape[2][0].state = 1;
            tetris_figure.shape[0][1].state = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_T:
            tetris_figure.collum = 3;
            tetris_figure.row = 2;
            tetris_figure.shape[0][0].state = 1;
            tetris_figure.shape[0][1].state = 1;
            tetris_figure.shape[0][2].state = 1;
            tetris_figure.shape[1][1].state = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_I:
            tetris_figure.collum = 4;
            tetris_figure.row = 3;
            tetris_figure.shape[1][0].state = 1;
            tetris_figure.shape[1][1].state = 1;
            tetris_figure.shape[1][2].state = 1;
            tetris_figure.shape[1][3].state = 1;
            break;
            
        default:
            (assert(false));
    }
    
    // Присвоить координаты кубикам
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строка
        for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбец
        {
            tetris_figure.shape[i][j].y = TETRIS_START_COORDINATE_Y + i;    // Строка
            tetris_figure.shape[i][j].x = TETRIS_START_COORDINATE_X + j;    // Столбец
        }
}

// Функция для создания новой фигуры
static void tetris_create_new_figure(void)
{
    // Выбор типа новой фигуры
    tetris_figure.type = tetris_figure_type[rng_get_number(TETRIS_FIGURE_NUMBER - 1)];
    // Выбор цвета новой фигуры
    tetris_figure.color = tetris_colors[rng_get_number(TETRIS_FIGURE_COLORS_NUMBER - 1)];
    
    // Присвоить форму фигуре
    tetris_init_shape(tetris_figure.type);
    
    // Отрисовка фигуры
    tetris_draw_figure();
}

// Старт новой игры
static void tetris_start_new_game(void)
{
    // Область для отрисовки
    lcd_position_t field;
    
    // Очистить игровое поле
    TETRIS_DRAW_FIELD(field, 9, 169, 9, 309, LCD_COLOR_WHITE);
    // Очистить поле для счёта
    TETRIS_DRAW_FIELD(field, 189, 219, 269, 309, LCD_COLOR_WHITE);
    
    // Обнулить игровое поле
    for (uint8_t i = 0; i < TETRIS_FIELD_HEIGHT; i++)
        for (uint8_t j = 0; j < TETRIS_FIELD_WIDTH; j++)
            tetris.field[i][j] = 0;
    
    // Сбросить счёт
    tetris.points = 0;
    
    // Создать стартовую фигуру
    tetris_create_new_figure();
    
    // Запуск основного таймера смещения фигуры вниз
    //timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_DOWN_OFFSET_TIME));
}

void tetris_init(void)
{
    // Область для отрисовки
    lcd_position_t field;
    // Заполение фона статичным цветом
    TETRIS_DRAW_FIELD(field, 0, LCD_WIDTH - 1, 0, LCD_HEIGHT - 1, LCD_COLOR_GRAY);
    
    // Начать новую игру
    tetris_start_new_game();
}
