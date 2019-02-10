/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "FreeRTOS.h"
#include "stm32f4xx.h"
#include "task.h"

TaskHandle_t xTaskHandle1= NULL;
TaskHandle_t xTaskHandle2= NULL;


void vTask1_handler(void *params);
void vTask2_handler(void *params);

#ifdef USE_SEMIHOSTING
extern void initialise_monitor_handles();
#endif

static void prvSetupHardware(void);
static void prvSetupUart(void);

char usr_msg[250];
void printmsg(char *msg);

#define TRUE 1
#define FALSE 0
#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE

//global var
uint8_t UART_ACCESS_KEY = AVAILABLE;

int main(void)
{

#ifdef USE_SEMIHOSTING
initialise_monitor_handles();
printf("Hello World example code\n");
#endif

	DWT->CTRL |= (1 << 0 );  //0000 0010 reg


	//reset the RCC cloc, HSI On reszta off
	RCC_DeInit();
	//update systemcoreClck
	SystemCoreClockUpdate();

	prvSetupHardware();
	sprintf(usr_msg, "Iza ;* \n");
	printmsg(usr_msg);



	//recording
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();
	//3

	xTaskCreate(vTask1_handler, "Task-1", configMINIMAL_STACK_SIZE,	NULL, 2, &xTaskHandle1 );
	xTaskCreate(vTask2_handler, "Task-2", configMINIMAL_STACK_SIZE,	NULL, 2, &xTaskHandle2 );


	//4
	vTaskStartScheduler();

	for(;;);
}


vTask1_handler (void *params){

	while(1){
		if(UART_ACCESS_KEY == AVAILABLE){
			UART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("Hello World Task-1\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}
	}

}


vTask2_handler (void *params){

	while(1){
		if(UART_ACCESS_KEY == AVAILABLE){
			UART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("Hello World Task-2\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}

	}

}

static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;


		//1 UART Enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins));

	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);


	memset(&uart2_init, 0, sizeof(uart2_init));

	uart2_init.USART_BaudRate=115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &uart2_init);


	//enable UART to gpio
	USART_Cmd(USART2, ENABLE);

}



static void prvSetupHardware(void)
{
	prvSetupUart();

}

void printmsg(char *msg)
{
	for (uint32_t i=0; i<strlen(msg); i++){

		while ( USART_GetFlagStatus(USART2,USART_FLAG_TXE) !=SET );
		USART_SendData(USART2, msg[i]);
		//USART_GetFlagStatus(USART2, )
	}

}
