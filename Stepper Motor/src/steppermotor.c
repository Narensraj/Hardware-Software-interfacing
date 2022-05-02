/* steppermotor.c
 *
 * Author: Naren subburaj
 * Date:  10-02-2022
 *
 * this file takes two command timerinit and timer '0' 'delay'
 * timerinit function initialize the  timer2 and timer5
 * timer command takes two arguments first one to select between timer and counter
 * the other argument takes amount of delay for counter in us
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal_dac.h"
#include "stm32f4xx_hal_def.h"
#include "common.h"
#include "main.h"

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim1;
int stepCounter=0;
int usersteps=0;
uint32_t pinState=0;


void steppermotorinit (void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = (GPIO_PIN_6| GPIO_PIN_7 |GPIO_PIN_9 |	GPIO_PIN_8);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = (GPIO_PIN_6 | GPIO_PIN_10 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_3 | GPIO_PIN_9);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = (GPIO_PIN_7 |GPIO_PIN_6 | GPIO_PIN_9 | GPIO_PIN_8); 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate = 0;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,1); //RST as High
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,1); //PS pin
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,1); //OE off

	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,0); //STEP pin 

	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,1); ///FR high clockwise
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,1); //MD1
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,1);//MD2

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1); //ATT1
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,1); //ATT2

  HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9);//MONI

	printf("Stepper motor initialised successfully\n");
}



void TimerInit(void)
{
  GPIOC->MODER |= 0x00000000;    //make gpio port c as input
  RCC->AHB1ENR |= (1<<0);        //turn on the clock of port A
  GPIOA->MODER |= (1<<10);       //make PA5 pin as output 

  RCC->APB1ENR |= (1<<0);        // Enable clock source for timer 2
  RCC->APB1ENR |= (1<<3);        // Enable clock source for timer 5

  TIM2->PSC = 100-1;              // 50 000 000 / 100 = 1us
  TIM2->ARR = 1000-1;           // 65535/1 = 65 ms delay
  TIM2->CNT |= 0;                //set counter to 0          
  TIM2->CR1 |= (1<<0);           //start the timer
  while(!(TIM2->SR & (1<<0)));
  
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 10 -1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 100-1; //
  //htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim5); //Start the timer 5 
}




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
  //int i;
  //usersteps = 1600;
  if(htim == &htim5){
   	GPIOC->ODR ^= (1<<7); 
    stepCounter++; 
  
  

    //if((stepCounter) == (2*usersteps)){
    if((stepCounter) == (2*usersteps)){
      stepCounter=0;
      
      //HAL_TIM_Base_Stop_IT(&htim5);        //start timer with intrrupt
     // HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,0);
    }

  }
}


void us_delay(int delay){            
      TIM2->CNT = 0;                  //Timer 2 counter set as 0
      //TIM2->CR1 |= (1<<0);          //start the timer
       while(TIM2->CNT < delay); 
}

void ms_delay(uint16_t ms_delay){     //Funtion gets the ms value from the user
  for(uint16_t i = 0; i< ms_delay; i++){ 
    us_delay(1000);                  //The delay is 1000ms
  }
}



void TIM5_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim5);
}

void timerIntr()                      //Timer interrupt initialzation 
{
 
  HAL_NVIC_SetPriority(TIM5_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
  
  HAL_TIM_Base_Start_IT(&htim5);

}


void stepperconfig(int delay)                //Counter funtion which on/off the led using interrupt 
{
  //uint32_t pinState=0; 
  while(1)
  {
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,1);             //PA5 is On
    ms_delay((int)delay);              //timer delay funtion is called
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,0);            //PA5 is Off
    ms_delay((int)delay);              //timer delay funtion is called
  }

}



ParserReturnVal_t Cmdstepperinit(int mode)
{
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
	steppermotorinit ();
  TimerInit();
  return CmdReturnOk;
}

ADD_CMD("stepperinit",Cmdstepperinit,"            Initialies the motor funcs")




ParserReturnVal_t Cmdstepperenable(int mode)
{
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,0); //OE on

  return CmdReturnOk;
}

ADD_CMD("stepperenable",Cmdstepperenable,"            Enables the motor")



ParserReturnVal_t Cmdstepperdisable(int mode)
{
  
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,1); //OE off

  return CmdReturnOk;
}

ADD_CMD("stepperdisable",Cmdstepperdisable,"            Enables the motor")



ParserReturnVal_t StepperStep(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  
  uint32_t rc=0;
  uint32_t rc1=0;
  uint32_t delay=0;
  uint32_t directionSteps=0;

  rc =  fetch_uint32_arg(&directionSteps);
  if(rc) {
   printf("Please enter the steps.\n");
   return CmdReturnBadParameter1;
  }

  int dir = (int)directionSteps;
  rc1 =  fetch_uint32_arg(&delay);
  if(rc1) {
   printf("Please enter the steps.\n");
   return CmdReturnBadParameter1;
  }
if(dir < 0){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,1);     //FR motor direction 0-Clockwise 1-AntiClockwise
  usersteps = abs((int)dir);
  printf("user steps = %d\n",usersteps);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,1);      //PS 1-operating 0-standby
 
}
else if(dir > 0){
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,0);    //FR motor direction 0-Clockwise 1-AntiClockwise
  usersteps = (int)dir;
  printf("user steps = %d\n",usersteps);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,1);      //PS 1-operating 0-standby

}else{
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,0);      //PS 1-operating 0-standby

}
  timerIntr();
  //htim5.Init.Prescaler = (int)delay-1;       //update the prescaler
  htim5.Init.Period = (1000*((int)delay)-1);            //max delay we get here is 1 miliseconds
  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Start_IT(&htim5);        //start timer with intrrupt
   
   
return CmdReturnOk;
}

ADD_CMD("step",StepperStep,"                Example Command")



ParserReturnVal_t Motorconfig(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  uint32_t arg1=0;
  uint32_t arg2=0;
  fetch_uint32_arg(&arg1);
  fetch_uint32_arg(&arg2);

  if ((arg1 == 0) && (arg2 == 0)){
    //Setting both pins high to get micro stepping 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,0); //MD1
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);//MD2

  }else if((arg1 == 1) && (arg2 == 0)){
    //Setting both pins high to get micro stepping 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,1); //MD1
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);//MD2
  }else if((arg1 == 0) && (arg2 == 1)){
    //Setting both pins high to get micro stepping 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,0); //MD1
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,1);//MD2
  }else if((arg1 == 1) && (arg2 == 1)){
    //Setting both pins high to get micro stepping 
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,1); //MD1
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,1);//MD2
  }
  return CmdReturnOk;
}

ADD_CMD("config",Motorconfig,"this command turns on the stepper motor.")



ParserReturnVal_t trapz(int mode)
{
//uint16_t delay = 100;
int i;
//uint32_t usersteps = 5000;
if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  int delay = 10;
for(i=0;i < 1600;++i)
{
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,1);
  HAL_Delay(delay);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,0);
  HAL_Delay(delay);


  // if(i == 1599)
  // {
  //     HAL_TIM_Base_Stop_IT(&htim5);        //start timer with intrrupt
  //     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,0);
  // }

  timerIntr();
  //htim5.Init.Prescaler = (int)delay-1;       //update the prescaler
  htim5.Init.Period = (100*(int)delay)-1;            //max delay we get here is 1 miliseconds
  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Start_IT(&htim5);        //start timer with intrrupt

  delay;
  printf("%d %d\n",delay,i);

  // if(i == 50)
  // {
  // HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,1);
  // HAL_Delay(10);
  // HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,0);
  // HAL_Delay(10);
  // timerIntr();
  // //htim5.Init.Prescaler = (int)delay-1;       //update the prescaler
  // htim5.Init.Period = (100*(int)delay)-1;            //max delay we get here is 1 miliseconds
  // HAL_TIM_Base_Init(&htim5);
  // HAL_TIM_Base_Start_IT(&htim5);        //start timer with intrrupt
  // }
  // else{
    
  // }


}
return CmdReturnOk;
}

ADD_CMD("Start",trapz,"                Example Command")

