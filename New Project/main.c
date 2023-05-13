#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "tm4c123gh6pm.h"
#include "semphr.h"
int Please_Dont_Optimize=0;
#define MaxDelay   portMAX_DELAY
/* Lock local variable */

int lock=0;

/* Queues */

xQueueHandle DriverMotorQueue;
xQueueHandle PassengerMotorQueue;

/* Semaphores */

xSemaphoreHandle JammingDriverSemaphore;
xSemaphoreHandle JammingPassengerSemaphore;

/* Define your Tasks Here */
void PortC_init()
{
	 SYSCTL_RCGCGPIO_R |=0x4;
	 while((SYSCTL_PRGPIO_R & 0X4)!=0X4);
	 GPIO_PORTC_LOCK_R = 0x4C4F434B;
	 GPIO_PORTC_CR_R |= 0XFF;
	 GPIO_PORTC_DEN_R |= 0XFF;
	 GPIO_PORTC_DIR_R |= 0X00;
	 GPIO_PORTC_PDR_R |=0xFF;
}



void PortD_init()
{
	 SYSCTL_RCGCGPIO_R |=0x08;
	 while((SYSCTL_PRGPIO_R & 0X08)!=0X08);
	 GPIO_PORTD_LOCK_R = 0x4C4F434B;
	 GPIO_PORTD_CR_R |= 0XFF;
	 GPIO_PORTD_DEN_R |= 0XFF;
	 GPIO_PORTD_DIR_R |= 0X00;
	 GPIO_PORTD_PDR_R |=0xFF;
}

void PortB_init()
{
SYSCTL_RCGCGPIO_R |=0x02;
 while((SYSCTL_PRGPIO_R & 0X2)!=0X2);
 GPIO_PORTB_LOCK_R = 0x4C4F434B;
 GPIO_PORTB_CR_R |= 0XFF;
 GPIO_PORTB_DIR_R |= 0X00;
 GPIO_PORTB_DEN_R |= 0XFF;
 GPIO_PORTB_PDR_R |=0xFF;
}

void PortE_init()
{
	 SYSCTL_RCGCGPIO_R |=0x10;
	 while((SYSCTL_PRGPIO_R & 0x10)!=0x10);
	 GPIO_PORTE_LOCK_R = 0x4C4F434B;
	 GPIO_PORTE_CR_R |= 0XFF;
	 GPIO_PORTE_DEN_R |= 0XFF;
	 GPIO_PORTE_DIR_R |= 0X18;
	 GPIO_PORTE_PDR_R |=0xFF;
}

void PortF_init()
{
	 SYSCTL_RCGCGPIO_R |=0x20;
	 while((SYSCTL_PRGPIO_R & 0x20)!=0x20);
	 GPIO_PORTF_LOCK_R = 0x4C4F434B;
	 GPIO_PORTF_CR_R |= 0XFF;
	 GPIO_PORTF_DEN_R |= 0XFF;
	 GPIO_PORTF_DIR_R |= 0XFF;
	 GPIO_PORTF_PDR_R |=0xFF;
}

void PortA_init()
{
	 SYSCTL_RCGCGPIO_R |=0x01;
	 while((SYSCTL_PRGPIO_R & 0X01)!=0X01);
	 //GPIO_PORTA_LOCK_R = 0x4C4F434B;
	 GPIO_PORTA_CR_R |= 0XFF;
	 GPIO_PORTA_DEN_R |= 0XFF;
	 GPIO_PORTA_DIR_R |= 0X1F;
}

void inits(){
  PortA_init();
	PortB_init();
	PortC_init();
	PortD_init();
	PortE_init();
	PortF_init();
	
}

void JammerDriverHandler(){
	xSemaphoreTake(JammingDriverSemaphore,0);
		while (1){
				xSemaphoreTake(JammingDriverSemaphore,MaxDelay);
				for(int i=0;i<99999;i++){
								if ((GPIO_PORTC_DATA_R&0x20)==0x20)//Limit switch is  pressed pin C5
						{	
						GPIO_PORTF_DATA_R = 0x04;	
						GPIO_PORTE_DATA_R&=0xFFFFFFE7; // Motor Stop
						GPIO_PORTF_DATA_R = 0x04;	
						}
						else{
						GPIO_PORTE_DATA_R|=0x08; // Motor Down		
						GPIO_PORTF_DATA_R = 0x02;
						//GPIO_PORTE_DATA_R&=0xFFFFFFE7; // Motor Stop
						GPIO_PORTF_DATA_R = 0x06;								
						}
					}
				//Motor Neutral
					GPIO_PORTE_DATA_R&=0xFFFFFFE7;
		}
}

