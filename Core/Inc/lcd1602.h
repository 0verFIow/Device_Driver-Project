#include "main.h"

#define CLEAR_DISPLAY 0x01
#define RETURN_HOME 0x03
#define DISPLAY_ON 0x0C
#define ENTRY_MODE_SET 0x06
#define FUNCTION_SET 0x28

#define LCD_RS GPIO_PIN_0
#define LCD_EN GPIO_PIN_1
#define LCD_D4 GPIO_PIN_2
#define LCD_D5 GPIO_PIN_3
#define LCD_D6 GPIO_PIN_5
#define LCD_D7 GPIO_PIN_6


#define SET 1
#define RESET 0
