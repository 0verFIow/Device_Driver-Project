#include "main.h"
#include "bmp180.h"

extern I2C_HandleTypeDef hi2c3;

// BMP180 I2C 주소
#define I2C_BMP_ADDRESS 0x77 << 1

void i2c_bmp180_main(void);
void bmp180_read_calibration_data(void);
uint16_t read_uncompensated_temperature_value(void);
uint32_t read_uncompensated_pressure_value(uint8_t oss);
int32_t bmp180_calculate_temperature(uint16_t UT);
int32_t bmp180_calculate_pressure(uint32_t UP, uint8_t oss);

int16_t AC1, AC2, AC3;
uint16_t AC4, AC5, AC6;
int16_t B1, B2;
int16_t MB, MC, MD;

// 온도 보정 계산 결과(b5)는 압력 보정에 사용되므로 전역 변수로 선언
static int32_t b5 = 0;

void i2c_bmp_main(void)
{
	bmp180_read_calibration_data();
    while(1)
    {
    	//uint16_t UT = read_uncompensated_temperature_value();
    	uint32_t UP = read_uncompensated_pressure_value(0); // OSS는 0

    	printf("AC1:%d,AC2:%d,AC3:%d,AC4:%d,AC5:%d,AC6:%d\n",AC1,AC2,AC3,AC4,AC5,AC6);
    	printf("B1:%d,B2:%d,MB:%d,MC:%d,MD:%d\n",B1,B2,MB,MC,MD);
    	//int32_t temperature = bmp180_calculate_temperature(UT);
    	int32_t pressure = bmp180_calculate_pressure(UP,0); // OSS값이 필요해서

    	//printf("Temp: %d.%d C, Pressure: %ld Pa\n", temperature/10, temperature%10, pressure);
    	printf("Pressure: %ld Pa\n",pressure); // 압력만
    	//printf("Temp: %d.%d C\n",temperature/10, temperature%10); // 온도만
        HAL_Delay(3000);  // 3초 대기
    }
}

// unit16_t는 UT가 16비트이기 때문에
uint16_t read_uncompensated_temperature_value(void)
{
	uint8_t tempCmd = BMP180_TEMP_CMD;
	 uint8_t buffer[2];
	 uint16_t UT = 0;

	// BMP180_CTRL_REG에 BMP180_TEMP_CMD(0x2E) 명령어 쓰기
	 if (HAL_I2C_Mem_Write(&hi2c3, BMP180_ADDR, BMP180_CTRL_REG, I2C_MEMADD_SIZE_8BIT, &tempCmd, 1, 100) == HAL_OK)
	 {
	     HAL_Delay(5);
	 }
	else
	{
		printf("error\n");
	}

	// 0xF6 레지스터부터 2바이트 읽기 (0xF6: MSB, 0xF7: LSB)
	if(HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, 0xF6, I2C_MEMADD_SIZE_8BIT,buffer, 2, 100) == HAL_OK)
	{
		// 읽어온 데이터를 합쳐 16비트 값으로 변환 (MSB << 8 | LSB)
		UT = ((uint16_t)buffer[0] << 8) | buffer[1];
	}
	else
	{
		// 에러 발생 시 처리 코드 (예: 에러 리턴 값 설정)
		UT = 0;
	}

	return UT;
}

