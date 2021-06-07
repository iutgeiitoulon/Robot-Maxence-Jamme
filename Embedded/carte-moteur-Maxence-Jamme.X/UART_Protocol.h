/* 
 * File:   UART_Protocol.h
 * Author: GEII Robot
 *
 * Created on 7 juin 2021, 10:02
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* msgPayload);

void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* msgPayload);

void UartDecodeMessage(unsigned char c);

void UartProcessDecodedMessage(unsigned char function,unsigned char payloadLength, unsigned char* payload);

#define StateReceptionWaiting 0
#define StateReceptionFunctionMSB 1
#define StateReceptionFunctionLSB 2
#define StateReceptionPayloadLengthMSB 3
#define StateReceptionPayloadLengthLSB 4
#define StateReceptionPayload 5
#define StateReceptionCheckSum 6
#endif	/* UART_PROTOCOL_H */

