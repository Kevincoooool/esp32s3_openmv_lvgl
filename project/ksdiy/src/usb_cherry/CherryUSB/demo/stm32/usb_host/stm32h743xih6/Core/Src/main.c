/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "usbh_core.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart1;

HCD_HandleTypeDef hhcd_USB_OTG_HS;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USB_OTG_HS_HCD_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1,(uint8_t*)&ch,1,1000);
  return ch;
}

void usb_hc_low_level_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /* USER CODE BEGIN USB_OTG_HS_MspInit 0 */

  /* USER CODE END USB_OTG_HS_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.PLL3.PLL3M = 25;
    PeriphClkInitStruct.PLL3.PLL3N = 240;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 5;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
  /** Enable USB Voltage detector
  */
    HAL_PWREx_EnableUSBVoltageDetector();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USB_OTG_HS GPIO Configuration
    PB15     ------> USB_OTG_HS_DP
    PB14     ------> USB_OTG_HS_DM
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_OTG2_FS;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
    /* USB_OTG_HS interrupt Init */
    HAL_NVIC_SetPriority(OTG_HS_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
  /* USER CODE BEGIN USB_OTG_HS_MspInit 1 */

  /* USER CODE END USB_OTG_HS_MspInit 1 */
}

/*����ΪNormal��cache������Ϊ write-back����������cache��
 *�ں��ʵ�ʱ��(��cache���Ծ����������ǿ�Ƹ���)�����ݸ��µ���Ӧ��SRAM�ռ�
 *�ر�ע�⣺���Ҫ�����������£�д֮��ҪSCB_CleanDCache��������ʱҪSCB_InvalidateDCache
 */
#define MPU_Normal_WB        0x00


/*����ΪNormal��cache������Ϊ write-back����������cache��
 *�ں��ʵ�ʱ��(��cache���Ծ����������ǿ�Ƹ���)�����ݸ��µ���Ӧ��SRAM�ռ�
 *�ر�ע�⣺���Ҫ�����������£�д֮��ҪSCB_CleanDCache��������ʱҪSCB_InvalidateDCache
 */
#define MPU_Normal_WBWARA    0x01  //�ⲿ���ڲ�д����д�����


/*����Ϊ normal��cache������Ϊ Write-through��������cache��ͬʱ��
 *������ͬʱд����Ӧ�������ַ�ռ�
 *�ر�ע�⣺���Ҫ�����������£�����ֱ�����ڴ�д���ݣ���������ʱҪSCB_InvalidateDCache
 */
#define MPU_Normal_WT         0x02


/*����Ϊ normal�����ù���,���û���
 */
#define MPU_Normal_NonCache   0x03


/*����Ϊ Device������������Ч�����ù���,���û���
 */
#define MPU_Device_NonCache   0x04

/**
  * @brief  ����MPU�������Ժʹ�С�Ĵ���ֵ
    * @param  Region            MPU��������ȡֵ��Χ��0��7��
    * @param  AccessPermission  ���ݷ���Ȩ�ޣ�ȡֵ��Χ��MPU_NO_ACCESS��MPU_PRIV_RO_URO��
    * @param  TypeExtField      �������� Cache ���ԣ�����ȡֵ��0��1��2����һ��ֻ��0��1
    *
  * @param  Address             MPU�����������ַ���ر�ע�����õ�Address��Ҫ��Size����
  * @param  Size                MPU���������С,����ȡֵ��MPU_1KB��MPU_4KB ...MPU_512MB��
    * @param  IsShareable       �����Ĵ洢�ռ��Ƿ���Թ���1=������0=��ֹ����
  * @param  IsCacheable         �����Ĵ洢�ռ��Ƿ���Ի��棬1=�����棬0=��ֹ���档
  * @param  IsBufferable        ʹ��Cache֮�󣬲�����write-through����write-back(bufferable)
    *                           1=�����壬����д��write-back����0=��ֹ���壬��ֱд��write-through����
  * @retval None
  */