//uin32_t는 UP가 16~19BIT이기 때문에
uint32_t read_uncompensated_pressure_value(uint8_t oss)
{
	uint8_t cmd = 0x34 + (oss << 6);
	uint32_t UP = 0;
	uint8_t buffer[3];

	// BMP180_CTRL_REG에 oss의 값을 넣기
	if (HAL_I2C_Mem_Write(&hi2c3, BMP180_ADDR, BMP180_CTRL_REG, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 100) == HAL_OK)
		{
		 // oversampling 설정에 따른 적절한 대기 시간 설정
			switch (oss)
			{
				case 0:
					HAL_Delay(5);  // 최소 4.5ms, 여유있게 5ms
					break;
				case 1:
					HAL_Delay(8);  // 최소 7.5ms, 여유있게 8ms
					break;
				case 2:
					HAL_Delay(14); // 최소 13.5ms, 여유있게 14ms
					break;
				case 3:
					HAL_Delay(26); // 최소 25.5ms, 여유있게 26ms
					break;
				default:
					HAL_Delay(5);
					break;
			}
		}
	else
	    {
			printf("error\n");
	    }

	if(HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, 0xF6, I2C_MEMADD_SIZE_8BIT,buffer, 3, 100) == HAL_OK)
	{
		// 읽어온 데이터를 합쳐 16비트 값으로 변환 (MSB << 8 | LSB)
		UP = ((uint16_t)buffer[0] << 16 | (uint16_t)buffer[1] << 8 | buffer[2]) >> (8-oss);
	}
	else
	{
		// 에러 발생 시 처리 코드 (예: 에러 리턴 값 설정)
		UP = 0;
	}
	return UP;
}

int32_t bmp180_calculate_temperature(uint16_t UT)
{
    int32_t X1, X2, T;

    X1 = (((int32_t)UT - AC6) * AC5) >> 15;
    X2 = ((int32_t)MC << 11) / (X1 + MD);
    b5 = X1 + X2;
    T = (b5 + 8) >> 4;  // 온도: 0.1°C 단위
    printf("X1:%d,X2:%d,b5:%d,T:%d\n",X1,X2,b5,T);
    return T;
}

int32_t bmp180_calculate_pressure(uint32_t UP, uint8_t oss)
{
    int32_t B6, X1, X2, X3, P;
    uint32_t B3, B4, B7;

    B6 = b5 - 4000;
    X1 = (B2 * ((B6 * B6) >> 12)) >> 11;
    X2 = (AC2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = ((((int32_t)AC1) * 4 + X3) << oss) + 2;
    B3 = B3 >> 2;
    printf("B6:%d,X1:%d,X2:%d,X3:%d,B3:%d\n",B6,X1,X2,X3,B3);

    X1 = (AC3 * B6) >> 13;
    X2 = (B1 * ((B6 * B6) >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    B4 = (AC4 * (uint32_t)(X3 + 32768)) >> 15;

    B7 = ((uint32_t)UP - B3) * (50000 >> oss);
    if(B7 < 0x80000000)
    {
        P = (B7 * 2) / B4;
    }
    else
    {
        P = (B7 / B4) * 2;
    }
    printf("X1:%d,X2:%d,X3:%d,B4:%d,B7:%d,P=%d\n",X1,X2,X3,B4,B7,P);

    X1 = (P >> 8) * (P >> 8);
    printf("X1:%d\n",X1);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * P) >> 16;
    P = P + ((X1 + X2 + 3791) >> 4);
    printf("X1:%d,X2:%d,P=%d\n",X1,X2,P);
    return P;  // 압력: Pa 단위
}

void bmp180_read_calibration_data(void)
{
    uint8_t calib[22];

    // 0xAA 주소부터 22바이트 읽기
    if(HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, 0xAA, I2C_MEMADD_SIZE_8BIT, calib, 22, 1000) == HAL_OK)
    {
        AC1 = (int16_t)(calib[0] << 8 | calib[1]);
        AC2 = (int16_t)(calib[2] << 8 | calib[3]);
        AC3 = (int16_t)(calib[4] << 8 | calib[5]);
        AC4 = (uint16_t)(calib[6] << 8 | calib[7]);
        AC5 = (uint16_t)(calib[8] << 8 | calib[9]);
        AC6 = (uint16_t)(calib[10] << 8 | calib[11]);
        B1  = (int16_t)(calib[12] << 8 | calib[13]);
        B2  = (int16_t)(calib[14] << 8 | calib[15]);
        MB  = (int16_t)(calib[16] << 8 | calib[17]);
        MC  = (int16_t)(calib[18] << 8 | calib[19]);
        MD  = (int16_t)(calib[20] << 8 | calib[21]);
    }
    else // 예외처리
    {
    	printf("error\n");
    }
}



