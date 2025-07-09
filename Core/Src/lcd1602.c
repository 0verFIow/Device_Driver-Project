#include "lcd1602.h"
#include "i2c_protocol.h"

void lcd1602_main(void);
void lcd1602_init(void);
void lcd1602_data(uint8_t data);
void Write_Data_to_DDRAM(uint8_t nibble);
void pulse_enable(void);
void Set_RS(uint8_t rs);
void lcd1602_move_cursor(uint8_t row, uint8_t column);


void lcd1602_main(void)
{
	uint8_t value=0;
	lcd1602_init();

	while(1)
	{
		lcd1602_move_cursor(0,0);
		lcd1602_string("Hello!");
		lcd1602_move_cursor(1, 0);
		lcd1602_data(value+'0');
		value++;
		if(value>9)value=0;
		HAL_Delay(500);
	}
}


void lcd1602_command(uint8_t command)
{
	uint8_t high_nibble, low_nibble;

	high_nibble = command >> 4;
	low_nibble = (command & 0x0f);

	// RAM에 데이터를 직접 넣는 방식
#if 0
	Set_RS(0);
	Write_Data_to_DDRAM(high_nibble);
	Write_Data_to_DDRAM(low_nibble);
#else
	//I2C를 구현해서 통신해야함

#endif
}

void lcd1602_data(uint8_t data)
{ // 1byte를 출력

	uint8_t high_nibble, low_nibble;

	high_nibble = data >> 4;
	low_nibble = (data & 0x0f);

#if 0
	// RAM에 데이터를 직접 넣는 방식
	Set_RS(1);
	Write_Data_to_DDRAM(high_nibble);
	Write_Data_to_DDRAM(low_nibble);
#else
	//I2C를 구현해서 통신해야함

#endif
}


void lcd1602_init(void) //초기화
{
	HAL_Delay(50);

	Write_Data_to_DDRAM(RETURN_HOME);
	HAL_Delay(2);
	Write_Data_to_DDRAM(RETURN_HOME);
	HAL_Delay(2);
	Write_Data_to_DDRAM(RETURN_HOME);
	HAL_Delay(2);
	Write_Data_to_DDRAM(0x02); // 4BIT MODE 설정

	lcd1602_command(FUNCTION_SET); //Function Set 4비트 모드에서 몇 줄로 표시할지와 폰트 종류 설정
	delay_us(40); // Function Set 최소 시간
	lcd1602_command(DISPLAY_ON);
	delay_us(40); // Display On 최소 시간
	lcd1602_command(ENTRY_MODE_SET);	//Entry mode set ( 커서만 오른쪽으로 이동 )
	delay_us(40); // Entry mode set 최소 시간
	lcd1602_command(CLEAR_DISPLAY);
	HAL_Delay(2); // Clear display 최소 시간
}

void lcd1602_string(uint8_t *str){ //string값 찍는 함수 , null을 만날 때 까지 출력
	while(*str)lcd1602_data(*str++);
}

void lcd1602_move_cursor(uint8_t row, uint8_t column){ // 커서 이동 함수
	lcd1602_command(0x80 | row<<6 | column);
	return;
}

//이건 RAM에 데이터를 직접 ACCESS 하는 방식
void Write_Data_to_DDRAM(uint8_t nibble)
{
	GPIOB->ODR = GPIOB->ODR & ~LCD_D4 | ((nibble & 0x01) ? LCD_D4 : 0);
	GPIOB->ODR = GPIOB->ODR & ~LCD_D5 | ((nibble & 0x02) ? LCD_D5 : 0);
	GPIOB->ODR = GPIOB->ODR & ~LCD_D6 | ((nibble & 0x04) ? LCD_D6 : 0);
	GPIOB->ODR = GPIOB->ODR & ~LCD_D7 | ((nibble & 0x08) ? LCD_D7 : 0);
	delay_us(37); // 최소 시간
	pulse_enable();
}

void pulse_enable(void)
{
    // EN 핀 제어: EN 핀에 펄스 생성
    HAL_GPIO_WritePin(GPIOB, LCD_EN, SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, LCD_EN, RESET);
    HAL_Delay(1);
}

void Set_RS(uint8_t rs)
{
	HAL_GPIO_WritePin(GPIOB, LCD_RS, (rs ? SET : RESET));
}

