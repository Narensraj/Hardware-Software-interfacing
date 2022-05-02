/* example.c:
*
*   Template code for implementing a simple task, using the ADD_TASK()
*   macro.  Also contains template code for a simple monitor command.
*
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "common.h"
#include "main.h"

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim1;
int stepCounter=0;
int usersteps=0;
uint32_t pinState=0;






void ExampleInit(void *data)
{

/* Place Initialization things here.  This function gets called once
 * at startup.
 */

}

void ExampleTask(void *data)
{

/* Place your task functionality in this function.  This function
 * will be called repeatedly, as if you placed it inside the main
 * while(1){} loop.
 */

}

ADD_TASK(ExampleTask,  /* This is the name of the function for the task */
 ExampleInit,  /* This is the initialization function */
 NULL,         /* This pointer is passed as 'data' to the functions */
 0,            /* This is the number of milliseconds between calls */
"This is the help text for the task")




void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
if(htim == &htim5){
  
 
  GPIOB->ODR ^= (1<<4);
  //GPIOB->ODR |= (1<<4);
  stepCounter++; 
  
  

    if((stepCounter) == (2*usersteps)){
      stepCounter=0;
      
      HAL_TIM_Base_Stop_IT(&htim5);        //start timer with intrrupt
      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,0);
    }
   
  
    //printf("step counted are %d\n",stepCounter);
}

}


void TIM5_IRQHandler(void)
{
HAL_TIM_IRQHandler(&htim5);
}

void timerIntr()
{

HAL_NVIC_SetPriority(TIM5_IRQn, 0, 1);//Set inturrupt priority 
HAL_NVIC_EnableIRQ(TIM5_IRQn);

HAL_TIM_Base_Start_IT(&htim5);        //start timer with intrrupt

}



void stepperINIT()
{

HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);      //OE 0-Enable 1-Disable

HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,0);      //PS 1-operating 0-standby

HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,1);     //RST 1-notreset 0-reset


HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);      //Steps 0,1 we need to delay according to user 

HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);     //FR motor direction 0-Clockwise 1-AntiClockwise

//Setting both pins high to get micro stepping 
HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,1);      //MD1
HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,1);      //MD2

//Setting both pins low to get maximum current 
HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,0);      //ATT1
HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,0);      //ATT2

HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,1);      //VREF Connected to 3.3v


}


ParserReturnVal_t Motorconfig(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  uint32_t arg1=0;
  uint32_t arg2=0;
  fetch_uint32_arg(&arg1);
  fetch_uint32_arg(&arg2);

  if ((arg1 == 0) && (arg2 == 0)){
    //Setting both pins high to get micro stepping 
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,0);      //MD1
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,0);      //MD2

  }else if((arg1 == 1) && (arg2 == 0)){
    //Setting both pins high to get micro stepping 
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,1);      //MD1
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,0);      //MD2
  }else if((arg1 == 0) && (arg2 == 1)){
    //Setting both pins high to get micro stepping 
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,0);      //MD1
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,1);      //MD2
  }else if((arg1 == 1) && (arg2 == 1)){
    //Setting both pins high to get micro stepping 
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,1);      //MD1
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,1);      //MD2
  }
  return CmdReturnOk;
}

ADD_CMD("config",Motorconfig,"this command turns on the stepper motor.")



ParserReturnVal_t stepperEnable(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,1);      //PS 1-operating 0-standby

  return CmdReturnOk;
}

ADD_CMD("stepperenable",stepperEnable,"this command turns on the stepper motor.")



ParserReturnVal_t rotate(int mode)
{
  uint32_t step;
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  fetch_uint32_arg(&step);
  //int acc=0;
  for(int i = 0;i<=(int)step;i++){
    GPIOB->ODR &= ~(1<<4);
    HAL_Delay(1);
    GPIOB->ODR |= (1<<4);


  }
  
  return CmdReturnOk;
}

ADD_CMD("rotate",rotate,"this command turns on the stepper motor.")