void JammerPassengerHandler(){
	xSemaphoreTake(JammingPassengerSemaphore,0);
		while (1){
				xSemaphoreTake(JammingPassengerSemaphore,MaxDelay);
				for(int i=0;i<99999;i++){
				//Passenger Motor Down 
						if ((GPIO_PORTA_DATA_R&0x80)==0x80)//Limit switch is pressed
						{	
							GPIO_PORTF_DATA_R=0x04;
							GPIO_PORTA_DATA_R&=0xFFFFFFF3;// Stop motor
							GPIO_PORTF_DATA_R=0x02;
						}
						else{
							GPIO_PORTF_DATA_R=0x02;
							GPIO_PORTA_DATA_R|=0x04; // motor down
							GPIO_PORTF_DATA_R=0x04;
						}
					}
				//Motor Neutral
				GPIO_PORTA_DATA_R&=0xFFFFFFF3;
		}
}

void DriverMotor(){
	int command;
			while (1){
					if ((GPIO_PORTE_DATA_R&0x01)==0x01)//Driver Jammed
						xSemaphoreGive(JammingDriverSemaphore);
					xQueueReceive(DriverMotorQueue,&command,MaxDelay);
					switch(command){
						case 1:
						// Motor UP
						
					if ((GPIO_PORTA_DATA_R&0x20)==0x00) //if the limit switch is pressed
						{
							GPIO_PORTF_DATA_R = 0x08;
							GPIO_PORTE_DATA_R&=0xFFFFFFE7; // Motor Stop
							GPIO_PORTF_DATA_R = 0x08;
						}					
							
					else
						{
							for(int i=0;i<9999;i++){
							Please_Dont_Optimize++;	
							GPIO_PORTE_DATA_R|=0x10;
							} // Motor Up
							GPIO_PORTF_DATA_R = 0x02;
							//
							GPIO_PORTE_DATA_R&=0xFFFFFFE7; // Motor Stop
							GPIO_PORTF_DATA_R = 0x04;
						}
							break;
						case 2:
						//Motor Down
						if ((GPIO_PORTC_DATA_R&0x20)==0x20)//Limit switch is  pressed pin C5
						{	
						
						GPIO_PORTF_DATA_R = 0x04;	
						GPIO_PORTE_DATA_R&=0xFFFFFFE7; // Motor Stop
						GPIO_PORTF_DATA_R = 0x04;	
						}
						else{
						for(int i=0;i<9999;i++){
							Please_Dont_Optimize++;	
							GPIO_PORTE_DATA_R|=0x08; // Motor Down								
						}
						GPIO_PORTF_DATA_R = 0x02;
						GPIO_PORTE_DATA_R&=0xFFFFFFE7; // Motor Stop
						GPIO_PORTF_DATA_R = 0x06;								
						}
							break;
						default:
						//Motor Stop
						GPIO_PORTE_DATA_R&=0xFFFFFFE7;
						break;
				}						
			}
}

void PassengerMotor(){
	int command;
			while (1){
				if ((GPIO_PORTE_DATA_R&0x02)==0x2)//Passenger Jammed
					xSemaphoreGive(JammingPassengerSemaphore);
					xQueueReceive(PassengerMotorQueue,&command,MaxDelay);
					switch(command){
						case 1:
						// Motor UP
						if ((GPIO_PORTA_DATA_R&0x40)==0x40)//Limit switch is pressed
						{
							GPIO_PORTA_DATA_R&=0xFFFFFFF3;//motor stop
							GPIO_PORTF_DATA_R = 0x02;		
						}
						else {
							for(int i=0;i<9999;i++){
								Please_Dont_Optimize++;	
								GPIO_PORTA_DATA_R|=0x08; // Motor Up
							}						
							GPIO_PORTF_DATA_R = 0x04;		
							GPIO_PORTA_DATA_R&=0xFFFFFFF3; // Motor Stop
							GPIO_PORTF_DATA_R = 0x06;		
						}
							break;
						case 2:
						//Motor Down
						if ((GPIO_PORTA_DATA_R&0x80)==0x80)//Limit switch is pressed
						{	
							GPIO_PORTF_DATA_R=0x02;
							GPIO_PORTA_DATA_R&=0xFFFFFFF3;// Stop motor
							GPIO_PORTF_DATA_R=0x04;
						}
						else{
							for(int i=0;i<9999;i++){
								Please_Dont_Optimize++;	
								GPIO_PORTA_DATA_R|=0x04; // motor down
							}
							GPIO_PORTF_DATA_R=0x06;
							GPIO_PORTA_DATA_R&=0xFFFFFFF3;// Stop motor
							GPIO_PORTF_DATA_R=0x08;
						}
							break;
						default:
						//Motor Stop
						GPIO_PORTA_DATA_R&=0xFFFFFFF3;
						break;
				}						
			}
}

