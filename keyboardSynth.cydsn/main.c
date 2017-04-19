/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <stdio.h>

extern double volatile freqtable[8];
extern float volatile sinetable[1024];

void synthInit( void );
int TONE=0;

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    VDAC8_1_Start();    //starts VDAC
    isr_1_Start();      //starts interrupt 1
    synthInit();        //intializes synth
    

    while (1)           //loop forever, and delay
    {
        /* Place your application code here. */
        CyDelay(100);
    }
}

/* [] END OF FILE */
