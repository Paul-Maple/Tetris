    /*** Определение бит в регистрах LCD Дисплея ***/

    /* Memory Access Control register */
// Бит порядка адрессов строк
#define LCD_MAC_MY_POS      (7U)       
#define LCD_MAC_MY_MSK      (0x1 << LCD_MAC_MY_POS)
#define LCD_MAC_MY          (LCD_MAC_MY_MSK)                                    /*!< 0x10000000 */
// Бит порядка адрессов столбцов
#define LCD_MAC_MX_POS      (6U)
#define LCD_MAC_MX_MSK      (0x1 << LCD_MAC_MX_POS)
#define LCD_MAC_MX          (LCD_MAC_MX_MSK)                                    /*!< 0x01000000 */
// Бит обмена строк на столбцы
#define LCD_MAC_MV_POS      (5U)
#define LCD_MAC_MV_MSK      (0x1 << LCD_MAC_MV_POS)
#define LCD_MAC_MV          (LCD_MAC_MV_MSK)                                    /*!< 0x00100000 */
// Направление обновления дисплея (Начало сверху/снизу)
#define LCD_MAC_ML_POS      (4U)
#define LCD_MAC_ML_MSK      (0x1 << LCD_MAC_ML_POS)
#define LCD_MAC_ML          (LCD_MAC_ML_MSK)                                    /*!< 0x00010000 */
// Бит цветового фильтра (RGB/BGR)
#define LCD_MAC_BGR_POS     (3U) 
#define LCD_MAC_BGR_MSK     (0x1 << LCD_MAC_BGR_POS)
#define LCD_MAC_BGR         (LCD_MAC_BGR_MSK)                                   /*!< 0x00001000 */
// Бит направления обновления дисплея (Начало спрва/слева)
#define LCD_MAC_MH_POS      (2U)
#define LCD_MAC_MH_MSK      (0x1 << LCD_MAC_MH_POS)
#define LCD_MAC_MH          (LCD_MAC_MH_MSK)                                    /*!< 0x00000100 */

    /* Interface Control register */
// Способ контроля записи
#define LCD_IC1_WEMODE_POS       (0U)
#define LCD_IC1_WEMODE_MSK       (0x1 << LCD_IC1_WEMODE_POS)
#define LCD_IC1_WEMODE           (LCD_IC1_WEMODE_MSK)
// Метод передачи данных в кадровую память дисплея
#define LCD_IC2_MTD_1_POS        (0U)
#define LCD_IC2_MTD_1_MSK        (0x0 << LCD_IC2_MTD_1_POS)
#define LCD_IC2_MTD_1            (LCD_IC2_MTD_1_MSK)
#define LCD_IC2_MTD_2_POS        (1U)
#define LCD_IC2_MTD_2_MSK        (0x1 << LCD_IC2_MTD_2_POS)
#define LCD_IC2_MTD_2            (LCD_IC2_MTD_2_MSK)
// Режим работы с внешним интерфейсом дисплея
#define LCD_IC3_DM_POS           (2U)
#define LCD_IC3_DM_RGB_MSK       (0x1 << LCD_IC3_DM_POS)
#define LCD_IC3_DM_RGB           (LCD_IC3_DM_RGB_MSK)
// Интерфейс доступа к GRAM дисплея
#define LCD_IC3_RM_POS           (1U)
#define LCD_IC3_RM_RGB_MSK       (0x1 << LCD_IC3_RM_POS)
#define LCD_IC3_RM_RGB           (LCD_IC3_RM_RGB_MSK)

    /*  Pixel format register */
// Формат пикселя
#define LCD_PIXEL_16_BIT       0x55