void Driver(){
	int up=1;
	int down=2;
			while (1){
				if ((GPIO_PORTE_DATA_R&0x01)==0x01)//Driver Jammed
					xSemaphoreGive(JammingDriverSemaphore);
				if ((GPIO_PORTE_DATA_R&0x04)==0x04) //Lock Button is up
					lock=1;
				else //Lock Button is down
					lock=0;
				if ((GPIO_PORTB_DATA_R&0x01)==0x01)//Driver Manual Up
					xQueueSendToBack(DriverMotorQueue,&up,0);
				if ((GPIO_PORTB_DATA_R&0x02)==0x2)//Driver Manual Down
					xQueueSendToBack(DriverMotorQueue,&down,0);
				if ((GPIO_PORTD_DATA_R&0x01)==0x01)//Driver auto Up
					xQueueSendToBack(DriverMotorQueue,&up,0);
				if ((GPIO_PORTD_DATA_R&0x02)==0x2)//Driver auto Down
					xQueueSendToBack(DriverMotorQueue,&down,0);
				if ((GPIO_PORTB_DATA_R&0x04)==0x4)//Pass Manual Up
					xQueueSendToBack(PassengerMotorQueue,&up,0);
				if ((GPIO_PORTB_DATA_R&0x08)==0x8)//Pass Manual Down
					xQueueSendToBack(PassengerMotorQueue,&down,0);
				if ((GPIO_PORTD_DATA_R&0x04)==0x4)//Pass auto Up
					xQueueSendToBack(PassengerMotorQueue,&up,0);
				if ((GPIO_PORTD_DATA_R&0x08)==0x8)//Pass auto Down
					xQueueSendToBack(PassengerMotorQueue,&down,0);	
				taskYIELD(); //Allow Passenger Task to take CPU
			} 
}

void Passenger(){
	int up=1;
	int down=2;
			while (1){
				if (lock ==0){	 //Check lock variable
				
				if ((GPIO_PORTE_DATA_R&0x02)==0x2)//Passenger Jammed
					xSemaphoreGive(JammingPassengerSemaphore);
				if ((GPIO_PORTB_DATA_R&0x10)==0x10)//Pass Manual Up
					xQueueSendToBack(PassengerMotorQueue,&up,0);
				if ((GPIO_PORTB_DATA_R&0x20)==0x20)//Pass Manual Down
					xQueueSendToBack(PassengerMotorQueue,&down,0);
				if ((GPIO_PORTD_DATA_R&0x40)==0x40)//Pass auto Up
					xQueueSendToBack(PassengerMotorQueue,&up,0);
				if ((GPIO_PORTD_DATA_R&0x80)==0x80)//Pass auto Down
					xQueueSendToBack(PassengerMotorQueue,&down,0);
				}	
				taskYIELD(); //Allow Driver Task to take CPU
			}
}

int main(){
	
	/*initialization*/
	
	inits();
	
	/*Queues creation */
	
	DriverMotorQueue=xQueueCreate(10,sizeof(int));
	PassengerMotorQueue=xQueueCreate(10,sizeof(int));
	
	/*Semaphores creation */
	
	vSemaphoreCreateBinary(JammingDriverSemaphore);
	vSemaphoreCreateBinary(JammingPassengerSemaphore);
 
	/* Create you Tasks Here using xTaskCreate() */
	
	xTaskCreate(JammerDriverHandler,"JammerDriverHandler",240,NULL,4,NULL);
	xTaskCreate(JammerPassengerHandler,"JammerPassengerHandler",240,NULL,4,NULL);
	xTaskCreate(DriverMotor,"DriverMotor",240,NULL,3,NULL);
	xTaskCreate(PassengerMotor,"PassengerMotor",240,NULL,3,NULL);
  xTaskCreate(Driver,"Driver",240,NULL,2,NULL);
  xTaskCreate(Passenger,"Passenger",240,NULL,2,NULL);

  // Startup of the FreeRTOS scheduler. The program should block here. 
  
	vTaskStartScheduler();
  
	// The following line should never be reached. 
  //Failure to allocate enough memory from the heap could be a reason.
  
	for (;;);
}