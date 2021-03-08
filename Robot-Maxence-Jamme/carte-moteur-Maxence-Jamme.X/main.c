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
    //PWMSetSpeed(-15, MOTEUR_GAUCHE);  //  + = avance | - = recule | (batterie face � nous)
    while(1){
        unsigned int * resultat;
        resultat = ADCGetResult();
        if (ADCIsConversionFinished() == 1){
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result[4])*3.3/4096*3.2;
            robotState.distanceTelemetreDroit = 34/volts-5;
            volts = ((float) result[3])*3.3/4096*3.2;
            robotState.distanceTelemetreExtremeDroit = 34/volts-5;
            volts = ((float) result[2])*3.3/4096*3.2;
            robotState.distanceTelemetreCentre = 34/volts-5;
            volts = ((float) result[1])*3.3/4096*3.2;
            robotState.distanceTelemetreGauche = 34/volts-5;
            volts = ((float) result[0])*3.3/4096*3.2;
            robotState.distanceTelemetreExtremeGauche= 34/volts-5;
            
        }
    }
}

unsigned char stateRobot;
int temps_demi_tour = 1000;
int fonce = 0;
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
            PWMSetSpeedConsigne(fonce, MOTEUR_DROIT);
            PWMSetSpeedConsigne(fonce, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
        break;
        
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
        break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
        break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;  

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(-13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
        break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
        break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_TOURNE_PETIT_GAUCHE:
            PWMSetSpeedConsigne(-5, MOTEUR_DROIT);
            PWMSetSpeedConsigne(13, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_PETIT_GAUCHE_EN_COURS;
        break;
        case STATE_TOURNE_PETIT_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;  
        
        case STATE_TOURNE_PETIT_DROITE:
            PWMSetSpeedConsigne(13, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-5, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_PETIT_DROITE_EN_COURS;
        break;
        case STATE_TOURNE_PETIT_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
        break;

        case STATE_DEMI_TOUR_DROITE:
            timestamp = 0;
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
            stateRobot = STATE_DEMI_TOUR_DROITE_EN_COURS;
        break;
        case STATE_DEMI_TOUR_DROITE_EN_COURS:
            if (timestamp > temps_demi_tour)
                SetNextRobotStateInAutomaticMode();
                break;
        break;

        case STATE_DEMI_TOUR_GAUCHE:
            timestamp = 0;
            PWMSetSpeedConsigne(20, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-20, MOTEUR_GAUCHE);
            stateRobot = STATE_DEMI_TOUR_GAUCHE_EN_COURS;
        break;
        case STATE_DEMI_TOUR_GAUCHE_EN_COURS:
            if (timestamp > temps_demi_tour)
                SetNextRobotStateInAutomaticMode();
                break;
        break;

        default :
            stateRobot = STATE_ATTENTE;
        break;
    }
}

unsigned char nextStateRobot=0;

int dist_1 = 35;
int dist_2 = 25;
int dist_3 = 5;
int dist_4 = 25;
int old_dir = 0;    //permet de savoir le dernier sens vers lequel a tourner le robot
int flag = 1 ;
void SetNextRobotStateInAutomaticMode(){
    unsigned char positionObstacle = PAS_D_OBSTACLE;

    //D�termination de la position des obstacles en fonction des t�l�m�tres
    if ( robotState.distanceTelemetreExtremeGauche < dist_4 ){ 
        positionObstacle = OSTACLE_LOIN_A_GAUCHE;
    }else if ( robotState.distanceTelemetreExtremeDroit < dist_4 ){ 
        positionObstacle = OSTACLE_LOIN_A_DROITE;
    }else if ( robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreDroit > dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche > dist_1 && robotState.distanceTelemetreExtremeDroit > dist_3 ){ 
        positionObstacle = OSTACLE_LOIN_A_GAUCHE;
    }else if ( robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreDroit > dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche > dist_1 && robotState.distanceTelemetreExtremeDroit < dist_3 ){ 
        positionObstacle = OSTACLE_LOIN_A_DROITE;
    }else if(robotState.distanceTelemetreExtremeDroit < dist_3 && robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreDroit > dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche > dist_1){
        positionObstacle = OSTACLE_LOIN_A_DROITE;
    }else if(robotState.distanceTelemetreExtremeDroit > dist_3 && robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreDroit > dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche > dist_1){
        positionObstacle = OSTACLE_LOIN_A_GAUCHE;
    }else if ( robotState.distanceTelemetreDroit < dist_1 && robotState.distanceTelemetreCentre < dist_2 && robotState.distanceTelemetreGauche < dist_1){ //Obstacle � droite et au milieu et � gauche
        positionObstacle = OSTACLE_A_DROITE_ET_EN_FACE_ET_A_DROITE;
    }else if ( robotState.distanceTelemetreDroit < dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche < dist_1){ //Obstacle � droite et � gauche
        positionObstacle = OSTACLE_A_GAUCHE_ET_A_DROITE;
    }else if ( robotState.distanceTelemetreDroit < dist_1 && robotState.distanceTelemetreCentre < dist_2 && robotState.distanceTelemetreGauche > dist_1){ //Obstacle � droite et en face
        positionObstacle = OSTACLE_A_DROITE_ET_EN_FACE;
    }else if ( robotState.distanceTelemetreDroit > dist_1 && robotState.distanceTelemetreCentre < dist_2 && robotState.distanceTelemetreGauche < dist_1){ //Obstacle � gauche et en face
        positionObstacle = OSTACLE_A_GAUCHE_ET_EN_FACE;
    }else if ( robotState.distanceTelemetreDroit < dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche > dist_1){ //Obstacle � droite
        positionObstacle = OBSTACLE_A_DROITE;
    }else if(robotState.distanceTelemetreDroit > dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche < dist_1){ //Obstacle � gauche
        positionObstacle = OBSTACLE_A_GAUCHE;
    }else if(robotState.distanceTelemetreCentre < dist_2){ //Obstacle en face
        positionObstacle = OBSTACLE_EN_FACE;
    }else if(robotState.distanceTelemetreDroit > dist_1 && robotState.distanceTelemetreCentre > dist_2 && robotState.distanceTelemetreGauche > dist_1){ //pas d?obstacle
        positionObstacle = PAS_D_OBSTACLE;
        flag = 1;
    }
    if(flag ==0){
        fonce = 0;
    }else{
        fonce = fonce + 21;
        flag = 0;
        if(fonce>25){
            fonce = 25;
        }
    }
    


//    // codage : | -> capteur detecte   ET   0 -> capteur ne detecte rien
//    if(robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreGauche < dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit < dist_2 && robotState.distanceTelemetreExtremeDroit < dist_3){
//        //Capteurs : | | | | |  ->  Demi tour dans le dernier sens effectuer    32
//        positionObstacle = OBSTACLE_PARTOUT;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche < dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit < dist_2 && robotState.distanceTelemetreExtremeDroit < dist_3){
//        //Capteurs : 0 | | | |  ->  Tourne vers la gauche    16
//        positionObstacle = OBSTACLE_A_DROITE;
//    }else if(robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreGauche < dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit < dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : | | | | 0  ->  Tourne vers la droite    31
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit < dist_2 && robotState.distanceTelemetreExtremeDroit < dist_3){
//        //Capteurs : 0 0 | | |  ->  Tourne vers la gauche    8
//        positionObstacle = OBSTACLE_A_DROITE;
//    }else if(robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreGauche < dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : | | | 0 0  ->  Tourne vers la droite    29
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre > dist_1 && robotState.distanceTelemetreDroit < dist_2 && robotState.distanceTelemetreExtremeDroit < dist_3){
//        //Capteurs : 0 0 0 | |  ->  Tourne vers la gauche    4
//        positionObstacle = OBSTACLE_A_DROITE;
//    }else if(robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreGauche < dist_2 && robotState.distanceTelemetreCentre > dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : | | 0 0 0  ->  Tourne vers la droite    25
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre > dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit < dist_3){
//        //Capteurs : 0 0 0 0 |  ->  Tourne vers la gauche    2
//        positionObstacle = OBSTACLE_A_DROITE;
//    }else if(robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre > dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : | 0 0 0 0  ->  Tourne vers la droite    17
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre > dist_1 && robotState.distanceTelemetreDroit < dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : 0 0 0 | 0  ->  Tourne vers la gauche    3
//        positionObstacle = OBSTACLE_A_DROITE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche < dist_2 && robotState.distanceTelemetreCentre > dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : 0 | 0 0 0  ->  Tourne vers la droite    9
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit < dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : 0 0 | | 0  ->  Tourne vers la gauche    7
//        positionObstacle = OSTACLE_A_DROITE_ET_EN_FACE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche < dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : 0 | | 0 0  ->  Tourne vers la droite    13
//        positionObstacle = OSTACLE_A_GAUCHE_ET_EN_FACE;
//    }else if(robotState.distanceTelemetreExtremeGauche > dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit < dist_3){
//        //Capteurs : 0 0 | 0 |  ->  Tourne vers la gauche    6
//        positionObstacle = OBSTACLE_A_DROITE;
//    }else if(robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : | 0 | 0 0  ->  Tourne vers la droite    21
//        positionObstacle = OBSTACLE_A_GAUCHE;
//    }else if(robotState.distanceTelemetreExtremeGauche < dist_3 && robotState.distanceTelemetreGauche > dist_2 && robotState.distanceTelemetreCentre < dist_1 && robotState.distanceTelemetreDroit > dist_2 && robotState.distanceTelemetreExtremeDroit > dist_3){
//        //Capteurs : 0 0 0 0 0  ->  Tout droit   21
//        positionObstacle = PAS_D_OBSTACLE;
//    }else{
//        //Capteurs : On à Droite et à Gauche necessite un DEMI-TOUR par la D ou par la G.
//        positionObstacle = OBSTACLE_PARTOUT;
//    }
    


    //Determination de l etat a venir du robot
    if (positionObstacle == PAS_D_OBSTACLE){
        nextStateRobot = STATE_AVANCE;
        fonction_led(LED_OFF);
    }else if (positionObstacle == OBSTACLE_A_DROITE){
        nextStateRobot = STATE_TOURNE_GAUCHE;
        old_dir = 0;
        fonction_led(LED_D);
    }else if (positionObstacle == OBSTACLE_A_GAUCHE){
        nextStateRobot = STATE_TOURNE_DROITE;
        old_dir = 1;
        LED_ORANGE = 1;
        fonction_led(LED_G);
    }else if (positionObstacle == OBSTACLE_EN_FACE){
        if(old_dir==0) {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        }else{
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        }        
        fonction_led(LED_M);
    }else if (positionObstacle == OSTACLE_A_DROITE_ET_EN_FACE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        old_dir = 0;
        fonction_led(LED_DM);
    }else if (positionObstacle == OSTACLE_A_GAUCHE_ET_EN_FACE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        old_dir = 1;
        fonction_led(LED_GM);
    }else if (positionObstacle == OSTACLE_A_GAUCHE_ET_A_DROITE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        fonction_led(LED_GD);
    }else if (positionObstacle == OSTACLE_A_DROITE_ET_EN_FACE_ET_A_DROITE){
        if(old_dir==0) {
            nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        }else{
            nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
        } 
        fonction_led(LED_GMD);
    }else if (positionObstacle == OSTACLE_A_EXTREME_DROITE){
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
        fonction_led(LED_GMD);
    }else if (positionObstacle == OSTACLE_LOIN_A_DROITE){
        nextStateRobot = STATE_TOURNE_PETIT_GAUCHE;
        fonction_led(LED_GMD);
    }else if (positionObstacle == OSTACLE_LOIN_A_GAUCHE){
        nextStateRobot = STATE_TOURNE_PETIT_DROITE;
        fonction_led(LED_GD);
    }else if (positionObstacle == OSTACLE_LOIN_EN_FACE){
        if(old_dir==0) {
            nextStateRobot = STATE_TOURNE_PETIT_GAUCHE;
        }else{
            nextStateRobot = STATE_TOURNE_PETIT_DROITE;
        }
        fonction_led(LED_GD);
    }else if (positionObstacle == OBSTACLE_PARTOUT){        
        if(old_dir==0) {
            nextStateRobot = STATE_DEMI_TOUR_GAUCHE;
        }else{
            nextStateRobot = STATE_DEMI_TOUR_DROITE;
        }
    }
    //Si l?on n?est pas dans la transition de l?�tape en cours
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