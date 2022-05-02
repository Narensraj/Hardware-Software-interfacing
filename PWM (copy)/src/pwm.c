/* example.c:
 *
 *
 *   
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "common.h"

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim2;
uint16_t pulse = 0;
  int count  = 0; 
int myindex[1000];
int incrimentalpulse = 0;
int decrimentalpulse = 0;
int timeUS = 0;
int timerCounter = 0;
int flag = 0; 
int flag1 = 0; 
int i = 0;
int start = 5;
uint32_t myticks = 0;

void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim5);
}

// FUNCTION      : timerint
// DESCRIPTION   : Initialize the timer 
// RETURNS       : Nothing
void timerinit()
{
    RCC->APB1ENR |= (1 << 3); // Enable clock source for timer 5

    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 50 - 1; // 50 000 000/50 = 1,000,000
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 1000 - 1; // max delay we get here is 1 miliseconds
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim5);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    RCC->APB1ENR |= (1 << 0); // Enable clock source for timer 2
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 1000 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500 - 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
   

   
    GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  

   
    GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}

void us_delay1(int delay)
{
    TIM2->CNT = 0;
    // TIM2->CR1 |= (1<<0);          //start the timer
    while (TIM2->CNT < delay);
}

void ms_delay(uint16_t ms_delay)
{
    for (uint16_t i = 0; i < ms_delay; i++)
    {
        us_delay1(1000);
    }
}

// FUNCTION      : timerintr
// DESCRIPTION   : Initialize the timer interrupt function
// RETURNS       : Nothing
void timerIntr()
{
    HAL_NVIC_SetPriority(TIM5_IRQn, 0, 1); // Set inturrupt priority
    HAL_NVIC_EnableIRQ(TIM5_IRQn);

    HAL_TIM_Base_Start_IT(&htim5); // start timer with intrrupt
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim5)
    {

        // if(incrimentalpulse == pulse)
        // {
        //    TIM2->CCR1 = pulse;
        //     timerCounter++;
        //     printf("%d",timercounter);
        //     if(timeUS == timerCounter)
        //     {
        //     flag = 1;

        //     // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        //     // HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); 
        //     // printf("Motor Stopped after %d",timeUS);
        //     }
        //     else
        //     {
        //        flag = 0;
        //     }
        //             if(flag == 1)
        // {
        //     incrimentalpulse -=5;
        //     TIM2->CCR1 = incrimentalpulse;
        // } 

        // }
        // else
        // {
        //     //TIM2->CCR1 = incrimentalpulse;
        //     incrimentalpulse +=5;
        //     TIM2->CCR1 = incrimentalpulse;
        // }
        
            if(pulse != 0)
            {
            incrimentalpulse -=5;
            TIM2->CCR1 = incrimentalpulse;
            }
            else
            {
            //TIM2->CCR1 = pulse;
            }

        // decrimentalpulse = pulse;

        // if(pulse == 0)
        // {
            
        // }
    }
}


// FUNCTION      : pwmSelection
// DESCRIPTION   : PWM channel are selected
// RETURNS       : Nothing
void DIRSelection(int dir)
{
    switch(dir)
    {
    case 1:
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
        break;
    }
    case 2:
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);   
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
        break;
    }
    case 0:
    {
        flag1 = 1;
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);   
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
        break;
    }
    default:
    {
        printf("Select channel from 1 to 3\n");
    }
    }
}


// FUNCTION      : dutySelection
// DESCRIPTION   : duty values is recevied and printings its value in cmd line
// RETURNS       : Nothing
void dutySelection(uint32_t duty)
{
    if(duty > 0 || duty < 100) //Checks whether duty value is between 0 and 100
    {
    pulse = duty * 10; 
    printf("The entered duty value is %ld\n",duty);
    }
    else{
    printf("Enter the duty value between 0 to 100\n");
    }
}


// FUNCTION      : pwmInit
// DESCRIPTION   : PWM is intialized 
// RETURNS       : Nothing
ParserReturnVal_t DCIniti(int mode)
{
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;
    flag = 0;
    RCC->AHB1ENR |= (1 << 0);  // turn on the clock of port A
    GPIOA->MODER |= (1 << 10); // make PA5 pin as output
    GPIOA->MODER &= ~(1 << 3); // make PA0 pin as alternate
   

    timerinit();
    timerIntr();
    // TIM2->CR1 |= (1 << 0);     // start the timer

    return CmdReturnOk;
}
ADD_CMD("dcinit", DCIniti, "This function initialise the pwmconfiguration")



// FUNCTION      : pwmConfig
// DESCRIPTION   : User enters 2 values from the command line. Those are Channel selection and duty
// RETURNS       : Nothing
ParserReturnVal_t DCMotorconfig(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

    uint32_t dir=0;
    uint32_t duty=0;
    uint32_t sec = 0;
    fetch_uint32_arg(&dir);
    fetch_uint32_arg(&duty);
    fetch_uint32_arg(&sec);

    DIRSelection(dir);
    incrimentalpulse = duty*10;
    //flag = 0;
    timerCounter = 0;
    timeUS = sec*1000;
    dutySelection(duty);

    //flag = 1;

  timerIntr();
  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Start_IT(&htim5); 
  return CmdReturnOk;
}

ADD_CMD("dc",DCMotorconfig,"                Example Command")



ParserReturnVal_t countingArguments(int mode)
{
    uint32_t num = 0;
    // int cmdarg[5];
    int numargs = 0;
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;

    while (0 == fetch_uint32_arg(&num))
    {

        printf("num of args is %d\n", (int)num);
        numargs++;
    }

    return CmdReturnOk;
}

ADD_CMD("example", countingArguments, "                Example Command")