#include "main.h"
#include "extern.h"
#include "stm32f4xx_hal.h"

#define BMP180_ADDR           (0x77 << 1)  // BMP180의 I2C 7비트 주소
#define BMP180_CTRL_REG       0xF4         // 제어 레지스터 주소
#define BMP180_TEMP_CMD       0x2E         // 온도 측정 명령
