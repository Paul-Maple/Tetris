#include "test.h"
#include "lcd.h"

void test_init(void)
{
    
}
/*
//--------------------------------------------------------------------------------------------------------
// Проверка коллизии с нижней границей
static bool tetris_check_collision_down(void)
{
    // Цвет пикселя для чтения
    uint16_t color;
    // Координаты пикселя для чтения из памяти LCD
    lcd_position_t pixel;
    
    // Флаг проверки нижнего кубика в столбце
    bool chek_colum = false;
    
    // Проходимся снизу-вверх, слева-направо
    for (uint8_t j = 0; j < tetris_figure.collum; j++)      // Столбец
    {
        for (uint8_t i = 0; i < tetris_figure.row; i++)     // Строка
        {
            if (tetris_shape[i][j] == 1)
            {
                // Назначение координат одного пикселя
                pixel.x1 = pixel.x2 = tetris_figure.x + j * 10 + 5;
                pixel.y1 = pixel.y2 = tetris_figure.y + i * 10 - 1;
                // Чтение цвета пикселя
                //lcd_read_color(pixel, &color, 1);
                
                // Если произошла коллизия
                if (color != LCD_COLOR_WHITE)
                    return true;   
                
                // Нижний куб в столбце проверен
                chek_colum = true;
                
                // Проверенный пиксель - самый нижний в столбце,
                   значит следующую стоку проверять не надо 
                // Переход к следующей строке
                break;
            }
        }
        // Переходим к следующему стлобцу
        if (chek_colum)
            continue;
    }
    return false;
}

// Проверка коллизии с правой границей
static bool tetris_check_collision_right(void)
{
    // Цвет пикселя для чтения
    uint16_t color[10];
    // Координаты пикселя для чтения из памяти LCD
    lcd_position_t pixel;
    
    // Флаг проверки самого правого кубика в строке
    bool chek_row = false;
    
    // Проходимся справо-налево, снизу-вверх
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строка
    {
        for (int8_t j = tetris_figure.collum; j >= 0; j--)  // Столбец
        {
            if (tetris_shape[i][j] == 1)
            {
                    // Проверка боковых 10-ти пикселей 
                // Назначение координат столбца пикселей
                pixel.x1 = pixel.x2 = tetris_figure.x + j * 10 + 11;
                pixel.y1 = tetris_figure.y + i * 10;
                pixel.y2 = pixel.y1 + 10;
                // Чтение цвета пикселей
                //lcd_read_color(pixel, color, 10);
                
                // Проверка цвета всех 10-ти боковых пикселей
                for (uint8_t k = 0; k < 10; k++)
                    // Если произошла коллизия
                    if (color[k] != LCD_COLOR_WHITE)
                        return true;
                
                // Крайний правый куб в строке проверен
                chek_row = true;
                
                // Проверенный пиксель - самый крайний в строке,
                   значит следующий столбец проверять не надо 
                // Переход к следующему столбцу
                break;
            }
        }
        // Переходим к следующей строке
        if (chek_row)
            continue;
    }
    return false;
}

// Проверка коллизии с левой границей
static bool tetris_check_collision_left(void)
{
    // Цвет пикселя для чтения
    uint16_t color[10];
    // Координаты пикселя для чтения из памяти LCD
    lcd_position_t pixel;
    
    // Флаг проверки самого левого кубика в строке
    bool chek_row = false;
    
    // Проходимся слево-направо, снизу-вверх
    for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строка
    {
        for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбец
        {
            if (tetris_shape[i][j] == 1)
            {
                    // Проверка боковых 10-ти пикселей 
                // Назначение координат столбца пикселей
                pixel.x1 = pixel.x2 = tetris_figure.x + j * 10 - 1;
                pixel.y1 = tetris_figure.y + i * 10;
                pixel.y2 = pixel.y1 + 10;
                // Чтение цвета пикселей
                //lcd_read_color(pixel, color, 10);
                
                // Проверка цвета всех 10-ти боковых пикселей
                for (uint8_t k = 0; k < 10; k++)
                    // Если произошла коллизия
                    if (color[k] != LCD_COLOR_WHITE)
                        return true;
                
                // Крайний левый куб в строке проверен
                chek_row = true;
                
                // Проверенный пиксель - самый крайний в строке,
                   значит следующий столбец проверять не надо 
                // Переход к следующему столбцу
                break;
            }
        }
        // Переходим к следующей строке
        if (chek_row)
            continue;
    }
    return false;
}

// Проверка коллизий
static bool tetris_check_collision(tetris_offset_timer_cb_t offset)
{
    // Флаг коллизии
    bool collision_flag = false;
    
    switch(offset)
    {
        case TETRIS_OFFSET_DOWN:
            collision_flag = tetris_check_collision_down();
            break;
        
        case TETRIS_OFFSET_RIGHT:
            collision_flag = tetris_check_collision_right();
            break;
        
        case TETRIS_OFFSET_LEFT:
            collision_flag = tetris_check_collision_left();
            break;
        
        default:
            assert(false);
    }
    
    if (!collision_flag)
        return false;
    
    // Если произошла коллизия с нижней границей
    if (offset == TETRIS_OFFSET_DOWN)
    {
        // Установка флага необходимости создания новой фигуры
        tetris_get_new_figure_needed = true;
        return true;
    }
    
    // Если произошла коллизия с боковыми границами
    if ((offset == TETRIS_OFFSET_LEFT) || (offset == TETRIS_OFFSET_RIGHT))
        return true;
    
    return false;
}

// Переотрисовка фигуры с учётом смещения
static void tetris_redrawing_figure(tetris_offset_timer_cb_t offset)
{
    // Координаты фигуры для отрисовки
    lcd_position_t figure;
    
    // Если смещаем вниз
    if (offset == TETRIS_OFFSET_DOWN)
    {
        for (uint8_t i = 0; i < tetris_figure.row; i++)         // Строки
        {
            for (uint8_t j = 0; j < tetris_figure.collum; j++)  // Столбцы
            {
                if (tetris_shape[i][j] == 1)
                {
                    // Координаты смещения по осям
                    uint16_t offset_x = tetris_figure.x + j * 10;
                    uint16_t offset_y = tetris_figure.y + i * 10;
                    
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
        tetris_figure.y -= TETRIS_OFFSET_Y;
        return;
    }

    // Если смещаем вправо
    if (offset == TETRIS_OFFSET_RIGHT)
    {
        for (uint8_t i = 0; i < tetris_figure.row; i++)
        {
            for (int8_t j = tetris_figure.collum; j >= 0; j--)
            {
                if (tetris_shape[i][j] == 1)
                {
                    // Очищаем область
                    tetris_draw_cube(i * 10, j * 10, LCD_COLOR_WHITE);
                    // Вызов отрисовки одного куба фигуры
                    tetris_draw_cube(i * 10, (j + 1) * 10, tetris_figure.color);
                }  
            }
        }
        
        // Изменяем координату фигуры
        tetris_figure.x += TETRIS_OFFSET_X;
        return;
    }
    
    // Если смещаем влево
    if (offset == TETRIS_OFFSET_LEFT)
    {
        // Изменяем координату фигуры
        tetris_figure.x -= TETRIS_OFFSET_X;
        
        for (uint8_t i = 0; i < tetris_figure.row;  i++)
        {
            for (uint8_t j = 0; j < tetris_figure.collum; j++)
            {  
                if (tetris_shape[i][j] == 1)
                {
                    // Очищаем область
                    tetris_draw_cube(i * 10, (j + 1) * 10, LCD_COLOR_WHITE);
                    // Вызов отрисовки одного куба фигуры
                    tetris_draw_cube(i * 10, j * 10, tetris_figure.color);
                }  
            }
        }
        return;
    }
}

// Смещение фигуры по осям с учётом коллизий
static void tetris_offset_timer_cb(timer_t *timer)
{
    // Ассерт на нулевой указатель таймера
    ASSERT_NULL_PTR(timer);
    
    // Если кнопка ВНИЗ нажата
    if (tetris_button_down_pressed)
    {
        // Если не произошла коллизия
        if (!tetris_check_collision(TETRIS_OFFSET_DOWN))
        {
            // Перерисовываем фигуру
            tetris_redrawing_figure(TETRIS_OFFSET_DOWN);
            // Запуск таймера для задержки обработки непрерывно нажатой кнопки
            timer_start(&tetris_offset_timer_cb_timer, TIMER_TICKS_MS(5));
        }
    }
    
    // Если нажаты обе кнопки - выход без обработки их одновременного нажатия
    if (tetris_button_left_pressed & tetris_button_right_pressed)
        return;
    
    // Если кнопка ВЛЕВО нажата
    if (tetris_button_left_pressed)
    {
        // Если не произошла коллизия
        if (!tetris_check_collision(TETRIS_OFFSET_LEFT))
        {
            // Перерисовываем фигуру
            tetris_redrawing_figure(TETRIS_OFFSET_LEFT);
            // Запуск таймера для задержки обработки непрерывно нажатой кнопки
            timer_start(&tetris_offset_timer_cb_timer, TIMER_TICKS_MS(120));
        }
    }
    
    // Если кнопка ВПРАВО нажата
    if (tetris_button_right_pressed)
    {
        // Если не произошла коллизия
        if (!tetris_check_collision(TETRIS_OFFSET_RIGHT))
        {
            // Перерисовываем фигуру
            tetris_redrawing_figure(TETRIS_OFFSET_RIGHT);
            // Запуск таймера для задержки обработки непрерывно нажатой кнопки
            timer_start(&tetris_offset_timer_cb_timer, TIMER_TICKS_MS(120));
        }
    }
}

// Проверка коллизий при повороте фигуры
static bool tetris_check_collision_rotate(void)
{
    // Временный массив формы фигуры
    bool temp_shape[5][5];
    
    // Поворачиваем временную фигуру
    for (uint8_t i = 0; i < tetris_figure.row; i++)
        for (uint8_t j = 0; j < tetris_figure.collum; j++)
            if (tetris_shape[i][j] == 1)
                // Поворачиваем относительно текущих размеров
                temp_shape[j][tetris_figure.row - 1 - i] = 1;
    
    // Проходимся по всей временной фигуре
    for (uint8_t i = 0; i < tetris_figure.row; i++)
        for (uint8_t j = 0; j < tetris_figure.collum; j++)
            if (temp_shape[i][j] != tetris_shape[i][j])
            {
                // Цвет пикселя для чтения
                uint16_t color;
                // Координаты пикселя для чтения из памяти LCD
                lcd_position_t pixel;
                
                 // Проверка боковых 10-ти пикселей 
                // Назначение координат столбца пикселей
                pixel.x1 = pixel.x2 = tetris_figure.x + j * 10 + 5;
                pixel.y1 = pixel.y2 = tetris_figure.y + i * 10 + 5;
                // Чтение цвета пикселей
                //lcd_read_color(pixel, &color, 1);
                
                // Если произошла коллизия
                if (color != LCD_COLOR_WHITE)
                    return true;
            }
    return false;
}

// Вращение фигуры
static void tetris_rotate_figure(void)
{
    // Если кнопка не нажата - выход сразу
    if (!tetris_button_rotate_pressed)
        return;
    
    // Если при повороте произойдёт коллизия - выход без поворота
    if (tetris_check_collision_rotate())
        return;
    
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
            {
                tetris_shape[i][j] = 0;
                tetris_draw_cube(i * 10, j * 10, LCD_COLOR_WHITE);
            }
        }
    }

    // Поворачиваем фигуру на 90 градусов по часовой стрелке
    for (uint8_t i = 0; i < tetris_figure.row; i++)
    {
        for (uint8_t j = 0; j < tetris_figure.collum; j++)
        {
            if (temp_shape[i][j] == 1)
            {
                // Поворачиваем относительно текущих размеров
                tetris_shape[j][tetris_figure.row - 1 - i] = 1;
                tetris_draw_cube(j * 10, (tetris_figure.row - 1 - i) * 10, tetris_figure.color);
            }
        }
    }
    
    // Обновляем размеры после поворота
    uint8_t temp_size = tetris_figure.collum;
    tetris_figure.collum = tetris_figure.row;
    tetris_figure.row = temp_size;
}

*/
