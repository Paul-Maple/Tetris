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

// Размер массива с формой фигуры
#define TETRIS_SIZE_FIGURE_SAPE     5

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
    
    // Структура координат фигуры в игровом массиве
    struct
    {
        uint8_t x;
        uint8_t y;
    } game_array;
    
    // Реальный размер фигуры (Кол-вл кубиков по горизонтали и вертикали)
    uint8_t collum; // Столбец
    uint8_t row;    // Строка
    
    // Массив с формой фигуры
    bool shape[TETRIS_SIZE_FIGURE_SAPE][TETRIS_SIZE_FIGURE_SAPE];

} tetris_figure_t;

// Текущая активная фигура
static tetris_figure_t tetris_figure;

// Массив игрового поля
static bool tetris_field[TETRIS_FIELD_HEIGHT][TETRIS_FIELD_WIDTH];

    /*** Пиксели для смещения фигуры ***/
// Постоянное смещение фигуры по таймеру
#define TETRIS_TIMER_OFFSET      1
// Cмещение фигуры по кнопке
#define TETRIS_OFFSET_Y     TETRIS_TIMER_OFFSET
#define TETRIS_OFFSET_X     10

    /*** Таймеры ***/
// Время смещения фигуры (миллисекунды)
#define TETRIS_DOWN_OFFSET_TIME         30
#define TETRIS_KEY_DOWN_OFFSET_TIME     5
#define TETRIS_KEY_SIDE_OFFSET_TIME     150

// Предварительное объявление callback функций таймеров
static void tetris_down_offset_timer_cb (timer_t *timer);
static void tetris_key_offset_timer_cb (timer_t *timer);

// Таймер для непрерывного смещения фигуры вниз и по кнопке ВНИЗ
static timer_t tetris_down_offset_timer = TIMER_STATIC_INIT(TIMER_MODE_CONTINUOUS, tetris_down_offset_timer_cb);
// Таймер для смещения фигуры по нажатию кнопок
static timer_t tetris_key_offset_timer = TIMER_STATIC_INIT(TIMER_MODE_ONE_SHOT, tetris_key_offset_timer_cb);

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

// Отрисовка одного куба фигуры 10х10 (цветной куб - 8х8)
static void tetris_draw_cube(const uint16_t offset_y, const uint16_t offset_x, const lcd_color_t color)
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
            temp_shape[i][j] = tetris_figure.shape[i][j];
            // Очистка
            if (temp_shape[i][j] == 1)
            {
                tetris_figure.shape[i][j] = 0;
                tetris_draw_cube(i * TETRIS_CUBE_SIZE, j * TETRIS_CUBE_SIZE, LCD_COLOR_WHITE);
            }
        }
    
    // Поворачиваем фигуру на 90 градусов по часовой стрелке
    for (uint8_t i = 0; i < tetris_figure.row; i++)
        for (uint8_t j = 0; j < tetris_figure.collum; j++)
            if (temp_shape[i][j] == 1)
            {
                // Поворачиваем относительно текущих размеров
                tetris_figure.shape[j][tetris_figure.row - 1 - i] = 1;
                tetris_draw_cube(j * TETRIS_CUBE_SIZE, (tetris_figure.row - 1 - i) * TETRIS_CUBE_SIZE, tetris_figure.color);
            }
    
    // Обновляем размеры после поворота
    uint8_t temp_size = tetris_figure.collum;
    tetris_figure.collum = tetris_figure.row;
    tetris_figure.row = temp_size;
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
            // Обработка состояния кнопки
            state ?
                timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_DOWN_OFFSET_TIME)) :   // Срабатывание чаще
                timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_DOWN_OFFSET_TIME));        // Срабатывание реже
            break;
        case KEY_NAME_RIGHT:
        case KEY_NAME_LEFT:
        case KEY_NAME_ROTATE:
            // Обработка состояния кнопки
            if (state)
                //timer_start(&tetris_key_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_SIDE_OFFSET_TIME));    // Запуск таймера
                tetris_key_offset_timer_cb(&tetris_key_offset_timer);
            break;
        default:
            assert(false);
    }
}