static void BSP_MPU_ConfigRegion(uint8_t    Number,
                                 uint8_t    TypeExtField,
                                 uint32_t   Address,
                                 uint32_t   Size,
                                 uint8_t    IsBufferable,
                                 uint8_t    IsCacheable)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    /* ����MPU */
    HAL_MPU_Disable();

    /* ����MPU����*/
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = Address;                      //�������ַ��
    MPU_InitStruct.Size = Size;                                //Ҫ���õ�����������С��
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;  //���ݷ���Ȩ�������������û�����Ȩģʽ�Ķ�/д����Ȩ�ޡ�
    MPU_InitStruct.IsBufferable = IsBufferable;                //�����ǿɻ���ģ���ʹ�û�д���档 �ɻ��浫���ɻ��������ʹ��ֱд���ԡ�WB
    MPU_InitStruct.IsCacheable = IsCacheable;                  //�����Ƿ�ɻ���ģ�����ֵ�Ƿ���Ա����ڻ����С�//M7 �ں�ֻҪ������ Cache��read allocate ���ǿ�����
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;     //�����Ƿ�����ڶ������������֮�乲��H7 ��Ӧ�ñʼǶ���������ǿ������������ͬ�ڹر� Cache
    MPU_InitStruct.Number = Number;                            //����š�
    MPU_InitStruct.TypeExtField = TypeExtField;                //������չ�ֶΣ������������ڴ�������͡�
    MPU_InitStruct.SubRegionDisable = 0x00;                    //����������ֶΡ�
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;//ָ����ʽ���λ��0=����ָ����ʣ�1=��ָֹ�����

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* ����MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT); //��ʾ��ֹ�˱������������κ�δʹ�� MPU �������������ڴ��쳣 MemFault


}

void cpu_mpu_config(uint8_t Region, uint8_t Mode, uint32_t Address, uint32_t Size)
{
    switch (Mode)
    {
    case MPU_Normal_WB:
        /*write back,no write allocate */
        /* �����ڴ�ΪNormal����,���ù���, ��дģʽ����д���ȡ����*/
        BSP_MPU_ConfigRegion(Region, MPU_TEX_LEVEL0, Address, Size, MPU_ACCESS_BUFFERABLE, MPU_ACCESS_CACHEABLE);
        break;

    case MPU_Normal_WBWARA:
        /*write back,write and read allocate */
        /* �����ڴ�ΪNormal����,���ù���, ��дģʽ��д���ȡ����*/
        BSP_MPU_ConfigRegion(Region, MPU_TEX_LEVEL1, Address, Size, MPU_ACCESS_BUFFERABLE, MPU_ACCESS_CACHEABLE);
        break;

    case MPU_Normal_WT:
        /*write through,no write allocate */
        /* �����ڴ�ΪNormal����,���ù���, ֱдģʽ*/
        BSP_MPU_ConfigRegion(Region, MPU_TEX_LEVEL0, Address, Size, MPU_ACCESS_NOT_BUFFERABLE, MPU_ACCESS_CACHEABLE);
        break;

    case MPU_Normal_NonCache:
        /* �����ڴ�ΪNormal����,���ù������û���ģʽ*/
        BSP_MPU_ConfigRegion(Region, MPU_TEX_LEVEL1, Address, Size, MPU_ACCESS_NOT_BUFFERABLE, MPU_ACCESS_NOT_CACHEABLE);
        break;
    }

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();
  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

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
  MX_USART1_UART_Init();
  //MX_USB_OTG_HS_HCD_Init();
  /* USER CODE BEGIN 2 */

  cpu_mpu_config(0, MPU_Normal_NonCache, 0x24070000, MPU_REGION_SIZE_64KB);

  printf("Start usb host task...\r\n");
  extern void usbh_class_test(void);                   
  usbh_initialize();
  usbh_class_test();
  vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USB_OTG_HS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_HS_HCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_HS_Init 0 */

  /* USER CODE END USB_OTG_HS_Init 0 */

  /* USER CODE BEGIN USB_OTG_HS_Init 1 */

  /* USER CODE END USB_OTG_HS_Init 1 */
  hhcd_USB_OTG_HS.Instance = USB_OTG_HS;
  hhcd_USB_OTG_HS.Init.Host_channels = 16;
  hhcd_USB_OTG_HS.Init.speed = HCD_SPEED_FULL;
  hhcd_USB_OTG_HS.Init.dma_enable = ENABLE;
  hhcd_USB_OTG_HS.Init.phy_itface = USB_OTG_EMBEDDED_PHY;
  hhcd_USB_OTG_HS.Init.Sof_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.low_power_enable = DISABLE;
  hhcd_USB_OTG_HS.Init.use_external_vbus = DISABLE;
  if (HAL_HCD_Init(&hhcd_USB_OTG_HS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_HS_Init 2 */

  /* USER CODE END USB_OTG_HS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
