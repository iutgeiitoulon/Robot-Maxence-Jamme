#ifndef CB_TX1_H
#define	CB_TX1_H

void SendMessage(unsigned char* message, int length);
void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get();
void SendOne();
unsigned char CB_TX1_IsTranmitting();
int CB_TX1_GetDataSize();
int CB_TX1_RemainingSize();
#endif	/* CB_TX1_H */

