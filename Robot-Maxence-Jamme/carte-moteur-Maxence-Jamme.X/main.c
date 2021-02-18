#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"


int main (void) {
    InitOscillator();
    InitIO();
    InitTimer23();
    InitTimer1();
    InitPWM();
    InitADC1();
    InitTimer4();
    //PWMSetSpeed(-15, MOTEUR_DROIT); // 0 = Droite | 1 = Gauche |pourcentage detat bas de PWM 
    //PWMSetSpeed(-15, MOTEUR_GAUCHE);  //  + = avance | - = recule | (batterie face à nous)
    while(1){
        unsigned int * resultat;
        resultat = ADCGetResult();
        if (ADCIsConversionFinished() == 1){
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result[2])*3.3/4096*3.2;
            robotState.distanceTelemetreDroit = 34/volts-5;
            volts = ((float) result[1])*3.3/4096*3.2;
            robotState.distanceTelemetreCentre = 34/volts-5;
            volts = ((float) result[0])*3.3/4096*3.2;
            robotState.distanceTelemetreGauche = 34/volts-5;
        }
        if(robotState.distanceTelemetreDroit<=20){
            LED_BLANCHE = 1;
        }else{
            LED_BLANCHE = 0;
        }
        if(robotState.distanceTelemetreCentre<=20){
            LED_BLEUE = 1;
        }else{
            LED_BLEUE = 0;
        }
        if(robotState.distanceTelemetreGauche<=20){
            LED_ORANGE = 1;
        }else{
            LED_ORANGE = 0;
        }
    }
}

unsigned char stateRobot;

void OperatingSystemLoop(void){
    switch (stateRobot){
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;

        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
                break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(-20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
        break;
        
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
        break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
        break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;  

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
        break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
        break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        default :
            stateRobot = STATE_ATTENTE;
        break;
    }
}

unsigned char nextStateRobot=0;

void SetNextRobotStateInAutomaticMode(){
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    //Détermination de la position des obstacles en fonction des télémètres
    if ( robotState.distanceTelemetreDroit < 20 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 30){ //Obstacle à droite
        positionObstacle = OBSTACLE_A_DROITE;
    }else if(robotState.distanceTelemetreDroit > 20 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche < 30){ //Obstacle à gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    }else if(robotState.distanceTelemetreCentre < 20){ //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    }else if(robotState.distanceTelemetreDroit > 20 && robotState.distanceTelemetreCentre > 30 && robotState.distanceTelemetreGauche > 30){ //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
    }

    //Détermination de l?état à venir du robot
    if (positionObstacle == PAS_D_OBSTACLE){
        nextStateRobot = STATE_AVANCE;
    }else if (positionObstacle == OBSTACLE_A_DROITE){
        nextStateRobot = STATE_TOURNE_GAUCHE;
        LED_BLANCHE = 1;
    }else if (positionObstacle == OBSTACLE_A_GAUCHE){
        nextStateRobot = STATE_TOURNE_DROITE;
        LED_ORANGE = 1;
        
    }else if (positionObstacle == OBSTACLE_EN_FACE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        LED_BLEUE = 1;
    }
    //Si l?on n?est pas dans la transition de l?étape en cours
    if (nextStateRobot != stateRobot - 1){
        stateRobot = nextStateRobot;  
    }

}