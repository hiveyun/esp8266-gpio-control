#include <EEPROM.h>
#include "storage.h"

void storage_read(char * buff, int addrStart, int length) {
    for (int i = 0; i < length; ++i) {
        buff[i] = char(EEPROM.read(i + addrStart));
    }
}

void storage_write(char * buff, int addrStart, int length) {
    for (int i = 0; i < length; ++i) {
        EEPROM.write(i + addrStart, buff[i]);
    }
    EEPROM.commit();
}
