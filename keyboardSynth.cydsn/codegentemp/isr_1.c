/*******************************************************************************
* File Name: isr_1.c  
* Version 1.70
*
*  Description:
*   API for controlling the state of an interrupt.
*
*
*  Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include <cydevice_trm.h>
#include <CyLib.h>
#include <isr_1.h>
#include "cyapicallbacks.h"

#if !defined(isr_1__REMOVED) /* Check for removal by optimization */

/*******************************************************************************
*  Place your includes, defines and code here 
********************************************************************************/
/* `#START isr_1_intc` */
#include <project.h>
#include <math.h>


    uint32 volatile int_n = 0;         //initialize count to 0
    double volatile freqtable[8];      //declare array to store set frequencies
    double volatile sinetable[256];     //declare array to store sine values
    
double fn(double t, int tone)          //function responsible for square or sine wave generation.
{
    int di;                            
    int y =(int) t;                    //takes integer part of ratio of sampling frequency to period, given a note's frequency
    di = (int) ((t-y)*256);            //difference between above mentioned ratio and it's integer part, scaled by 256. This value's integer part
    
    if (tone==0){                      //option 1 of tone, generates square wave, sounds something like an electric guitar
        if(0<(t-y)&&(t-y)<0.5)         
            return 1;
        else 
            return 0;
    }
    else{                              //option 1 of tone, generates sine wave, sounds something like a flute
       return sinetable[di]; 
    }
    return 0;
}
    
void synthInit(){                       //intializes synthesizer, creates tables of frequencys and sine values
    int i;
    freqtable[0]=440;                   //initialize frequency of lowest note to 440Hz, or A4
    for(i=0;i<7;){
        freqtable[i+1]=(pow(2.0,(1.0/6)))*freqtable[i];     //generates next note 1 whole step above previous
        if(i==2 || i==6){
            freqtable[i+1]=(pow(2.0,(1.0/12)))*freqtable[i]; //generates next note 1 half step above previous
        }
        i++;
    } 
    for(i=0;i<8;){
        freqtable[i] = 1/freqtable[i];
        i++;
    }

    for(i=0;i<256;i++){                //generates table of sine values
        sinetable[i] = (0.5) + (0.5)*sin(2*(M_PI)*i/256); 
    }
    
}

 
/* `#END` */

#ifndef CYINT_IRQ_BASE
#define CYINT_IRQ_BASE      16
#endif /* CYINT_IRQ_BASE */
#ifndef CYINT_VECT_TABLE
#define CYINT_VECT_TABLE    ((cyisraddress **) CYREG_NVIC_VECT_OFFSET)
#endif /* CYINT_VECT_TABLE */

/* Declared in startup, used to set unused interrupts to. */
CY_ISR_PROTO(IntDefaultHandler);


/*******************************************************************************
* Function Name: isr_1_Start
********************************************************************************
*
* Summary:
*  Set up the interrupt and enable it. This function disables the interrupt, 
*  sets the default interrupt vector, sets the priority from the value in the
*  Design Wide Resources Interrupt Editor, then enables the interrupt to the 
*  interrupt controller.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_Start(void)
{
    /* For all we know the interrupt is active. */
    isr_1_Disable();

    /* Set the ISR to point to the isr_1 Interrupt. */
    isr_1_SetVector(&isr_1_Interrupt);

    /* Set the priority. */
    isr_1_SetPriority((uint8)isr_1_INTC_PRIOR_NUMBER);

    /* Enable it. */
    isr_1_Enable();
}


/*******************************************************************************
* Function Name: isr_1_StartEx
********************************************************************************
*
* Summary:
*  Sets up the interrupt and enables it. This function disables the interrupt,
*  sets the interrupt vector based on the address passed in, sets the priority 
*  from the value in the Design Wide Resources Interrupt Editor, then enables 
*  the interrupt to the interrupt controller.
*  
*  When defining ISR functions, the CY_ISR and CY_ISR_PROTO macros should be 
*  used to provide consistent definition across compilers:
*  
*  Function definition example:
*   CY_ISR(MyISR)
*   {
*   }
*   Function prototype example:
*   CY_ISR_PROTO(MyISR);
*
* Parameters:  
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_StartEx(cyisraddress address)
{
    /* For all we know the interrupt is active. */
    isr_1_Disable();

    /* Set the ISR to point to the isr_1 Interrupt. */
    isr_1_SetVector(address);

    /* Set the priority. */
    isr_1_SetPriority((uint8)isr_1_INTC_PRIOR_NUMBER);

    /* Enable it. */
    isr_1_Enable();
}


