#include <xc.h>
#include "UART_Protocol.h"

unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char * msgPayload){
//Fonction prenant entrée la trame et sa longueur pour calculer le checksum
    unsigned char Checksum = 0;
    Checksum ^= (0xFE);
    Checksum ^= (msgFunction >> 8);
    Checksum ^= (msgFunction >> 0);
    Checksum ^= (msgPayloadLength >> 8);
    Checksum ^= (msgPayloadLength >> 0);
    int i = 0;
    for (i = 0; i < msgPayloadLength; i++)
    {
        Checksum ^= msgPayload[i];
    }            
    return Checksum;
}

void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload){
//Fonction d?encodage et d?envoi d?un message
    unsigned char Checksum = 0;            
    unsigned char trame[msgPayloadLength+6];
    int pos = 0;
    trame[pos++] = (0xFE);
    trame[pos++] = (msgFunction >> 8);
    trame[pos++] = (msgFunction >> 0);
    trame[pos++] = (msgPayloadLength >> 8);
    trame[pos++] = (msgPayloadLength >> 0);
    int i = 0;
    for (i = 0; i < msgPayloadLength; i++)
    {
        trame[pos++] =  msgPayload[i];
    }
    Checksum = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

    trame[pos++] = (Checksum);
    SendMessage(&trame, msgPayloadLength+6);
}

int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
int rcvState = 0;
void UartDecodeMessage(unsigned char c){
//Fonction prenant en entrée un octet et servant à reconstituer les trames
    switch (rcvState)
    {
        case StateReceptionWaiting:
            if(c == 0xFE)
            {
                rcvState = StateReceptionFunctionMSB;
                msgDecodedPayloadLength = 0;
                msgDecodedFunction = 0;
                msgDecodedPayloadIndex = 0;
            }
            break;
        case StateReceptionFunctionMSB:
            msgDecodedFunction = (c<<8);
            rcvState = StateReceptionFunctionLSB;
            break;
        case StateReceptionFunctionLSB:
            msgDecodedFunction += c;
            rcvState = StateReceptionPayloadLengthMSB;
            break;
        case StateReceptionPayloadLengthMSB:
            msgDecodedPayloadLength = (c<<8);
            rcvState = StateReceptionPayloadLengthLSB;
            break;
        case StateReceptionPayloadLengthLSB:
            msgDecodedPayloadLength += c;
            if (msgDecodedPayloadLength > 1500)
            {
                rcvState = StateReceptionWaiting;
            }
            rcvState = StateReceptionPayload;                    
            msgDecodedPayload = new byte[msgDecodedPayloadLength];
            break;
        case StateReceptionPayload:
            msgDecodedPayload[msgDecodedPayloadIndex] = c;
            msgDecodedPayloadIndex++;
            if(msgDecodedPayloadIndex == msgDecodedPayloadLength)
            {
                rcvState = StateReceptionCheckSum;
            }                
        break;
        case StateReceptionCheckSum:
            receivedChecksum = c;
            calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            if (calculatedChecksum == receivedChecksum){
                TextBoxReception.Text += "youpi\n";
                ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            else
            {
                TextBoxReceptionText += "snif";
            }
            rcvState = StateReceptionWaiting;
            break;
        default:
            rcvState = StateReceptionWaiting;
        break;
    }
}

void UartProcessDecodedMessage(unsigned char function,unsigned char payloadLength, unsigned char* payload){
//Fonction appelée après le décodage pour exécuter l?action
//correspondant au message reçu

}

//*************************************************************************/
//Fonctions correspondant aux messages
//*************************************************************************/