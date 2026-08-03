/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BME280_addr (0x76 << 1)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

uint8_t config_hum = 0x01;
uint8_t config_pres_temp = 0x27;


HAL_StatusTypeDef ret;

uint8_t sensor_data[8];

int32_t pres_data;
int32_t temp_data;
uint16_t hum_data;

float pressure;
float temperature;
float humidity;

//Calibration parameter variables
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;

uint8_t dig_H1;
int16_t dig_H2;
uint8_t dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t dig_H6;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

int32_t  BME280_compensate_T_int32(int32_t adc_T);
uint32_t BME280_compensate_P_int64(int32_t adc_P);
uint32_t BME280_compensate_H_int32(int32_t adc_H);

HAL_StatusTypeDef BME280_init(void);
HAL_StatusTypeDef BME280_calibration_parameters(void);

void BME280_ReadData(void);
void Display_Update(void);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  BME280_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  BME280_ReadData();

	  Display_Update();

	  HAL_Delay(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

HAL_StatusTypeDef  BME280_init(){

	HAL_StatusTypeDef ret;

	ret = HAL_I2C_Mem_Write(&hi2c1, BME280_addr, 0xF2, I2C_MEMADD_SIZE_8BIT, &config_hum, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("Write Error for humidity sensor\r\n");
		return ret;
	}
	ret = HAL_I2C_Mem_Write(&hi2c1, BME280_addr, 0xF4, I2C_MEMADD_SIZE_8BIT, &config_pres_temp, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("Write Error for temperature & pressure sensor\r\n");
		return ret;
	}

	return BME280_calibration_parameters();

}

void BME280_ReadData(){

	ret = HAL_I2C_Mem_Read(&hi2c1, BME280_addr, 0xF7, I2C_MEMADD_SIZE_8BIT, sensor_data, 8, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("Read Error\r\n");
		return;
	}

	pres_data = ((int32_t)sensor_data[0] << 12 ) | ((int32_t)sensor_data[1] <<4 ) | ((int32_t)sensor_data[2] >> 4 );

	temp_data = ((int32_t)sensor_data[3] << 12)| ((int32_t)sensor_data[4] <<4)  | ((int32_t)sensor_data[5] >> 4 );

	hum_data = ((uint32_t)sensor_data[6]<< 8) | ((uint32_t)sensor_data[7]);

	temperature = BME280_compensate_T_int32(temp_data) / 100.0f;
	pressure = BME280_compensate_P_int64(pres_data) / 25600.0f;
	humidity = BME280_compensate_H_int32(hum_data) / 1024.0f;

}

void Display_Update(){

	printf("Pressure: %.2f hPa\r\n", pressure);
	printf("Temperature: %.2f °C\r\n", temperature);
	printf("Humidity: %.2f %%RH\r\n\n", humidity);

}


int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

//Storing the hard-coded calibration parameters from bme280 into usable variables
HAL_StatusTypeDef BME280_calibration_parameters(){

uint8_t calibration1[26];
uint8_t calibration2[7];

HAL_StatusTypeDef ret;


ret = HAL_I2C_Mem_Read(&hi2c1, BME280_addr, 0x88, I2C_MEMADD_SIZE_8BIT, calibration1, sizeof(calibration1), HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("Calibration parameter set 1 Read Error\r\n");
		return ret;
	}
ret = HAL_I2C_Mem_Read(&hi2c1, BME280_addr, 0xE1, I2C_MEMADD_SIZE_8BIT, calibration2, sizeof(calibration2), HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("Calibration parameter set 2 Read Error\r\n");
		return ret;
	}

dig_T1 = ((uint16_t)calibration1[1] << 8) | calibration1[0];
dig_T2 = (int16_t)(((uint16_t)calibration1[3] << 8) | calibration1[2]);
dig_T3 = (int16_t)(((uint16_t)calibration1[5] << 8) | calibration1[4]);

dig_P1 = ((uint16_t)calibration1[7] << 8) | calibration1[6];
dig_P2 = (int16_t)(((uint16_t)calibration1[9] << 8) | calibration1[8]);
dig_P3 = (int16_t)(((uint16_t)calibration1[11] << 8) | calibration1[10]);
dig_P4 = (int16_t)(((uint16_t)calibration1[13] << 8) | calibration1[12]);
dig_P5 = (int16_t)(((uint16_t)calibration1[15] << 8) | calibration1[14]);
dig_P6 = (int16_t)(((uint16_t)calibration1[17] << 8) | calibration1[16]);
dig_P7 = (int16_t)(((uint16_t)calibration1[19] << 8) | calibration1[18]);
dig_P8 = (int16_t)(((uint16_t)calibration1[21] << 8) | calibration1[20]);
dig_P9 = (int16_t)(((uint16_t)calibration1[23] << 8) | calibration1[22]);

dig_H1 = calibration1[25];
dig_H2 = (int16_t)(((uint16_t)calibration2[1] << 8) | calibration2[0]);
dig_H3 = calibration2[2];
dig_H4 = (int16_t)(((int16_t)(int8_t)calibration2[3] << 4) | (calibration2[4] & (0x0F)));
dig_H5 = (int16_t)(((int16_t)(int8_t)calibration2[5] << 4) | (calibration2[4] >> 4));
dig_H6 = (int8_t)calibration2[6];

printf("P1=%u P2=%d P3=%d\r\n", dig_P1, dig_P2, dig_P3);
printf("P4=%d P5=%d P6=%d\r\n", dig_P4, dig_P5, dig_P6);
printf("P7=%d P8=%d P9=%d\r\n", dig_P7, dig_P8, dig_P9);

printf("H1=%u H2=%d H3=%u H4=%d H5=%d H6=%d\r\n\n", dig_H1, dig_H2, dig_H3, dig_H4, dig_H5, dig_H6);

return HAL_OK;
}

// Returns temperature in DegC, resolution is 0.01 DegC.
// Output value of "5123" equals 51.23 DegC.
// t_fine carries fine temperature as global value.

int32_t t_fine;

int32_t BME280_compensate_T_int32(int32_t adc_T)
{
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) *
            ((int32_t)dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
             ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
             ((int32_t)dig_T3)) >> 14;

    t_fine = var1 + var2;

    T = (t_fine * 5 + 128) >> 8;

    return T;
}