/*******************************************************************************
* Function Name: isr_1_Stop
********************************************************************************
*
* Summary:
*   Disables and removes the interrupt.
*
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_Stop(void)
{
    /* Disable this interrupt. */
    isr_1_Disable();

    /* Set the ISR to point to the passive one. */
    isr_1_SetVector(&IntDefaultHandler);
}


/*******************************************************************************
* Function Name: isr_1_Interrupt
********************************************************************************
*
* Summary:
*   The default Interrupt Service Routine for isr_1.
*
*   Add custom code between the coments to keep the next version of this file
*   from over writting your code.
*
* Parameters:  
*
* Return:
*   None
*
*******************************************************************************/
CY_ISR(isr_1_Interrupt)
{
    #ifdef isr_1_INTERRUPT_INTERRUPT_CALLBACK
        isr_1_Interrupt_InterruptCallback();
    #endif /* isr_1_INTERRUPT_INTERRUPT_CALLBACK */ 

    /*  Place your Interrupt code here. */
    /* `#START isr_1_Interrupt` */
    
   
    uint8_t fOut;
    extern int TONE;
   
    
    //double Ta[8];                                   //declares table of periods
    double T=1.0/12000;                             //sets sampling period T
    
    
    
    
    fOut=0;                                         //initialize output to DAC to be 0
    
    if(Tone_Switch_Read()==0){                      //Changes option of TONE given button push
        TONE=1-TONE;
        CyDelay(100);
    }
    
    if(Key_1_Read()== 0)                            //checks for button 1 press
    {
        //Ta[0]=(1.0/freqtable[0]);  //A4             sets period given frequency of first note 
        fOut += 64*fn(int_n*T/freqtable[0], TONE);         //updates output to VDAC
    }
    if(Key_2_Read()==0){                            //checks for button 2 press 
        //Ta[1]=(1/freqtable[1]);  //A#4/Bb4          sets period given frequency of second note
        fOut += 64*fn(int_n*T/freqtable[1], TONE);         //updates output to VDAC
    }
    if(Key_3_Read()==0){                            //checks for button 3 press
        //Ta[2]=(1/freqtable[2]);  //B4               sets period given frequency of third note 
        fOut += 64*fn(int_n*T/freqtable[2], TONE);         //updates output to VDAC
    }
    if(Key_4_Read()==0){                            //checks for button 4 press
        //Ta[3]=(1/freqtable[3]);  //C5               sets period given frequency of fourth note
        fOut += 64*fn(int_n*T/freqtable[3], TONE);         //updates output to VDAC
    }
    if(Key_5_Read()==0){         //C#5/Db5          checks for button 5 press    
        //Ta[4]=(1/freqtable[4]);                     //sets period given frequency of fifth note
        fOut += 64*fn(int_n*T/freqtable[4], TONE);         //updates output to VDAC
    }
    if(Key_6_Read()==0){         //D5               checks for button 6 press
        //Ta[5]=(1/freqtable[5]);                     //sets period given frequency for sixth note
        fOut += 64*fn(int_n*T/freqtable[5], TONE);         //updates output to VDAC
    }
    if(Key_7_Read()==0){         //D#5/Eb5          checks for button 7 press
        //Ta[6]=(1/freqtable[6]);                     //sets period given frequency for seventh note
        fOut += 64*fn(int_n*T/freqtable[6], TONE);         //updates output to VDAC
    }
    if(Key_8_Read()==0){         //E5               checks for button 8 press
        //Ta[7]=(1/freqtable[7]);                     //sets period given frequency for eigth note
        fOut += 64*fn(int_n*T/freqtable[7], TONE);         //updates output to VDAC
    }

    static int temp=0;
    temp=1-temp;
    temp_Write(temp);
    VDAC8_1_SetValue(fOut); //sets sum of frequencies out to VDAC
    ++int_n;                                        //increments n
    /* `#END` */
}


