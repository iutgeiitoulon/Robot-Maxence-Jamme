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
            //volts = ((float) result[3])*3.3/4096*3.2;
            //robotState.distanceTelemetreExtremeDroit = 34/volts-5;
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
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
        break;
        
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
        break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
        break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;  

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
        break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
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

int distance_1 = 20;
int distance_2 = 30;

void SetNextRobotStateInAutomaticMode(){
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    //Détermination de la position des obstacles en fonction des télémètres
    if ( robotState.distanceTelemetreDroit < distance_1 && robotState.distanceTelemetreCentre < distance_2 && robotState.distanceTelemetreGauche < distance_1){ //Obstacle à droite et au milieu et à gauche
        positionObstacle = OSTACLE_A_DROITE_ET_EN_FACE_ET_A_DROITE;
    }else if ( robotState.distanceTelemetreDroit < distance_1 && robotState.distanceTelemetreCentre > distance_2 && robotState.distanceTelemetreGauche < distance_1){ //Obstacle à droite et à gauche
        positionObstacle = OSTACLE_A_GAUCHE_ET_A_DROITE;
    }else if ( robotState.distanceTelemetreDroit < distance_1 && robotState.distanceTelemetreCentre < distance_2 && robotState.distanceTelemetreGauche > distance_1){ //Obstacle à droite et en face
        positionObstacle = OSTACLE_A_DROITE_ET_EN_FACE;
    }else if ( robotState.distanceTelemetreDroit > distance_1 && robotState.distanceTelemetreCentre < distance_2 && robotState.distanceTelemetreGauche < distance_1){ //Obstacle à gauche et en face
        positionObstacle = OSTACLE_A_GAUCHE_ET_EN_FACE;
    }else if ( robotState.distanceTelemetreDroit < distance_1 && robotState.distanceTelemetreCentre > distance_2 && robotState.distanceTelemetreGauche > distance_1){ //Obstacle à droite
        positionObstacle = OBSTACLE_A_DROITE;
    }else if(robotState.distanceTelemetreDroit > distance_1 && robotState.distanceTelemetreCentre > distance_2 && robotState.distanceTelemetreGauche < distance_1){ //Obstacle à gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    }else if(robotState.distanceTelemetreCentre < distance_2){ //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    }else if(robotState.distanceTelemetreDroit > distance_1 && robotState.distanceTelemetreCentre > distance_2 && robotState.distanceTelemetreGauche > distance_1){ //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
    }

    //Détermination de l?état à venir du robot
    if (positionObstacle == PAS_D_OBSTACLE){
        nextStateRobot = STATE_AVANCE;
        fonction_led(LED_OFF);
    }else if (positionObstacle == OBSTACLE_A_DROITE){
        nextStateRobot = STATE_TOURNE_GAUCHE;
        fonction_led(LED_D);
    }else if (positionObstacle == OBSTACLE_A_GAUCHE){
        nextStateRobot = STATE_TOURNE_DROITE;
        LED_ORANGE = 1;
        fonction_led(LED_G);
    }else if (positionObstacle == OBSTACLE_EN_FACE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        fonction_led(LED_M);
    }else if (positionObstacle == OSTACLE_A_DROITE_ET_EN_FACE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        fonction_led(LED_DM);
    }else if (positionObstacle == OSTACLE_A_GAUCHE_ET_EN_FACE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        fonction_led(LED_GM);
    }else if (positionObstacle == OSTACLE_A_GAUCHE_ET_A_DROITE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        fonction_led(LED_GD);
    }else if (positionObstacle == OSTACLE_A_DROITE_ET_EN_FACE_ET_A_DROITE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        fonction_led(LED_GMD);
    }
    //Si l?on n?est pas dans la transition de l?étape en cours
    if (nextStateRobot != stateRobot - 1){
        stateRobot = nextStateRobot;  
    }

}

void fonction_led(int x){
    // LED_BLANCHE = 1;    //DROITE
    // LED_BLEUE = 1;      //MILIEU
    // LED_ORANGE = 1;     //GAUCHE
    switch(x){
        case 1:
            LED_BLANCHE = 0;    //DROITE
            LED_BLEUE = 0;      //MILIEU
            LED_ORANGE = 1;     //GAUCHE
        break;
        case 2:
            LED_BLANCHE = 1;    //DROITE
            LED_BLEUE = 0;      //MILIEU
            LED_ORANGE = 0;     //GAUCHE
        break;
        case 3:
            LED_BLANCHE = 0;    //DROITE
            LED_BLEUE = 1;      //MILIEU
            LED_ORANGE = 0;     //GAUCHE
        break;
        case 4:
            LED_BLANCHE = 1;    //DROITE
            LED_BLEUE = 1;      //MILIEU
            LED_ORANGE = 0;     //GAUCHE
        break;
        case 5:
            LED_BLANCHE = 0;    //DROITE
            LED_BLEUE = 1;      //MILIEU
            LED_ORANGE = 1;     //GAUCHE
        break;
        case 6:
            LED_BLANCHE = 1;    //DROITE
            LED_BLEUE = 0;      //MILIEU
            LED_ORANGE = 1;     //GAUCHE
        break;
        case 7:
            LED_BLANCHE = 1;    //DROITE
            LED_BLEUE = 1;      //MILIEU
            LED_ORANGE = 1;     //GAUCHE
        break;
        case 8: 
            LED_BLANCHE = 0;    //DROITE
            LED_BLEUE = 0;      //MILIEU
            LED_ORANGE = 0;     //GAUCHE
        break;
        default: 
            LED_BLANCHE = 0;    //DROITE
            LED_BLEUE = 0;      //MILIEU
            LED_ORANGE = 0;     //GAUCHE
        break;
    }
}