// Returns pressure in Pa as unsigned 32-bit integer in Q24.8 format
// (24 integer bits and 8 fractional bits)
// Example:
// 24674867 represents 24674867 / 256 = 96386.2 Pa = 963.862 hPa

uint32_t BME280_compensate_P_int64(int32_t adc_P)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) +
           ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1) *
            ((int64_t)dig_P1)) >> 33;

    if (var1 == 0)
    {
        return 0; // Avoid division by zero
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) +
        (((int64_t)dig_P7) << 4);

    return (uint32_t)p;
}

// Returns humidity in %RH as unsigned 32-bit integer in Q22.10 format
// (22 integer bits and 10 fractional bits)
// Example:
// 47445 represents 47445 / 1024 = 46.333 %RH

uint32_t BME280_compensate_H_int32(int32_t adc_H)
{
    int32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r = (((((adc_H << 14) -
                   (((int32_t)dig_H4) << 20) -
                   (((int32_t)dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
                 (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +
                 ((int32_t)32768))) >> 10) +
                 ((int32_t)2097152)) *
                 ((int32_t)dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r -
                (((((v_x1_u32r >> 15) *
                (v_x1_u32r >> 15)) >> 7) *
                ((int32_t)dig_H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);

    v_x1_u32r = (v_x1_u32r > 419430400 ?
                419430400 : v_x1_u32r);

    return (uint32_t)(v_x1_u32r >> 12);
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
