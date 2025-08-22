#ifndef __AES_H__
#define __AES_H__

#include <stdint.h>

void AES_Encrypt(const uint8_t* buffer, const uint8_t* key, uint8_t* encBuffer);

#endif // __AES_H__
