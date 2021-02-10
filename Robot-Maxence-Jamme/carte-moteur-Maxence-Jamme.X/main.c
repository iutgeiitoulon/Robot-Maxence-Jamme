#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"


int main ( void) {
    InitOscillator();
    InitIO();
    InitTimer23();
    InitTimer1();
    InitPWM();
    //PWMSetSpeed(-15, MOTEUR_DROIT); // 0 = Droite | 1 = Gauche |pourcentage detat bas de PWM 
    //PWMSetSpeed(-15, MOTEUR_GAUCHE);  //  + = avance | - = recule | (batterie face à nous)
    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;
    while(1){
        //LED_BLANCHE = !LED_BLANCHE;
    }
}