#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"

int main ( void) {
    InitOscillator();
    InitIO();
    InitTimer23();
    InitTimer1();
    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;
    while(1){
        //LED_BLANCHE = !LED_BLANCHE;
    }
}