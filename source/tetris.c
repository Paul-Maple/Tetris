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

// Размеры игровой области c учётом рамки: 16 x 30 кубиков
#define TETRIS_FIELD_WIDTH      16
#define TETRIS_FIELD_HEIGHT     30

// Размер одного куба фигуры (В пикселях)
#define TETRIS_CUBE_SIZE            10
#define TETRIS_COLOR_CUBE_SIZE      8

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

// Структура текущей фигуры для отрисовки
typedef struct
{
    // Цвет фигуры
    lcd_color_t color;
    // Тип фигуры
    tetris_figure_type_t type;
    
    // Текущие координаты фигуры в пикселях (Исползуются для смещения и отрисовки)
    uint16_t x;
    uint16_t y;
    
    // Размер фигуры (Кол-вл кубиков по горизонтали и вертикали)
    uint8_t collum; // Столбец
    uint8_t row;    // Строка
    
} tetris_figure_t;

// Текущая активная фигура
static tetris_figure_t tetris_figure;

// Массив с формой текущей фигуры фигуры
static bool tetris_shape[5][5] = 
{
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
};

// Массив игрового поля
static bool tetris_field[TETRIS_FIELD_HEIGHT][TETRIS_FIELD_WIDTH];

    /*** Пиксели для смещения фигуры ***/
// Постоянное смещение фигуры по таймеру
#define TETRIS_TIMER_OFFSET      1
// Cмещение фигуры по кнопке
#define TETRIS_OFFSET_Y     TETRIS_TIMER_OFFSET
#define TETRIS_OFFSET_X     10

    /*** Время таймеров ***/
// Время смещения фигуры (миллисекунды)
#define TETRIS_DOWN_OFFSET_TIME         30
#define TETRIS_KEY_DOWN_OFFSET_TIME     5
#define TETRIS_KEY_SIDE_OFFSET_TIME     100

// Предварительное объявление callback функций таймеров
static void tetris_down_offset_timer_cb(timer_t *timer);
static void tetris_key_offset_timer_cb(timer_t *timer);

// Таймер для непрерывного смещения фигуры вниз и по кнопке вниз
static timer_t tetris_down_offset_timer = TIMER_STATIC_INIT(TIMER_MODE_CONTINUOUS, tetris_down_offset_timer_cb);
// Таймер для смещения фигуры по нажатию кнопок
static timer_t tetris_key_offset_timer = TIMER_STATIC_INIT(TIMER_MODE_CONTINUOUS, tetris_key_offset_timer_cb);

// Перечисление направлений смещения фигуры
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
    // Смещение
    tetris_offset_t offset;
    
} tetris_key_t;

// Массив кнопок
static tetris_key_t tetris_key[TETRIS_KEY_NUMBER] = 
{
    {.state = false, .offset = TETRIS_OFFSET_DOWN},
    {.state = false, .offset = TETRIS_OFFSET_LEFT},
    {.state = false, .offset = TETRIS_OFFSET_RIGHT},
    {.state = false, .offset = TETRIS_OFFSET_ROTATE},
};
//{false, false, false, false};

// Отрисовка одного куба фигуры 10х10 (цветной куб - 8х8)
static void tetris_draw_cube(uint16_t offset_y, uint16_t offset_x, lcd_color_t color)
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;
    
    // Цветной куб
    TETRIS_DRAW_FIELD(figure,
                      tetris_figure.x + offset_x + 1,
                      figure.x1 + TETRIS_COLOR_CUBE_SIZE,
                      tetris_figure.y + offset_y + 1,
                      figure.y1 + TETRIS_COLOR_CUBE_SIZE,
                      color);
}

// Вращение фигуры
static void tetris_rotate_figure(void)
{   
    // Временный массив формы фигуры
    bool temp_shape[5][5];
    
    // Копируем значения во временный массив и очищаем старую фигуру
    for (uint8_t i = 0; i < 5; i++)
        for (uint8_t j = 0; j < 5; j++)
        {
            // Копирование
            temp_shape[i][j] = tetris_shape[i][j];
            // Очистка
            if (temp_shape[i][j] == 1)
            {
                tetris_shape[i][j] = 0;
                tetris_draw_cube(i * TETRIS_CUBE_SIZE, j * TETRIS_CUBE_SIZE, LCD_COLOR_WHITE);
            }
        }
    
    // Поворачиваем фигуру на 90 градусов по часовой стрелке
    for (uint8_t i = 0; i < tetris_figure.row; i++)
        for (uint8_t j = 0; j < tetris_figure.collum; j++)
            if (temp_shape[i][j] == 1)
            {
                // Поворачиваем относительно текущих размеров
                tetris_shape[j][tetris_figure.row - 1 - i] = 1;
                tetris_draw_cube(j * TETRIS_CUBE_SIZE, (tetris_figure.row - 1 - i) * TETRIS_CUBE_SIZE, tetris_figure.color);
            }
    
    // Обновляем размеры после поворота
    uint8_t temp_size = tetris_figure.collum;
    tetris_figure.collum = tetris_figure.row;
    tetris_figure.row = temp_size;
}

// Оповещение модуля о состоянии кнопки
void tetris_key_notice(key_name_t key, bool state)
{
    // Изменение состояния кнопки
    tetris_key[key].state = state;
    
    // Обрабатываем состояние кнопки
    switch (key) 
    {
        case KEY_NAME_DOWN:
            // Если кнопка ВНИЗ была нажата
            if (state)
                // Запустить таймер на 10 мсек
                timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_DOWN_OFFSET_TIME));
            else
                // Запустить таймер на 20 мсек
                timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_DOWN_OFFSET_TIME));
            break;
        case KEY_NAME_RIGHT:
        case KEY_NAME_LEFT:
        case KEY_NAME_ROTATE:
            timer_start(&tetris_key_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_SIDE_OFFSET_TIME));
            break;
        default:
            assert(false);
    }      
}