// Инициализация формы фигуры
static void tetris_init_shape(const tetris_figure_type_t type)
{
    // Обнуление массива с формой фигуры
    for (uint8_t i = 0; i < TETRIS_SIZE_FIGURE_SAPE; i++)
        for (uint8_t j = 0; j < TETRIS_SIZE_FIGURE_SAPE; j++)
            tetris_figure.shape[i][j] = 0;
    
    // Присвоение формы
    switch(type)
    {
        case TETRIS_FIGURE_TYPE_O:
            tetris_figure.collum = 2;
            tetris_figure.row = 2;
            tetris_figure.shape[0][0] = 1;
            tetris_figure.shape[0][1] = 1;
            tetris_figure.shape[1][0] = 1;
            tetris_figure.shape[1][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_Z:
            tetris_figure.collum = 3;
            tetris_figure.row = 2;
            tetris_figure.shape[0][0] = 1;
            tetris_figure.shape[0][1] = 1;
            tetris_figure.shape[1][1] = 1;
            tetris_figure.shape[1][2] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_L:
            tetris_figure.collum = 2;
            tetris_figure.row = 3;
            tetris_figure.shape[0][0] = 1;
            tetris_figure.shape[1][0] = 1;
            tetris_figure.shape[2][0] = 1;
            tetris_figure.shape[0][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_T:
            tetris_figure.collum = 3;
            tetris_figure.row = 2;
            tetris_figure.shape[0][0] = 1;
            tetris_figure.shape[0][1] = 1;
            tetris_figure.shape[0][2] = 1;
            tetris_figure.shape[1][1] = 1;
            break;
            
        case TETRIS_FIGURE_TYPE_I:
            tetris_figure.collum = 4;
            tetris_figure.row = 3;
            tetris_figure.shape[1][0] = 1;
            tetris_figure.shape[1][1] = 1;
            tetris_figure.shape[1][2] = 1;
            tetris_figure.shape[1][3] = 1;
            break;
            
        default:
            (assert(false));
    }
}

// Смещение фигуры вниз на 1 пиксель
static void tetris_offset_down()
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;
    
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строки
        for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбцы
            if (tetris_figure.shape[i][j] == 1)
            {
                // Координаты смещения по осям
                uint16_t offset_x = tetris_figure.x + j * TETRIS_CUBE_SIZE;
                uint16_t offset_y = tetris_figure.y + i * TETRIS_CUBE_SIZE;
                
                // Очистить верхнюю полосу (полоса 1 пиксель)
                TETRIS_DRAW_FIELD(figure,
                                  offset_x,
                                  offset_x + TETRIS_CUBE_SIZE,
                                  offset_y + TETRIS_CUBE_SIZE - 1,
                                  offset_y + TETRIS_CUBE_SIZE - 1,
                                  LCD_COLOR_WHITE);
                
                // Цветная полоcа внизу (полоса 1 пиксель)
                TETRIS_DRAW_FIELD(figure,
                                 offset_x + TETRIS_CUBE_SIZE - 9,
                                 offset_x + TETRIS_CUBE_SIZE - 1,
                                 offset_y,
                                 offset_y,
                                 tetris_figure.color);
            }
    
    // Изменяем координату фигуры
    tetris_figure.y -= TETRIS_OFFSET_Y;
}

// Смещение фигуры влево на 1 кубик
static void tetris_offset_left()
{
    // Пройтись по всем слолбцам
    for (uint8_t i = 0; i < tetris_figure.row; i++)          // Строки
        for (uint8_t j = 0; j < tetris_figure.collum; j++)   // Столбцы
            if (tetris_figure.shape[i][j] == 1)
            {
                // Очистить текущий кубик
                tetris_draw_cube(i * TETRIS_CUBE_SIZE, j * TETRIS_CUBE_SIZE, LCD_COLOR_WHITE);
                // Вызов отрисовки одного куба фигуры слева
                tetris_draw_cube(i * TETRIS_CUBE_SIZE, (j - 1) * TETRIS_CUBE_SIZE, tetris_figure.color);
            }
    // Изменяем координату фигуры
    tetris_figure.x -= TETRIS_OFFSET_X;
}

// Смещение фигуры вправо на 1 кубик
static void tetris_offset_right()
{
    // Пройтись по всем слолбцам
    for (uint8_t i = 0; i < tetris_figure.row; i++)             // Строки
        for (int8_t j = tetris_figure.collum; j >= 0 ; j--)     // Столбцы
            if (tetris_figure.shape[i][j] == 1)
            {
                // Очистить текущий кубик
                tetris_draw_cube(i * TETRIS_CUBE_SIZE, j * TETRIS_CUBE_SIZE, LCD_COLOR_WHITE);
                // Вызов отрисовки одного куба фигуры справа
                tetris_draw_cube(i * TETRIS_CUBE_SIZE, (j + 1) * TETRIS_CUBE_SIZE, tetris_figure.color);
            }
    // Изменяем координату фигуры
    tetris_figure.x += TETRIS_OFFSET_X;
}

// Перерисовка фигуры при смещении
static void tetris_redrawing_figure(const tetris_offset_t offset)
{
    // Обработка смещения
    switch (offset)
    {
        case TETRIS_OFFSET_DOWN:
            tetris_offset_down();
            break;
        
        case TETRIS_OFFSET_LEFT:
            tetris_offset_left();
            break;      
        
        case TETRIS_OFFSET_RIGHT:
            tetris_offset_right();
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
static bool tetris_check_collision(const tetris_offset_t offset)
{
    return false;
    // Пройтись по массиву игрового поля и проверить совпадения
    // Если произошла коллизия с нижней границей 
    //
    
    // то зафиксировать фигуру в массиве игрового поля
    // tetris_record_figure();
    
    // Проверить строки
    // Если они полностью заполнены - очистить
    // 
    
    // И создать новую фигуру
    // tetris_create_new_figure();
}

// Смещение фигуры по кнопкам с проверкой коллизий
static void tetris_key_offset_timer_cb(timer_t *timer)
{
    ASSERT_NULL_PTR(timer);
    
    // Определить нажата ли хотя бы одна кнопка
    bool state_key = tetris_key[TETRIS_OFFSET_LEFT].state   |
                     tetris_key[TETRIS_OFFSET_RIGHT].state  |
                     tetris_key[TETRIS_OFFSET_ROTATE].state;
    
    // Начальный индекс для итерации по массиву кнопок
    uint8_t iterator = 1;
    // Если нажаты кнопи ВПРАВО и ВЛЕВО одновременно
    if (tetris_key[TETRIS_OFFSET_LEFT].state & tetris_key[TETRIS_OFFSET_RIGHT].state)
        // Проверить только кнопку поворота
        iterator = 3;
        
    // Проверить только 1 или 3 кнопки (Кнопка ВНИЗ проверяется отдельно, из за смещения по 1 пикселю)
    for (uint8_t i = iterator; i < TETRIS_KEY_NUMBER; i++)
        // Если кнопка нажата - проверить коллизию
        if (tetris_key[i].state)
            // Если коллизии нет - обработать смещение
            if (!(tetris_check_collision(tetris_key[i].offset)))    
                tetris_redrawing_figure(tetris_key[i].offset);
    
    // Если нажата ли хотя бы одна кнопка - запуск таймера заново
    if (state_key)
        timer_start(&tetris_key_offset_timer, TIMER_TICKS_MS(TETRIS_KEY_SIDE_OFFSET_TIME));
}

// Функция для отрисовки фигуры по таймеру
static void tetris_create_new_figure(void)
{
    // Выбор типа новой фигуры
    tetris_figure.type = tetris_figure_type[rng_get_number(TETRIS_FIGURE_NUMBER - 1)];
    // Выбор цвета новой фигуры
    tetris_figure.color = tetris_colors[rng_get_number(TETRIS_FIGURE_COLORS_NUMBER - 1)];
    
    // Присвоить форму фигуре
    tetris_init_shape(tetris_figure.type);
    
    // Установка начального положения новой фигуры
    tetris_figure.x = 69;
    tetris_figure.y = 279;
    
    tetris_figure.game_array.x = ((69 + 1) / 10);                           // 7
    tetris_figure.game_array.y = TETRIS_FIELD_HEIGHT - ((279 + 1) / 10);    // 30 - 21 = 9
    
    // Отрисовка всех секций (кубиков) фигуры
    // Происходит один раз при создании новой фигуры
    // далее происходит только перерисовка изменяющихся частей,
    // а не фигуры целиком
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строка
        for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбец
            if (tetris_figure.shape[i][j] == 1)
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
}

void tetris_init(void)
{
    // Инициализация массива игрового поля нулями
    for (uint8_t i = 0; i < TETRIS_FIELD_HEIGHT; i++)
        for (uint8_t j = 0; j < TETRIS_FIELD_WIDTH; j++)
            tetris_field[i][j] = 0;
    
    // Запуск таймера
    //timer_start(&tetris_down_offset_timer, TIMER_TICKS_MS(TETRIS_DOWN_OFFSET_TIME));
    
    // Область для отрисовки:
    lcd_position_t field;
    // Рамка
    TETRIS_DRAW_FIELD(field, 0, LCD_WIDTH - 1, 0, LCD_HEIGHT - 1, LCD_COLOR_GRAY);
    // Игровое поле
    TETRIS_DRAW_FIELD(field, 9, 169, 9, 309, LCD_COLOR_WHITE);
    // Поле для счёта
    TETRIS_DRAW_FIELD(field, 189, 219, 269, 309, LCD_COLOR_WHITE);
    
    // Создать стартовую фигуру
    tetris_create_new_figure();
}
