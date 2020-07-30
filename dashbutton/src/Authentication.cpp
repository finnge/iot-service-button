#include "Authentication.h"

unsigned long auth_getUID(MFRC522 *mfrc522) {
    if (!(*mfrc522).PICC_ReadCardSerial()) {
        return 0;
    }
    unsigned long hex_num = 0;
    hex_num = (*mfrc522).uid.uidByte[0] << 24;
    hex_num += (*mfrc522).uid.uidByte[1] << 16;
    hex_num += (*mfrc522).uid.uidByte[2] << 8;
    hex_num += (*mfrc522).uid.uidByte[3];
    (*mfrc522).PICC_HaltA();

    return hex_num;
}