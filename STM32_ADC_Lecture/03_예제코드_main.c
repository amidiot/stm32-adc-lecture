/**
 ******************************************************************************
 * @file    main.c (사용자 코드 부분만 발췌)
 * @brief   STM32 ADC 토양 수분 센서 예제
 * @board   NUCLEO-F401RE
 * @sensor  Capacitive Soil Moisture Sensor v1.2
 * @author  김윤회
 ******************************************************************************
 * 
 * [핀 연결]
 *   센서 VCC  → NUCLEO 3.3V
 *   센서 GND  → NUCLEO GND  
 *   센서 AOUT → NUCLEO A0 (PA0)
 *
 * [CubeMX 설정]
 *   - PA0: ADC1_IN0
 *   - ADC1 Resolution: 12 bits
 *   - UART2: 115200 baud (기본 설정)
 *
 ******************************************************************************
 */

/* ==========================================================================
   이 파일은 main.c에 추가해야 할 사용자 코드만 포함합니다.
   CubeMX가 생성한 코드의 해당 섹션에 복사하여 붙여넣으세요.
   ========================================================================== */


/* ---------------------------------------------------------------------------
   [1] USER CODE BEGIN Includes
   --------------------------------------------------------------------------- */
#include <stdio.h>


/* ---------------------------------------------------------------------------
   [2] USER CODE BEGIN PD (Private Defines)
   --------------------------------------------------------------------------- */
// 센서 캘리브레이션 값 - 실측 후 조정 필요!
#define ADC_DRY   3100    // 공기 중 (건조) - 높은 값
#define ADC_WET   1500    // 물에 담근 상태 (습윤) - 낮은 값


/* ---------------------------------------------------------------------------
   [3] USER CODE BEGIN PV (Private Variables) - 선택사항
   --------------------------------------------------------------------------- */
// 필요한 경우 전역 변수 선언


/* ---------------------------------------------------------------------------
   [4] USER CODE BEGIN 0 (사용자 함수 정의)
   --------------------------------------------------------------------------- */

/**
 * @brief  printf 리다이렉트 함수 (시리얼 출력용)
 * @note   이 함수가 있어야 printf가 UART로 출력됩니다
 */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
    return len;
}

/**
 * @brief  ADC 값을 수분 퍼센트(%)로 변환
 * @param  adc_value: ADC 원시값 (0~4095)
 * @return 수분 퍼센트 (0~100)
 * @note   건조할수록 ADC 값이 높고, 습할수록 낮음
 */
int getMoisturePercent(uint32_t adc_value)
{
    // 경계값 처리
    if (adc_value >= ADC_DRY) return 0;     // 완전 건조
    if (adc_value <= ADC_WET) return 100;   // 완전 습윤
    
    // 선형 보간으로 퍼센트 계산
    // (DRY에서 멀어질수록 = 습해질수록 퍼센트 증가)
    return (ADC_DRY - adc_value) * 100 / (ADC_DRY - ADC_WET);
}

/**
 * @brief  ADC 원시값 읽기 (폴링 방식)
 * @return ADC 변환값 (0~4095), 실패 시 0
 */
uint32_t readADC(void)
{
    uint32_t adc_value = 0;
    
    HAL_ADC_Start(&hadc1);
    
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
        adc_value = HAL_ADC_GetValue(&hadc1);
    }
    
    HAL_ADC_Stop(&hadc1);
    
    return adc_value;
}

/**
 * @brief  여러 번 측정 후 평균값 반환 (노이즈 감소)
 * @param  samples: 측정 횟수
 * @return 평균 ADC 값
 */
uint32_t readADC_Averaged(int samples)
{
    uint32_t sum = 0;
    
    for (int i = 0; i < samples; i++)
    {
        sum += readADC();
        HAL_Delay(10);  // 측정 간 짧은 딜레이
    }
    
    return sum / samples;
}


/* ---------------------------------------------------------------------------
   [5] USER CODE BEGIN WHILE (메인 루프)
   --------------------------------------------------------------------------- */

/* === 기본 버전 === */
while (1)
{
    // 1. ADC 변환 시작
    HAL_ADC_Start(&hadc1);
    
    // 2. 변환 완료 대기 (타임아웃: 100ms)
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
        // 3. ADC 값 읽기 (0 ~ 4095)
        uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
        
        // 4. 전압으로 변환 (V)
        float voltage = adc_value * 3.3f / 4095.0f;
        
        // 5. 수분 퍼센트 계산
        int moisture = getMoisturePercent(adc_value);
        
        // 6. 시리얼로 출력
        printf("ADC: %4lu | Voltage: %.2fV | Moisture: %3d%%\r\n",
               adc_value, voltage, moisture);
    }
    
    // 7. ADC 정지
    HAL_ADC_Stop(&hadc1);
    
    // 1초 대기
    HAL_Delay(1000);
    
    /* USER CODE END WHILE */
}


/* ---------------------------------------------------------------------------
   [6] 응용 예제: LED 경고 시스템
   --------------------------------------------------------------------------- */

/*
 * GPIO 설정 필요: PA5 (보드 내장 LED) -> GPIO_Output
 * 
 * CubeMX에서 PA5를 GPIO_Output으로 설정 후
 * User Label을 "LED_WARNING"으로 지정하면 아래처럼 사용 가능
 */

#define MOISTURE_THRESHOLD  30  // 수분 30% 이하면 경고

// while 루프 안에 추가
/*
int moisture = getMoisturePercent(adc_value);

if (moisture < MOISTURE_THRESHOLD)
{
    // 수분 부족! LED 켜기
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    printf("[WARNING] Low moisture! Please water the plant.\r\n");
}
else
{
    // 수분 충분, LED 끄기
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}
*/


/* ---------------------------------------------------------------------------
   [7] 응용 예제: 평균값 사용 (노이즈 감소)
   --------------------------------------------------------------------------- */

/*
// 10회 측정 평균값 사용
uint32_t adc_value = readADC_Averaged(10);

float voltage = adc_value * 3.3f / 4095.0f;
int moisture = getMoisturePercent(adc_value);

printf("ADC(avg): %4lu | Voltage: %.2fV | Moisture: %3d%%\r\n",
       adc_value, voltage, moisture);

HAL_Delay(1000);
*/


/* ---------------------------------------------------------------------------
   [8] 응용 예제: 상태 문자열 출력
   --------------------------------------------------------------------------- */

/*
const char* getMoistureStatus(int percent)
{
    if (percent >= 70) return "Very Wet (젖음)";
    if (percent >= 50) return "Moist (적정)";
    if (percent >= 30) return "Slightly Dry (약간 건조)";
    if (percent >= 10) return "Dry (건조)";
    return "Very Dry (매우 건조)";
}

// 사용 예:
printf("Status: %s\r\n", getMoistureStatus(moisture));
*/


/* ===========================================================================
   END OF USER CODE
   =========================================================================== */

