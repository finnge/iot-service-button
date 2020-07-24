#ifndef Authentication
#define Authentication

#include <MFRC522.h>

// Reads UID of current RFID-Chip. Returns -1 if none detected.
unsigned long auth_getUID(MFRC522 *mfrc522);

#endif