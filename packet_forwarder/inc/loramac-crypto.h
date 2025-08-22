#ifndef __LORAMAC_CRYPTO_H__
#define __LORAMAC_CRYPTO_H__

#include <stdint.h>

void LoRaMacComputeMic(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint32_t *mic);
void LoRaMacPayloadEncrypt(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *encBuffer);

#endif // __LORAMAC_CRYPTO_H__
