/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2021 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
//osThreadId defaultTaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
TaskHandle_t HPT_Handler;	//high priority task handler
TaskHandle_t MPT_Handler; //medium priority task handler
TaskHandle_t LPT_Handler; //low priority task handler
TaskHandle_t VLPT_Handler; //very low priority task handler
SemaphoreHandle_t CountingSem;

uint8_t resource[3] = {11, 22, 33};
uint8_t resource_index = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void HPT_Task(void* vParam);
void MPT_Task(void* vParam);
void LPT_Task(void* vParam);
void VLPT_Task(void* vParam);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)
	{
		HAL_Delay(50);
		// release the semaphore here
		/* The xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
		 it will get set to pdTRUE inside the interrupt safe API function if a
		 context switch is required. */
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		xSemaphoreGiveFromISR(CountingSem, &xHigherPriorityTaskWoken);  // ISR SAFE VERSION
		xSemaphoreGiveFromISR(CountingSem, &xHigherPriorityTaskWoken);  // ISR SAFE VERSION
		xSemaphoreGiveFromISR(CountingSem, &xHigherPriorityTaskWoken);  // ISR SAFE VERSION

		/* Pass the xHigherPriorityTaskWoken value into portEND_SWITCHING_ISR(). If
		 xHigherPriorityTaskWoken was set to pdTRUE inside xSemaphoreGiveFromISR()
		 then calling portEND_SWITCHING_ISR() will request a context switch. If
		 xHigherPriorityTaskWoken is still pdFALSE then calling
		 portEND_SWITCHING_ISR() will have no effect */

		portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
		
	}
}
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	CountingSem = xSemaphoreCreateCounting(3,0);	//maxCount = 3, initCount = 0
	if(CountingSem == NULL) printf("Create CountingSema fail!!!\n");
	else printf("Create CountingSema successfully!!!\n");
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	xTaskCreate(HPT_Task, "HPT", 128, NULL, 3, &HPT_Handler);
	xTaskCreate(MPT_Task, "MPT", 128, NULL, 2, &MPT_Handler);
	xTaskCreate(LPT_Task, "LPT", 128, NULL, 1, &LPT_Handler);
	xTaskCreate(VLPT_Task, "VLPT", 128, NULL, 0, &VLPT_Handler);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	printf("START\n");
	vTaskStartScheduler();
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */
void HPT_Task(void* vParam)
{
	uint8_t num_of_sema = 0;
	uint8_t task_resource = 0;
	
	//Give 3 sema at the beginning
	xSemaphoreGive(CountingSem);
	xSemaphoreGive(CountingSem);
	xSemaphoreGive(CountingSem);
	
	while(1)
	{
		printf("HPT task: acquire the Semaphore\n");
		num_of_sema = uxSemaphoreGetCount(CountingSem);
		printf("Tokens avaiable are: %d\n", num_of_sema);
		
		xSemaphoreTake(CountingSem, portMAX_DELAY);
		task_resource = resource[resource_index];
		printf("Data is %d, HPT not release semaphore\n\n", task_resource);
		
		resource_index++;
		if(resource_index > 2) resource_index = 0;
		
		vTaskDelay(2000);
	}
}
	
void MPT_Task(void* vParam)
{
	uint8_t num_of_sema = 0;
	uint8_t task_resource = 0;
	
	while(1)
	{
		printf("MPT task: acquire the Semaphore\n");
		num_of_sema = uxSemaphoreGetCount(CountingSem);
		printf("Tokens avaiable are: %d\n", num_of_sema);
		
		xSemaphoreTake(CountingSem, portMAX_DELAY);
		task_resource = resource[resource_index];
		printf("Data is %d, MPT not release semaphore\n\n", task_resource);
		
		resource_index++;
		if(resource_index > 2) resource_index = 0;
		
		vTaskDelay(2000);
	}
}
void LPT_Task(void* vParam)
{
	uint8_t num_of_sema = 0;
	uint8_t task_resource = 0;
	
	while(1)
	{
		printf("LPT task: acquire the Semaphore\n");
		num_of_sema = uxSemaphoreGetCount(CountingSem);
		printf("Tokens avaiable are: %d\n", num_of_sema);
		
		xSemaphoreTake(CountingSem, portMAX_DELAY);
		task_resource = resource[resource_index];
		printf("Data is %d, LPT not release semaphore\n\n", task_resource);
		
		resource_index++;
		if(resource_index > 2) resource_index = 0;
		
		vTaskDelay(2000);
	}	
}
void VLPT_Task(void* vParam)
{
	uint8_t num_of_sema = 0;
	uint8_t task_resource = 0;
	
	while(1)
	{
		printf("VLPT task: acquire the Semaphore\n");
		num_of_sema = uxSemaphoreGetCount(CountingSem);
		printf("Tokens avaiable are: %d\n", num_of_sema);
		
		xSemaphoreTake(CountingSem, portMAX_DELAY);
		task_resource = resource[resource_index];
		printf("Data is %d, VLPT not release semaphore\n\n", task_resource);
		
		resource_index++;
		if(resource_index > 2) resource_index = 0;
		
		vTaskDelay(500);
	}
}
/* USER CODE END 4 */

/* StartDefaultTask function */
//void StartDefaultTask(void const * argument)
//{

//  /* USER CODE BEGIN 5 */
////  /* Infinite loop */
////  for(;;)
////  {
////    osDelay(1);
////  }
//  /* USER CODE END 5 */ 
//}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
