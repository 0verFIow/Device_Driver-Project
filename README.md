# STM32F4 Device Driver Example

이 프로젝트는 **STM32F4 시리즈** MCU에서  
- **I2C LCD1602** (텍스트 LCD)
- **BMP180** (기압/온도 센서)
를 제어하는 C 예제 코드입니다.

---

## 📂 폴더 구성
- `Core/` : STM32CubeIDE 기본 코드 (main, init 등)
- `Drivers/` : HAL 및 사용자 드라이버
- `.ioc` : CubeMX 프로젝트 설정파일

---

## 📝 주요 파일
- **lcd1602.c / lcd1602.h**  
  → I2C LCD1602 드라이버 함수  
  → "Hello!" 등 텍스트 표시, 커서 이동 등
- **bmp180.c / bmp180.h**  
  → I2C BMP180 센서 드라이버  
  → 온도/기압 읽기 및 보정 계산

---

## 🚀 빌드 & 실행 방법
1. **STM32CubeIDE**로 프로젝트 오픈 (`.ioc` 파일 더블클릭)
2. MCU 보드(예: STM32F411) 연결
3. 빌드 후 다운로드(F7 → Ctrl+F11)
4. LCD 및 BMP180 센서가 I2C로 연결되어 있어야 정상 동작

---

## 🛠️ 연결 회로 예시
- LCD1602 (I2C)  
  - SDA, SCL: MCU의 I2C3 핀에 연결
- BMP180  
  - SDA, SCL: 같은 I2C3 핀에 병렬 연결

---

## 📋 참고사항
- LCD1602와 BMP180은 **동일 I2C 버스** 사용
- 코드 내에서 `HAL_Delay()`, `printf()` 등 STM32 HAL 함수 사용
- CubeMX로 핀맵, 클럭 등은 직접 설정 필요

---

## 💬 문의
- 작성자: OverFlow (이웅)
- 질문은 [Issues](https://github.com/OverFlow/Device_Driver-Project/issues)로 남겨주세요!