// Получение формы фигуры
static void tetris_get_shape(tetris_figure_type_t type)
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

// Перерисовка фигуры при смещении
static void tetris_redrawing_figure(tetris_offset_t offset)
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;
    
    switch (offset)
    {
        case TETRIS_OFFSET_DOWN:
        // Смещение вниз (на 1 пиксель)
        for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строки
            for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбцы
                if (tetris_shape[i][j] == 1)
                {
                    // Координаты смещения по осям
                    uint16_t offset_x = tetris_figure.x + j * TETRIS_CUBE_SIZE;
                    uint16_t offset_y = tetris_figure.y + i * TETRIS_CUBE_SIZE;
                    
                    // Очистить верхнюю полосу
                    TETRIS_DRAW_FIELD(figure,
                                      offset_x,
                                      offset_x + TETRIS_CUBE_SIZE,
                                      offset_y + TETRIS_CUBE_SIZE - 1,
                                      offset_y + TETRIS_CUBE_SIZE - 1,
                                      LCD_COLOR_WHITE);
                    
                    // Цветная полоcа внизу 
                    TETRIS_DRAW_FIELD(figure,
                                     offset_x + TETRIS_CUBE_SIZE - 9,
                                     offset_x + TETRIS_CUBE_SIZE - 1,
                                     offset_y,
                                     offset_y,
                                     tetris_figure.color);
                }
        
        // Изменяем координату фигуры
        tetris_figure.y -= TETRIS_OFFSET_Y;
        break;
        
        case TETRIS_OFFSET_LEFT:
        case TETRIS_OFFSET_RIGHT:
            //
            break;
        case TETRIS_OFFSET_ROTATE:
            tetris_rotate_figure();
            break;
            
        default:
            assert(false);
    }
}

// Записывает координаты фигуры в игровой массив поля
void tetris_record_figure(void)
{
    
}

// Проверка коллизий
static bool tetris_check_collision(tetris_offset_t offset)
{
    // Пройтись по массиву игрового поля и проверить совпадения
    
    // Если произошла коллизия с нижней границей - зафиксировать фигуру в массиве игрового поля
    // tetris_record_figure();
    return false;
}

// Смещение фигуры по кнопкам с проверкой коллизий
static void tetris_key_offset_timer_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Проверить все 3 кнопки (Кнопка ВНИЗ проверяется отдельно, из за смещения по пикселю)
    for (uint8_t i = 1; i < TETRIS_KEY_NUMBER; i++)
        // Если кнопка нажата - проверить коллизию
        if (tetris_key[i].state)
            // Если коллизии нет - обработать смещение
            if (!(tetris_check_collision(tetris_key[i].offset)))    
                tetris_redrawing_figure(tetris_key[i].offset);
}

// Функция для отрисовки фигуры по таймеру
static void tetris_draw_new_figure(void)
{
    // Выбор типа новой фигуры
    tetris_figure.type = tetris_figure_type[rng_get_number(TETRIS_FIGURE_NUMBER - 1)];
    // Выбор цвета новой фигуры
    tetris_figure.color = tetris_colors[rng_get_number(TETRIS_FIGURE_COLORS_NUMBER - 1)];
    
    // Присвоить форму фигуре
    tetris_get_shape(tetris_figure.type);
    
    // Установка начального положения новой фигуры
    tetris_figure.x = 69;
    tetris_figure.y = 209; // 308
    
    // Отрисовка всех секций (кубиков) фигуры
    // Происходит один раз при создании новой фигуры
    // далее происходит только перерисовка изменяющихся частей,
    // а не фигуры целиком
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строка
        for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбец
            if (tetris_shape[i][j] == 1)
                // Вызов отрисовки одного куба фигуры
                tetris_draw_cube(i * TETRIS_CUBE_SIZE, j * TETRIS_CUBE_SIZE, tetris_figure.color);
}

// Смещение фигуры вниз с проверкой коллизий
static void tetris_down_offset_timer_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Проверка коллизий с учётом смещений
    if (!(tetris_check_collision(TETRIS_OFFSET_DOWN)))
        // Сместить фигуру вниз
        tetris_redrawing_figure(TETRIS_OFFSET_DOWN);
    else
        tetris_draw_new_figure();
}

void tetris_init(void)
{
    // Инициализация массива игрового поля нулями
    for (uint8_t i = 0; i < TETRIS_FIELD_HEIGHT; i++)
        for (uint8_t j = 0; j < TETRIS_FIELD_WIDTH; j++)
            tetris_field[i][j] = 0;
    
    // Запуск таймера
    //timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_DOWN_OFFSET_TIME));
    
    // Область для отрисовки
    lcd_position_t field;
    // Рамка
    TETRIS_DRAW_FIELD(field, 0, LCD_WIDTH - 1, 0, LCD_HEIGHT - 1, LCD_COLOR_GRAY);
    // Игровое поле
    TETRIS_DRAW_FIELD(field, 9, 169, 9, 309, LCD_COLOR_WHITE);
    // Поле для счёта
    TETRIS_DRAW_FIELD(field, 189, 219, 269, 309, LCD_COLOR_WHITE);
    
    // Отрисовать стартовую фигуру
    tetris_draw_new_figure();
}
