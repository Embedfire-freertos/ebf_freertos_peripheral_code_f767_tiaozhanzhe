/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 �̼�������
  *********************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32ȫϵ�п����� 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 
 
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "stm32f7xx.h"
#include "systemclock.h"
#include "bsp_led.h"
#include "bsp_key.h" 
#include "core_delay.h"   
#include "bsp_debug_usart.h"
#include "./lcd/bsp_lcd.h"
#include <stdlib.h>
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

/**************************** ������ ********************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */
static TaskHandle_t AppTaskCreate_Handle = NULL;/* ���������� */
static TaskHandle_t Test_Task_Handle = NULL;/* LED������ */
static TaskHandle_t KEY_Task_Handle = NULL;/* KEY������ */

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */


/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */


/*
*************************************************************************
*                             ��������
*************************************************************************
*/
static void AppTaskCreate(void);/* ���ڴ������� */

static void Test_Task(void* pvParameters);/* Test_Task����ʵ�� */
static void KEY_Task(void* pvParameters);/* KEY_Task����ʵ�� */

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */

void LCD_Test(void);
/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{	
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  /* ������Ӳ����ʼ�� */
  BSP_Init();
  
  printf("����һ��[Ұ��]-STM32ȫϵ�п�����-FreeRTOS�̼������̣�\n\n");
  printf("����KEY1�������񣬰���KEY2�ָ�����\n");
  
   /* ����AppTaskCreate���� */
  xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,  /* ������ں��� */
                        (const char*    )"AppTaskCreate",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )1, /* ��������ȼ� */
                        (TaskHandle_t*  )&AppTaskCreate_Handle);/* ������ƿ�ָ�� */ 
  /* ����������� */           
  if(pdPASS == xReturn)
    vTaskStartScheduler();   /* �������񣬿������� */
  else
    return -1;  
  
  while(1);   /* ��������ִ�е����� */    
}


/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
  
  taskENTER_CRITICAL();           //�����ٽ���
  
  /* ����Test_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )Test_Task, /* ������ں��� */
                        (const char*    )"Test_Task",/* �������� */
                        (uint16_t       )512,   /* ����ջ��С */
                        (void*          )NULL,	/* ������ں������� */
                        (UBaseType_t    )2,	    /* ��������ȼ� */
                        (TaskHandle_t*  )&Test_Task_Handle);/* ������ƿ�ָ�� */
  if(pdPASS == xReturn)
    printf("����Test_Task����ɹ�!\r\n");
  /* ����KEY_Task���� */
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,  /* ������ں��� */
                        (const char*    )"KEY_Task",/* �������� */
                        (uint16_t       )512,  /* ����ջ��С */
                        (void*          )NULL,/* ������ں������� */
                        (UBaseType_t    )3, /* ��������ȼ� */
                        (TaskHandle_t*  )&KEY_Task_Handle);/* ������ƿ�ָ�� */ 
  if(pdPASS == xReturn)
    printf("����KEY_Task����ɹ�!\r\n");
  
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  
  taskEXIT_CRITICAL();            //�˳��ٽ���
}



/**********************************************************************
  * @ ������  �� Test_Task
  * @ ����˵���� Test_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void Test_Task(void* parameter)
{	
  while (1)
  {
    LCD_Test(); 
  }
}

/**********************************************************************
  * @ ������  �� KEY_Task
  * @ ����˵���� KEY_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Task(void* parameter)
{	
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )
    {/* K1 ������ */
      printf("����Test_Task����\n");
      vTaskSuspend(Test_Task_Handle);/* ����LED���� */
      printf("����Test_Task����ɹ���\n");
    } 
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )
    {/* K2 ������ */
      printf("�ָ�Test_Task����\n");
      vTaskResume(Test_Task_Handle);/* �ָ�LED���� */
      printf("�ָ�Test_Task����ɹ���\n");
    }
    vTaskDelay(20);/* ��ʱ20��tick */
  }
}

/***********************************************************************
  * @ ������  �� BSP_Init
  * @ ����˵���� �弶�����ʼ�������а����ϵĳ�ʼ�����ɷ��������������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  *********************************************************************/
static void BSP_Init(void)
{
  /* ϵͳʱ�ӳ�ʼ����400MHz */
	SystemClock_Config();
  
  /* ��ʼ��SysTick */
  HAL_SYSTICK_Config( HAL_RCC_GetSysClockFreq() / configTICK_RATE_HZ );	

	/* Ӳ��BSP��ʼ��ͳͳ�����������LED�����ڣ�LCD�� */
    
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/* usart �˿ڳ�ʼ�� */
  DEBUG_USART_Config();

  /* KEY �˿ڳ�ʼ�� */
  Key_GPIO_Config();
		
	LCD_Init();
	/* LCD ��һ���ʼ�� */ 
	LCD_LayerInit(0, LCD_FB_START_ADDRESS,ARGB8888);
/* LCD �ڶ����ʼ�� */ 
	LCD_LayerInit(1, LCD_FB_START_ADDRESS+(LCD_GetXSize()*LCD_GetYSize()*4),ARGB8888);
	/* ʹ��LCD������������ */ 
	LCD_DisplayOn(); 

	/* ѡ��LCD��һ�� */
	LCD_SelectLayer(0);

	/* ��һ����������ʾȫ�� */ 
	LCD_Clear(LCD_COLOR_BLACK);  

	/* ѡ��LCD�ڶ��� */
	LCD_SelectLayer(1);

	/* �ڶ�����������ʾȫ�� */ 
	LCD_Clear(LCD_COLOR_TRANSPARENT);

	/* ���õ�һ�͵ڶ����͸����,��СֵΪ0�����ֵΪ255*/
	LCD_SetTransparency(0, 255);
	LCD_SetTransparency(1, 0);
}
 


