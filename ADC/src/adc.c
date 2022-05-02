//Author:Naren subburaj

//Date 30-01-2022
//
//this file uses command AdcInit to initialize adc channel 1 and 0
//and commanf adc is use to start the adc function.
//press on board switch B1 to stop the convertion.
//command EXAMPLE adcInit
//                adc 1 | 0 "1 | 0 to select the channel."


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx_it.h"
#include "common.h"
#include <string.h>

int ADCdata;

void ADC_conversion(void)
{
  ADC1->CR2 |= (1<<30); // Start  ADC conversion
  
}
int ADC_read(void)
{
  while(!(ADC1->SR & (1<<1))) {} // wait for conversion to be complete
    return (ADC1-> DR);
    
}


ADC_ChannelConfTypeDef sConfig = {0};
 ADC_HandleTypeDef hadc1;
uint16_t raw;
 char myMSG[]="Potentiometer value is ";
  char buffer[50];


void adcChannel(int channel){


  ADC1->SQR3 = 0;
  ADC1->SQR3 |=(channel << 0);//conversation in regular channel
  ADC1->SR |= 0; //clearing thr staus register
  ADC1->CR2 |= (1<<30); // Start  ADC conversion


}



ParserReturnVal_t gpioInit(int mode)
{
  if(mode != CMD_INTERACTIVE) 
  RCC->AHB2ENR |= 0x00000003;
    // initialized the struct to set gpio port a parameters.
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  return CmdReturnOk;
}

ADD_CMD("gpioInit",gpioInit,"0 | 1           Control LED")



ParserReturnVal_t AdcInit(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;


  RCC->AHB1ENR |= (1<<0); // Enable clock source for PORT A
  GPIOA->MODER |= (3<<0); // Pa0 pin as an Analog
  RCC->APB2ENR |= (1<<8);  // Enable clock source for ADC1
  ADC1->CR1 &= ~(1<<25);
  ADC1->CR1 &= ~(1<<24);// set the resolution 12 bit of adc 
  ADC1->CR2 &= ~(1<<0);  //  ADC disable
  
  GPIOA->MODER |= (3<<0);//pin PA0 as analog
  GPIOA->MODER |= (3<<2);//pin PA1 as analog
  GPIOC->MODER |= 0x00000000; //make gpio port c as input
  GPIOB->MODER |= 0x00005555; //make gpio portb as output
  GPIOC->IDR &= ~(1<<13);//reset PB13 pin
  ADC1->CR2 |=  (1<<0);  // ADC ON
  printf("Press on board Switch B1 to stop the converstion.\n");
  return CmdReturnOk;
}

ADD_CMD("adcInit",AdcInit,"0 | 1           Control LED")




ParserReturnVal_t Adc(int mode)
{
  uint32_t Channel_number;
  uint32_t  rc=0;
  uint32_t pinState=0;
  float voltage;

  //char msg[10];
  

  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  rc =  fetch_uint32_arg(&Channel_number);
  if(rc) {
     printf("Please supply the ADC channel 1 or 0.\n");
    return CmdReturnBadParameter1;
  }
  printf("selected channel is %d\n",(int)Channel_number);
  if(Channel_number < 0 || Channel_number > 1 ){
    printf("Please supply the ADC channel 1 or 0.\n");
  }else{
    (Channel_number == 0) ? (adcChannel(0)) : (adcChannel(1)); //select which channel we want to use.
    while(1){
    pinState = ~GPIOC->IDR;
    
    if(pinState & (1<<13)){
      break;
    }
    IWDG->KR |= 0x0000AAAA; // reload this value otherwise watchdog will reset
    ADC_conversion();
    ADCdata = ADC_read();
    voltage = ((float)ADCdata/4096)*3.3;
    
    //printf("ADC raw value is %d and measured voltage value is %.2f\n",ADCdata,voltage); 
    printf("%d %.2f\n",ADCdata,voltage); 
    
    
    }
  
  }
  
  

  return CmdReturnOk;
}

ADD_CMD("adc",Adc,"0 | 1           Control LED")

ParserReturnVal_t gpio(int mode)
{
  uint32_t OnOff,Pin_Num;
  uint32_t  rc=0;
  uint32_t rc1=0;
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
  
  rc =  fetch_uint32_arg(&Pin_Num);
  rc1 = fetch_uint32_arg(&OnOff);  

  if(rc) {
     printf("Please supply the pin number between 0 to 15.\n");
    return CmdReturnBadParameter1;
  }
  printf("pinnumber is %lu\n",Pin_Num);
  
  
  if(rc1) {
     printf("Please supply 1 | 0 to turn on or off the LED\n");
    return CmdReturnBadParameter1;
  }
  
  printf("pinstate is %lu\n",OnOff);
  (OnOff == 1 ) ? (GPIOA->ODR |= (1<<Pin_Num)) : (GPIOA->ODR ^= (1<<Pin_Num));//sets or resets the pin on GPIO port A.
  return CmdReturnOk;
}

ADD_CMD("gpio",gpio,"0 | 1           Control LED")

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
  

ParserReturnVal_t CmdExample(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  /* Put your command implementation here */
  printf("Example Command\n");
  
  return CmdReturnOk;
}

ADD_CMD("example",CmdExample,"                Example Command")
