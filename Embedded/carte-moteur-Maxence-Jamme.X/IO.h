#ifndef IO_H
#define IO_H

//Affectation des pins des LEDS
#define LED_ORANGE _LATC10 
#define LED_BLEUE _LATG7
#define LED_BLANCHE _LATG6

// Prototypes fonctions
void InitIO();
//Définitions des pins pour les hacheurs moteurs
#define MOTEUR1_IN1 _LATB14     //definie dans le IO.C en tant que _TRIS + B14
#define MOTEUR1_IN2 _LATB15     //definie dans le IO.C en tant que ...
#define MOTEUR2_IN1 _LATC6     //definie dans le IO.C en tant que ...
#define MOTEUR2_IN2 _LATC7     //definie dans le IO.C en tant que ...

//Configuration spécifique du moteur gauche
#define MOTEUR_GAUCHE_H_IO_OUTPUT MOTEUR1_IN1
#define MOTEUR_GAUCHE_L_IO_OUTPUT MOTEUR1_IN2
#define MOTEUR_GAUCHE_L_PWM_ENABLE IOCON1bits.PENL
#define MOTEUR_GAUCHE_H_PWM_ENABLE IOCON1bits.PENH
#define MOTEUR_GAUCHE_DUTY_CYCLE PDC1   // PD1 pour hacheur n°1

//Configuration spécifique du moteur droit
#define MOTEUR_DROIT_H_IO_OUTPUT MOTEUR2_IN1
#define MOTEUR_DROIT_L_IO_OUTPUT MOTEUR2_IN2
#define MOTEUR_DROIT_L_PWM_ENABLE IOCON6bits.PENL
#define MOTEUR_DROIT_H_PWM_ENABLE IOCON6bits.PENH
#define MOTEUR_DROIT_DUTY_CYCLE PDC6   // PD6 pour hacheur n°6


#endif /* IO_H */

