/* example.c:
 *
 *   Template code for implementing a simple task, using the ADD_TASK()
 *   macro.  Also contains template code for a simple monitor command.
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
  int count2 = 0;
  int count3 = 0;
  int count2A = 0;
  int count3A = 0;  
int myindex[1000];
int flag = 0; 

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
    htim5.Init.Prescaler = 500 - 1; // 50 000 000/50 = 1,000,000
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 1000 - 1; // max delay we get here is 1 miliseconds
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim5);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1 | GPIO_PIN_8;
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
    htim2.Init.Prescaler = 5000 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000 - 1;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim2);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 5000 - 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
    // HAL_TIM_MspPostInit(&htim2);
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

    //pulse = pulse*10;
    if (htim == &htim5)
    {
      
        if (flag == 1){
            TIM2->CCR1 = (myindex[count]/2);
            count++;
        if(count == 630){
            count = 0;
        }
        }else{
        TIM2->CCR1 = pulse;  
        }

        if (flag == 1){
            TIM2->CCR2 = (myindex[count2+210]/2);
            count2A++;
        if(count2A == 630){
            count2A = 0;
        }
        }else{
        TIM2->CCR2 = pulse;  
        }

        if (flag == 1){
            TIM2->CCR2 = (myindex[count3+410]/2);
            count3A++;
        if(count3A == 630){
            count3A = 0;
        }
        }else{
        TIM2->CCR3 = pulse;  
        }


        //TIM2->CCR2 = pulse;

        //TIM2->CCR3 = pulse;
        
    }
       // }
}

// FUNCTION      : pwmSelection
// DESCRIPTION   : PWM channel are selected
// RETURNS       : Nothing
void pwmSelection(int channelselect)
{
    flag=0;
    count = 0;
    switch(channelselect)
    {
    case 1:
    {
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 


        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);//Channel 2 & 3 are stopped
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
        printf("Channel 1 is selected\n");
        break;
    }
    case 2:
    {
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);//Channel 2 is intialized 
        
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);//Channel 1 & 3 are stopped
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
        printf("Channel 2 is selected\n");
        break;
    }
    case 3:
    {
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);//Channel 3 is intialized 

        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);//Channel 1 & 2 are stopped
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
        printf("Channel 3 is selected\n");
        break;
    }
    default:
    {
        printf("Select channel from 1 to 3\n");
    }
    }
}




// FUNCTION      : pwmOff
// DESCRIPTION   : PWM channel are turned off
// RETURNS       : Nothing
void pwmOff(int channelselect)
{

    switch(channelselect)
    {
    case 1:
    {
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);//Channel 1 is turned off
        printf("Channel 1 is turned off\n");
        break;
    }
    case 2:
    {
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);//Channel 2 is turned off
        printf("Channel 2 is turned off\n");
        break;
    }
    case 3:
    {
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);//Channel 3 is turned off
        printf("Channel 3 is turned off\n");
        break;
    }
    default:
    {
        printf("Select channel from 1 to 3 to turn off\n");
    }
    }
}


// FUNCTION      : pwmOn
// DESCRIPTION   : PWM channel are turned on
// RETURNS       : Nothing
void pwmOn(int channelselect)
{

    switch(channelselect)
    {
    case 1:
    {
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is turned on
        printf("Channel 1 is turned on\n");
        break;
    }
    case 2:
    {
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);//Channel 2 is turned on
        printf("Channel 2 is turned on\n");
        break;
    }
    case 3:
    {
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);//Channel 3 is turned on
        printf("Channel 3 is turned on\n");
        break;
    }
    default:
    {
        printf("Select channel from 1 to 3 to turn on\n");
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
ParserReturnVal_t pwmInit(int mode)
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
ADD_CMD("pwminit", pwmInit, "This function initialise the pwmconfiguration")



// FUNCTION      : pwmEnable
// DESCRIPTION   : PWM channel are enabled
// RETURNS       : Nothing
ParserReturnVal_t pwmEnable(int mode)
{
    if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;

    uint32_t channelselect = 0;
    fetch_uint32_arg(&channelselect);
    pwmOn(channelselect);

  return CmdReturnOk;
}

ADD_CMD("pwmOn",pwmEnable,"          pwm on")



// FUNCTION      : pwmDisable
// DESCRIPTION   : PWM channel are disabled
// RETURNS       : Nothing
ParserReturnVal_t pwmDisable(int mode)
{
    if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;

    uint32_t channelselect = 0;
    fetch_uint32_arg(&channelselect);
    pwmOff(channelselect);

  return CmdReturnOk;
}

ADD_CMD("pwmoff",pwmDisable,"          pwm off")


// FUNCTION      : pwmConfig
// DESCRIPTION   : User enters 2 values from the command line. Those are Channel selection and duty
// RETURNS       : Nothing
ParserReturnVal_t pwmConfig(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

    uint32_t channelselect=0;
    uint32_t duty=0;
    fetch_uint32_arg(&channelselect);
    fetch_uint32_arg(&duty);

    pwmSelection(channelselect); //The Channel selection funtion
    
    dutySelection(duty);         //The duty selection funtion


  timerIntr();
  HAL_TIM_Base_Init(&htim5);
  HAL_TIM_Base_Start_IT(&htim5); 
  return CmdReturnOk;
}

ADD_CMD("pwm",pwmConfig,"                Example Command")

// FUNCTION      : sinwave
// DESCRIPTION   : it generates sinwvae data points.
// RETURNS       : Nothing
ParserReturnVal_t sinwave(int mode)
{
    int Samplecounter  = 0;
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;

    for(float i =0; i <= 10 ; i += 0.01){
        
        myindex[count]= (int)(sin(i)*1000+1000);
        count++;
        Samplecounter++;
    }
    count = 0;

    flag = 1;
    timerinit();
    timerIntr();
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);//Channel 2 & 3 are stopped
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
    //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);//Channel 1 is intialized 
    //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);//Channel 1 is intialized 
    return CmdReturnOk;
}

ADD_CMD("sinwave", sinwave, "                Example Command")

// FUNCTION      : rgb
// DESCRIPTION   : RGB led glowing
// RETURNS       : Nothing
ParserReturnVal_t rgb(int mode)
{
     count  = 0;
  count2 = 0;
  count3 = 0;
  count2A = 0;
  count3A = 0;  
  
    int Samplecounter  = 0;
    if (mode != CMD_INTERACTIVE)
        return CmdReturnOk;

    for(float i =0; i <= 10 ; i += 0.01){
        
        myindex[count]= (int)(sin(i)*1000+1000);
        count++;
        Samplecounter++;
    }
    count = 0;

    flag = 1;
    timerinit();
    timerIntr();
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);//Channel 2 & 3 are stopped
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//Channel 1 is intialized 
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);//Channel 1 is intialized 
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);//Channel 1 is intialized 
    return CmdReturnOk;
}

ADD_CMD("rgb", rgb, "                Example Command")


// FUNCTION      : Stop
// DESCRIPTION   : stop all the pm channels
// RETURNS       : Nothing
ParserReturnVal_t stop(int mode)
{

    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);//Channel 2 & 3 are stopped
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    return CmdReturnOk;
}

ADD_CMD("stop", stop, "                Example Command")



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