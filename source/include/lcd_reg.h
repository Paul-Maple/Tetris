    /*** Определение бит в регистре LCD_MAC ***/
// Бит порядка адрессов строк
#define LCD_MAC_MY_POS      7        
#define LCD_MAC_MY_MSK      (0x1 << LCD_MAC_MY_POS)
#define LCD_MAC_MY          LCD_MAC_MY_MSK                                      /*!< 0x10000000 */
// Бит порядка адрессов столбцов
#define LCD_MAC_MX_POS      6
#define LCD_MAC_MX_MSK      (0x1 << LCD_MAC_MX_POS)
#define LCD_MAC_MX          LCD_MAC_MX_MSK                                      /*!< 0x01000000 */
// Бит обмена строк на столбцы
#define LCD_MAC_MV_POS      5
#define LCD_MAC_MV_MSK      (0x1 << LCD_MAC_MV_POS)
#define LCD_MAC_MV          LCD_MAC_MV_MSK                                      /*!< 0x00100000 */
// Направление обновления дисплея (Начало сверху/снизу)
#define LCD_MAC_ML_POS      4
#define LCD_MAC_ML_MSK      (0x1 << LCD_MAC_ML_POS)
#define LCD_MAC_ML          LCD_MAC_ML_MSK                                      /*!< 0x00010000 */
// Бит цветового фильтра (RGB/BGR)
#define LCD_MAC_BGR_POS     3 
#define LCD_MAC_BGR_MSK     (0x1 << LCD_MAC_BGR_POS)
#define LCD_MAC_BGR         LCD_MAC_BGR_MSK                                     /*!< 0x00001000 */
// Бит направления обновления дисплея (Начало спрва/слева)
#define LCD_MAC_MH_POS      2
#define LCD_MAC_MH_MSK      (0x1 << LCD_MAC_MH_POS)
#define LCD_MAC_MH          LCD_MAC_MH_MSK                                      /*!< 0x00000100 */
// Reserved (non set)
#define LCD_MAC_D1_POS      1
#define LCD_MAC_D1_MSK      (0x0 << LCD_MAC_D1_POS)
#define LCD_MAC_D1          LCD_MAC_D1_MSK                                      /*!< 0x00000010 */
// Reserved (non set)
#define LCD_MAC_D2_POS      0
#define LCD_MAC_D2_MSK      (0x0 << LCD_MAC_D2_POS)
#define LCD_MAC_D2          LCD_MAC_D2_MSK                                      /*!< 0x00000001 */

    