ParserReturnVal_t stepperDisable(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,0);      //PS 1-operating 0-standby

  return CmdReturnOk;
}

ADD_CMD("stepperdisable",stepperDisable,"this command turns on the stepper motor.")


void timerinit(){
RCC->APB1ENR |= (1<<3);        // Enable clock source for timer 5

htim5.Instance = TIM5;
htim5.Init.Prescaler = 50-1;            //50 000 000/50 = 1,000,000
htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
htim5.Init.Period = 1000-1;            //max delay we get here is 1 miliseconds
htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
HAL_TIM_Base_Init(&htim5);

}


void gpioAinit(){
/* Configure GPIO pins */
GPIOA->MODER &= ~(1<<12);       //make PA6 pin as input 
GPIOA->MODER &= ~(1<<13);       //make PA6 pin as input
GPIOA->IDR &= ~(1<<6);          //reset PC6 pin
GPIO_InitTypeDef  GPIO_InitStruct;
GPIO_InitStruct.Pin = ( GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_9);
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void gpioBinit(){
/* Configure GPIO pins */
GPIO_InitTypeDef  GPIO_InitStruct;
GPIO_InitStruct.Pin = (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_10);
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

ParserReturnVal_t stepperinit(int mode)
{
if(mode != CMD_INTERACTIVE) return CmdReturnOk;
//GPIO Initialisation
__GPIOC_CLK_ENABLE();

GPIOA->MODER |= (1<<10);       //make PA5 pin as output 

/* Configure GPIO pins */
GPIO_InitTypeDef  GPIO_InitStruct;
GPIO_InitStruct.Pin = GPIO_PIN_7;
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);


gpioAinit();

gpioBinit();

stepperINIT();  //Initialize steeper peripherals
timerinit();    //initialize timer intrrupts

return CmdReturnOk;
}

ADD_CMD("stepperinit",stepperinit,"this command initialises gpio portc and timer to control stepper motor.")



ParserReturnVal_t delay(int mode)
{
if(mode != CMD_INTERACTIVE) return CmdReturnOk;
uint32_t rc=0;
uint32_t delay=0;
rc =  fetch_uint32_arg(&delay);
  if(rc) {
   printf("Please enter the delay\n");
   return CmdReturnBadParameter1;
 }
htim5.Init.Prescaler = delay-1;       //update the prescaler
HAL_TIM_Base_Init(&htim5);
HAL_TIM_Base_Start_IT(&htim5);        //start timer with intrrupt

return CmdReturnOk;
}

ADD_CMD("delay",delay,"this command generates interrupt for given delay")




ParserReturnVal_t step(int mode)
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
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,1);     //FR motor direction 0-Clockwise 1-AntiClockwise
  usersteps = abs((int)dir);
  printf("user steps = %d\n",usersteps);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,1);      //PS 1-operating 0-standby
 
}
else if(dir > 0){
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);     //FR motor direction 0-Clockwise 1-AntiClockwise
  usersteps = (int)dir;
  printf("user steps = %d\n",usersteps);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,1);      //PS 1-operating 0-standby

}else{
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,0);      //PS 1-operating 0-standby

}
  timerIntr();
  //htim5.Init.Prescaler = (int)delay-1;       //update the prescaler
  htim5.Init.Period = (1000*((int)delay)-1);            //max delay we get here is 1 miliseconds
  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Start_IT(&htim5);        //start timer with intrrupt
  
  
  // for(int i = 0;i<=(int)usersteps;i++){
  //   GPIOB->ODR &= ~(1<<4);
  //   HAL_Delay((int)delay);
  //   GPIOB->ODR |= (1<<4);


  // }

return CmdReturnOk;
}

ADD_CMD("step",step,"this command takes steps and delay as input.")






ParserReturnVal_t CmdExample(int mode)
{
if(mode != CMD_INTERACTIVE) return CmdReturnOk;

/* Put your command implementation here */
printf("Example Command\n");

return CmdReturnOk;
}

ADD_CMD("example",CmdExample,"                Example Command")