/*******************************************************************************
* Function Name: isr_1_SetVector
********************************************************************************
*
* Summary:
*   Change the ISR vector for the Interrupt. Note calling isr_1_Start
*   will override any effect this method would have had. To set the vector 
*   before the component has been started use isr_1_StartEx instead.
* 
*   When defining ISR functions, the CY_ISR and CY_ISR_PROTO macros should be 
*   used to provide consistent definition across compilers:
*
*   Function definition example:
*   CY_ISR(MyISR)
*   {
*   }
*
*   Function prototype example:
*     CY_ISR_PROTO(MyISR);
*
* Parameters:
*   address: Address of the ISR to set in the interrupt vector table.
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_SetVector(cyisraddress address)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    ramVectorTable[CYINT_IRQ_BASE + (uint32)isr_1__INTC_NUMBER] = address;
}


/*******************************************************************************
* Function Name: isr_1_GetVector
********************************************************************************
*
* Summary:
*   Gets the "address" of the current ISR vector for the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Address of the ISR in the interrupt vector table.
*
*******************************************************************************/
cyisraddress isr_1_GetVector(void)
{
    cyisraddress * ramVectorTable;

    ramVectorTable = (cyisraddress *) *CYINT_VECT_TABLE;

    return ramVectorTable[CYINT_IRQ_BASE + (uint32)isr_1__INTC_NUMBER];
}


/*******************************************************************************
* Function Name: isr_1_SetPriority
********************************************************************************
*
* Summary:
*   Sets the Priority of the Interrupt. 
*
*   Note calling isr_1_Start or isr_1_StartEx will 
*   override any effect this API would have had. This API should only be called
*   after isr_1_Start or isr_1_StartEx has been called. 
*   To set the initial priority for the component, use the Design-Wide Resources
*   Interrupt Editor.
*
*   Note This API has no effect on Non-maskable interrupt NMI).
*
* Parameters:
*   priority: Priority of the interrupt, 0 being the highest priority
*             PSoC 3 and PSoC 5LP: Priority is from 0 to 7.
*             PSoC 4: Priority is from 0 to 3.
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_SetPriority(uint8 priority)
{
    *isr_1_INTC_PRIOR = priority << 5;
}


/*******************************************************************************
* Function Name: isr_1_GetPriority
********************************************************************************
*
* Summary:
*   Gets the Priority of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   Priority of the interrupt, 0 being the highest priority
*    PSoC 3 and PSoC 5LP: Priority is from 0 to 7.
*    PSoC 4: Priority is from 0 to 3.
*
*******************************************************************************/
uint8 isr_1_GetPriority(void)
{
    uint8 priority;


    priority = *isr_1_INTC_PRIOR >> 5;

    return priority;
}


/*******************************************************************************
* Function Name: isr_1_Enable
********************************************************************************
*
* Summary:
*   Enables the interrupt to the interrupt controller. Do not call this function
*   unless ISR_Start() has been called or the functionality of the ISR_Start() 
*   function, which sets the vector and the priority, has been called.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_Enable(void)
{
    /* Enable the general interrupt. */
    *isr_1_INTC_SET_EN = isr_1__INTC_MASK;
}


/*******************************************************************************
* Function Name: isr_1_GetState
********************************************************************************
*
* Summary:
*   Gets the state (enabled, disabled) of the Interrupt.
*
* Parameters:
*   None
*
* Return:
*   1 if enabled, 0 if disabled.
*
*******************************************************************************/
uint8 isr_1_GetState(void)
{
    /* Get the state of the general interrupt. */
    return ((*isr_1_INTC_SET_EN & (uint32)isr_1__INTC_MASK) != 0u) ? 1u:0u;
}


/*******************************************************************************
* Function Name: isr_1_Disable
********************************************************************************
*
* Summary:
*   Disables the Interrupt in the interrupt controller.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_Disable(void)
{
    /* Disable the general interrupt. */
    *isr_1_INTC_CLR_EN = isr_1__INTC_MASK;
}


/*******************************************************************************
* Function Name: isr_1_SetPending
********************************************************************************
*
* Summary:
*   Causes the Interrupt to enter the pending state, a software method of
*   generating the interrupt.
*
* Parameters:
*   None
*
* Return:
*   None
*
* Side Effects:
*   If interrupts are enabled and the interrupt is set up properly, the ISR is
*   entered (depending on the priority of this interrupt and other pending 
*   interrupts).
*
*******************************************************************************/
void isr_1_SetPending(void)
{
    *isr_1_INTC_SET_PD = isr_1__INTC_MASK;
}


/*******************************************************************************
* Function Name: isr_1_ClearPending
********************************************************************************
*
* Summary:
*   Clears a pending interrupt in the interrupt controller.
*
*   Note Some interrupt sources are clear-on-read and require the block 
*   interrupt/status register to be read/cleared with the appropriate block API 
*   (GPIO, UART, and so on). Otherwise the ISR will continue to remain in 
*   pending state even though the interrupt itself is cleared using this API.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void isr_1_ClearPending(void)
{
    *isr_1_INTC_CLR_PD = isr_1__INTC_MASK;
}

#endif /* End check for removal by optimization */


/* [] END OF FILE */
