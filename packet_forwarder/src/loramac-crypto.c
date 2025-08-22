#include "loramac-crypto.h"
#include "aes.h"
#include <string.h>

static void AES_CMAC(const uint8_t* buffer, uint16_t size, const uint8_t* key, uint8_t* cmac) {
    uint8_t K1[16] = {0}, K2[16] = {0};
    uint8_t X[16] = {0};
    uint8_t* M_i = (uint8_t*)buffer;
    uint16_t n_blocks = (size + 15) / 16;
    uint16_t last_block_size = size % 16 == 0 ? 16 : size % 16;
    uint16_t i; uint8_t j;

    AES_Encrypt(X, key, X);

    for (i = 0; i < 16; i++) {
        K1[i] = X[i] << 1;
        if (i < 15 && X[i+1] & 0x80) K1[i] |= 1;
    }
    if (X[0] & 0x80) K1[15] ^= 0x87;

    for (i = 0; i < 16; i++) {
        K2[i] = K1[i] << 1;
        if (i < 15 && K1[i+1] & 0x80) K2[i] |= 1;
    }
    if (K1[0] & 0x80) K2[15] ^= 0x87;

    if (n_blocks == 0) n_blocks = 1;

    memset(X, 0, 16);
    for (i = 0; i < n_blocks - 1; i++) {
        for (j = 0; j < 16; j++) X[j] ^= M_i[j];
        M_i += 16;
        AES_Encrypt(X, key, X);
    }

    uint8_t M_last[16] = {0};
    memcpy(M_last, M_i, last_block_size);

    if (last_block_size == 16) {
        for (j = 0; j < 16; j++) M_last[j] ^= K1[j];
    } else {
        M_last[last_block_size] = 0x80;
        for (j = 0; j < 16; j++) M_last[j] ^= K2[j];
    }

    for (j = 0; j < 16; j++) X[j] ^= M_last[j];
    AES_Encrypt(X, key, cmac);
}

void LoRaMacComputeMic(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint32_t *mic) {
    uint8_t B0[16] = {0};
    uint8_t micBlock[16];
    uint8_t bufferWithB0[256 + 16];

    B0[0] = 0x49; B0[5] = dir;
    B0[6] = address & 0xFF; B0[7] = (address >> 8) & 0xFF;
    B0[8] = (address >> 16) & 0xFF; B0[9] = (address >> 24) & 0xFF;
    B0[10] = sequenceCounter & 0xFF; B0[11] = (sequenceCounter >> 8) & 0xFF;
    B0[12] = (sequenceCounter >> 16) & 0xFF; B0[13] = (sequenceCounter >> 24) & 0xFF;
    B0[15] = size & 0xFF;

    memcpy(bufferWithB0, B0, 16);
    memcpy(bufferWithB0 + 16, buffer, size);

    AES_CMAC(bufferWithB0, 16 + size, key, micBlock);
    *mic = (uint32_t)micBlock[0] | ((uint32_t)micBlock[1] << 8) | ((uint32_t)micBlock[2] << 16) | ((uint32_t)micBlock[3] << 24);
}

void LoRaMacPayloadEncrypt(const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *encBuffer) {
    uint8_t bufferIndex = 0; uint16_t i;
    uint8_t sBlock[16] = {0}; uint8_t aBlock[16] = {0};

    aBlock[0] = 0x01; aBlock[5] = dir;
    aBlock[6] = address & 0xFF; aBlock[7] = (address >> 8) & 0xFF;
    aBlock[8] = (address >> 16) & 0xFF; aBlock[9] = (address >> 24) & 0xFF;
    aBlock[10] = sequenceCounter & 0xFF; aBlock[11] = (sequenceCounter >> 8) & 0xFF;
    aBlock[12] = (sequenceCounter >> 16) & 0xFF; aBlock[13] = (sequenceCounter >> 24) & 0xFF;

    while (size > 0) {
        aBlock[15] = ((bufferIndex / 16) + 1) & 0xFF;
        AES_Encrypt(aBlock, key, sBlock);
        for (i = 0; i < 16 && size > 0; i++) {
            encBuffer[bufferIndex] = buffer[bufferIndex] ^ sBlock[i];
            size--; bufferIndex++;
        }
    }
}
