/*******************************************************************************
  * File Name          : dcmotor.c
  * Description        : To control the spped of dc motor in trapezoid motion
  * Author:              Naren Subburaj
  * Date:                17/03/2022
  ******************************************************************************
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
int incrimentalPulse = 0;
int timeUs = 0;
int timeCounter = 0;
int flag = 0; 
int i = 0;

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
    htim5.Init.Prescaler = 500 - 1; // 50 000 000/500 = 1,00,000
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 1000 - 1; // max delay we get here is 10 milliseconds
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
    htim2.Init.Prescaler = 500 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 500 - 1;
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
  

   
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);//PA10 is set to IN1
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //PB3 is set to IN2
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);   //VS to MCU
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
        

        //Decrement the speed of the dc motor
        if(flag == 1){ //If flag is 1
                if(0 != (uint16_t)incrimentalPulse){ //Check whether the incrimentalpulse is not equal to 0
                   
                    incrimentalPulse -= 1; //incrimentalpulse by -1
                    TIM2->CCR1 = incrimentalPulse; //Set incrimentalpulse to capture compare
                    //printf("Motor is stopping.\n");
                }else if(0 == (uint16_t)incrimentalPulse){ //If the incrimentalpulse equal to 0
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);//PA10 is set to 0
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); //PB5 is set to 0
                        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //PB3 is set to 0
                        flag =0;
                        timeCounter=0;
                        incrimentalPulse=0;
                        HAL_TIM_Base_Stop_IT(&htim5); //Timer 5 interrupt is stopped
                }
        }else{
            //For incrementing the dc motor speed
            if (pulse != (uint16_t)incrimentalPulse) { //Check the pulse value is not equal to incrimentalpulse
            
            incrimentalPulse += 1; //Increase the incrimentalpulse by 1
            TIM2->CCR1 = incrimentalPulse; //Set incrimentalpulse to capture compare 
            //printf("Motor is speeding up.\n");
       
            //For staying in the speed for user time delay
            }else if(pulse == (uint16_t)incrimentalPulse){ //if the incrimentalpulse is equal to pulse
                
                if(timeUs > timeCounter){ //Check if the timerUS is greater than timer counter
                     timeCounter++; //Increment the timer counter
                //printf("The ISR has run %d times.\n",timeCounter);
                }else if((timeUs) == timeCounter){ //If the timer us is equal to timer counter
                    flag = 1; //Make flag as 1
                }
           
            }
        }
    }
}

// FUNCTION      : DirSelection
// DESCRIPTION   : Direction selection of the dc motor 
// RETURNS       : Nothing
void DIRSelection(int dir)
{
    switch(dir)
    {
    case 1:
    {

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
        break;
    }
    case 2:
    {   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);   
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
        break;
    }
    case 0:
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);   
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);//Channel 1 is stopped
        break;
    }
    default:
    {
        printf("Select direction as 1 - forward, 2 - Reverse, 0 - Brake\n");
    }
    }
}

// FUNCTION      : encoder_Init
// DESCRIPTION   : All the encoder elements are initialized
// RETURNS       : Nothing
void encoder_init(void)
{
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

//PA08 TIM1 CH1
//PA09 TIM1 CH2

GPIOA->MODER |= GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1;
GPIOA->AFR[1] |= GPIO_AFRH_AFRH0_0 | GPIO_AFRH_AFRH1_0;

TIM1->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
TIM1->CCMR1 &=~(TIM_CCMR1_IC1F) |~(TIM_CCMR1_IC2F);
TIM1->CCER &= ~(TIM_CCER_CC1P) |~(TIM_CCER_CC2P);
TIM1->CCER &= ~(TIM_CCER_CC1NP) |~(TIM_CCER_CC2NP);

TIM1->ARR = 1000;
// use rising/falling edges for both TI1 and TI2
TIM1->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
TIM1->CR1 |= TIM_CR1_CEN;
}


// FUNCTION      : encoderInit
// DESCRIPTION   : encoder initialize 
// RETURNS       : Nothing
ParserReturnVal_t encoderInit (int mode)
{
encoder_init();
return CmdReturnOk;
}
ADD_CMD("encoderinit", encoderInit, "Encoder Initialize")



// FUNCTION      : encoder_read
// DESCRIPTION   : read the encoder value
// RETURNS       : Nothing
ParserReturnVal_t encoder_read (int mode)
{
printf("Encoder: %lu\n", TIM1->CNT);
return CmdReturnOk;
}
ADD_CMD("encoder", encoder_read, "Encoder output in ticks")



// FUNCTION      : dutySelection
// DESCRIPTION   : Duty selection of the dc motor
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


// FUNCTION      : DCIniti
// DESCRIPTION   : Dc motor is intialized 
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
ADD_CMD("dcinit", DCIniti, "This function initialise the dc configuration")



// FUNCTION      : DCMotorConfig
// DESCRIPTION   : User enters 3 values from the command line. Those are Channel selection, duty and time
// RETURNS       : Nothing
ParserReturnVal_t DCMotorconfig(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

    uint32_t dir=0; //Direction value
    uint32_t duty=0;//Duty value
    uint32_t sec = 0;//Delay value

    fetch_uint32_arg(&dir);
    fetch_uint32_arg(&duty);
    fetch_uint32_arg(&sec);

    DIRSelection(dir);
    
    flag = 0;
    timeCounter = 0;
    timeUs = sec*100*2 ; //The delay time from user is multiplied by 100*2
    dutySelection(duty);
    timeCounter = 0;
    

  timerIntr();
  HAL_TIM_Base_Init(&htim5);    //Timer 5 interrupt is initialized 
  HAL_TIM_Base_Start_IT(&htim5);//Timer 5 interrupt is started  
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