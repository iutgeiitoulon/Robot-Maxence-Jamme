/* 
 * File:   main.h
 * Author: GEII Robot
 *
 * Created on 17 f�vrier 2021, 15:53
 */

#ifndef MAIN_H
#define	MAIN_H

#define FCY 40000000

#define STATE_ATTENTE 0
#define STATE_ATTENTE_EN_COURS 1
#define STATE_AVANCE 2
#define STATE_AVANCE_EN_COURS 3
#define STATE_TOURNE_GAUCHE 4
#define STATE_TOURNE_GAUCHE_EN_COURS 5
#define STATE_TOURNE_DROITE 6
#define STATE_TOURNE_DROITE_EN_COURS 7
#define STATE_TOURNE_SUR_PLACE_GAUCHE 8
#define STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS 9
#define STATE_TOURNE_SUR_PLACE_DROITE 10
#define STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS 11
#define STATE_ARRET 12
#define STATE_ARRET_EN_COURS 13
#define STATE_RECULE 14
#define STATE_RECULE_EN_COURS 15

#define PAS_D_OBSTACLE 0
#define OBSTACLE_A_GAUCHE 1
#define OBSTACLE_A_DROITE 2
#define OBSTACLE_EN_FACE 3
#define OSTACLE_A_GAUCHE_ET_A_DROITE 4
#define OSTACLE_A_GAUCHE_ET_EN_FACE 5
#define OSTACLE_A_DROITE_ET_EN_FACE 6
#define OSTACLE_A_DROITE_ET_EN_FACE_ET_A_DROITE 7


#define LED_G 1
#define LED_D 2
#define LED_M 3
#define LED_DM 4
#define LED_GM 5
#define LED_GD 6
#define LED_GMD 7
#define LED_OFF 8

void OperatingSystemLoop(void);


void SetNextRobotStateInAutomaticMode();
int main (void);
void fonction_led(int x);


#endif	/* MAIN_H */