/*���ڲ��Ը���Һ���ĺ���*/
void LCD_Test(void)
{
	/*��ʾ��ʾ����*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
    /* ѡ��LCD��һ�� */
    LCD_SelectLayer(0);
	
	/* ��������ʾȫ�� */
	LCD_Clear(LCD_COLOR_BLACK);	
	/*����������ɫ������ı�����ɫ(�˴��ı�������ָLCD�ı����㣡ע������)*/
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	/*ѡ������*/
	LCD_SetFont(&LCD_DEFAULT_FONT);

	LCD_DisplayStringLine(1,(uint8_t* )"BH 5.0 inch LCD para:");
	LCD_DisplayStringLine(2,(uint8_t* )"Image resolution:800x480 px");
	LCD_DisplayStringLine(3,(uint8_t* )"Touch pad:5 point touch supported");
	LCD_DisplayStringLine(4,(uint8_t* )"Use STM32-LTDC directed driver,");
	LCD_DisplayStringLine(5,(uint8_t* )"no need extern driver,RGB888,24bits data bus");
	LCD_DisplayStringLine(6,(uint8_t* )"Touch pad use IIC to communicate");
	//while(1)
	{		
	testCNT++;
	/*ʹ��c��׼��ѱ���ת�����ַ���*/
	sprintf(dispBuff,"Display value demo: testCount = %d ",testCNT);
	LCD_ClearLine(7);
	/*����������ɫ������ı�����ɫ(�˴��ı�������ָLCD�ı����㣡ע������)*/
	LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
	/*Ȼ����ʾ���ַ������ɣ���������Ҳ����������*/
	LCD_DisplayStringLine(7,(uint8_t* )dispBuff);


	/* ��ֱ�� */
	LCD_SetTextColor(LCD_COLOR_BLUE);

	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw line:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_DrawLine(50,250,750,250);  
	LCD_DrawLine(50,300,750,300);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_DrawLine(300,250,400,400);  
	LCD_DrawLine(600,250,600,400);

	vTaskDelay(1000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);


	/*������*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw Rect:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_DrawRect(200,250,200,100);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_DrawRect(350,250,200,50);

	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
	LCD_DrawRect(200,350,50,200);

	vTaskDelay(1000);


	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);


	/*������*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw Full Rect:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_FillRect(200,250,200,100);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_FillRect(350,250,200,50);

	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
	LCD_FillRect(200,350,50,200);

	vTaskDelay(1000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);

	/* ��Բ */
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw circle:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_RED);
	LCD_DrawCircle(200,350,50);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_GREEN);
	LCD_DrawCircle(350,350,75);

	vTaskDelay(1000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);


	/*���Բ*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Draw full circle:");

	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_BLACK);
	LCD_FillCircle(300,350,50);

	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_FillCircle(450,350,75);

	vTaskDelay(1000);

	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);

	LCD_ClearLine(8);
	
	/*͸��Ч�� ǰ�������*/
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_ClearLine(8);
	LCD_DisplayStringLine(8,(uint8_t* )"Transparency effect:");
	
	/*����ǰ���㲻͸����*/
	LCD_SetTransparency(1, 128);

    /* ѡ��LCD��һ�� */
    LCD_SelectLayer(1);
	
	/* ��������ʾȫ�� */
	LCD_Clear(LCD_COLOR_BLACK);	
	/*��ǰ����һ����ɫԲ*/
	LCD_SetColors(LCD_COLOR_RED,LCD_COLOR_TRANSPARENT);
	LCD_FillCircle(400,350,75);
	
	vTaskDelay(1000);
	
	/*͸��Ч�� ���������*/

	/* ѡ��LCD������ */
	LCD_SelectLayer(0);	
	LCD_Clear(LCD_COLOR_BLACK);		
	/*���ñ����㲻͸��*/
	LCD_SetTransparency(1, 0);
	

	/*�ڱ�����һ����ɫԲ*/
	LCD_SetColors(LCD_COLOR_GREEN,LCD_COLOR_BLACK);
	LCD_FillCircle(450,350,75);
	
	/*�ڱ�����һ����ɫԲ*/
	LCD_SetColors(LCD_COLOR_BLUE,LCD_COLOR_BLACK);
	LCD_FillCircle(350,350,75);
	
	vTaskDelay(1000);
	
	LCD_SetColors(LCD_COLOR_BLACK,LCD_COLOR_BLACK);
	LCD_FillRect(0,200,LCD_PIXEL_WIDTH,LCD_PIXEL_HEIGHT-200);
	}

}

/*********************************************END OF FILE**********************/

