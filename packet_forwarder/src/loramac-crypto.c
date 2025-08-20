/*
██╗ ██████╗ ██████╗  ██████╗  ██████╗
██║██╔═══██╗██╔══██╗██╔═══██╗██╔════╝
██║██║   ██║██████╔╝██║   ██║██║
██║██║   ██║██╔══██╗██║   ██║██║
██║╚██████╔╝██║  ██║╚██████╔╝╚██████╗
╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝  ╚═════╝

Copyright (C) 2013, 2014 IBM Corporation
Copyright (C) 2013, 2014 Semtech Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>
#include <stdint.h>
#include "loramac-crypto.h" // Note: Header changed for local compilation

// #################################################
// ## AES implementation from open-source LMIC   ##
// #################################################

// S-box
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

// Round constant
static const uint8_t RCON[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36
};

static uint8_t AES_Key[16];
static uint8_t AES_Buffer[16];

static void aes_addRoundKey(uint8_t round) {
    uint8_t i;
    for (i=0; i<16; i++)
        AES_Buffer[i] ^= AES_Key[i + (round << 4)];
}

static void aes_subBytes(void) {
    uint8_t i;
    for (i=0; i<16; i++)
        AES_Buffer[i] = sbox[AES_Buffer[i]];
}

static void aes_shiftRows(void) {
    uint8_t t;
    //
    t = AES_Buffer[1];
    AES_Buffer[1] = AES_Buffer[5];
    AES_Buffer[5] = AES_Buffer[9];
    AES_Buffer[9] = AES_Buffer[13];
    AES_Buffer[13] = t;
    //
    t = AES_Buffer[2];
    AES_Buffer[2] = AES_Buffer[10];
    AES_Buffer[10] = t;
    t = AES_Buffer[6];
    AES_Buffer[6] = AES_Buffer[14];
    AES_Buffer[14] = t;
    //
    t = AES_Buffer[15];
    AES_Buffer[15] = AES_Buffer[11];
    AES_Buffer[11] = AES_Buffer[7];
    AES_Buffer[7] = AES_Buffer[3];
    AES_Buffer[3] = t;
}

static uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1b : 0x00);
}

static void aes_mixColumns(void) {
    uint8_t i, a, b, c, d, e;
    for (i=0; i<16; i+=4) {
        a = AES_Buffer[i];
        b = AES_Buffer[i+1];
        c = AES_Buffer[i+2];
        d = AES_Buffer[i+3];
        e = a ^ b ^ c ^ d;
        AES_Buffer[i]   ^= e ^ xtime(a^b);
        AES_Buffer[i+1] ^= e ^ xtime(b^c);
        AES_Buffer[i+2] ^= e ^ xtime(c^d);
        AES_Buffer[i+3] ^= e ^ xtime(d^a);
    }
}

static void aes_cipher(void) {
    uint8_t round;
    aes_addRoundKey(0);
    for (round=1; round<10; ++round) {
        aes_subBytes();
        aes_shiftRows();
        aes_mixColumns();
        aes_addRoundKey(round);
    }
    aes_subBytes();
    aes_shiftRows();
    aes_addRoundKey(10);
}

static void aes_key_expansion (const uint8_t* key) {
    uint8_t i, j, k, t;
    // copy key
    for (i=0; i<16; i++)
        AES_Key[i] = key[i];
    // expand key
    for (i=16, k=0; i < 176; i+=4, k++) {
        // b
        t = AES_Key[i-1];
        // rot
        t = (t << 8) | (t >> 0); // No rotation for uint8_t in C
        // sub
        t = (sbox[(t >> 8) & 0xFF] & 0xFF) | ((sbox[t & 0xFF] & 0xFF) << 8); // simplified for uint8_t
        t = sbox[t];
        // xor
        t ^= RCON[k];
        //
        for (j=0; j<4; j++) {
            AES_Key[i+j] = AES_Key[i-16+j] ^ t;
            t = AES_Key[i+j];
        }
    }
}

void aes_encrypt(const uint8_t* buf, const uint8_t* key) {
    uint8_t i;
    aes_key_expansion(key);
    for(i=0; i<16; i++)
        AES_Buffer[i] = buf[i];
    aes_cipher();
}

// #################################################
// ## CMAC implementation from open-source LMIC  ##
// #################################################

static void LShift(const uint8_t* In, uint8_t* Out) {
    int i;
    uint8_t Overflow = 0;
    for( i = 15; i >= 0; i-- ) {
        Out[i] = In[i] << 1;
        Out[i] |= Overflow;
        Overflow = ( In[i] & 0x80 ) ? 1 : 0;
    }
}

static void Xor(const uint8_t* In, uint8_t* Out) {
    int i;
    for( i = 0; i < 16; i++ ) {
        Out[i] ^= In[i];
    }
}

static void aes_cmac (uint8_t* buf, int len, const uint8_t* key, uint8_t* cmac) {
    uint8_t i;
    uint8_t K1[16];
    uint8_t K2[16];
    uint8_t X[16] = { 0 };

    // generate subkeys K1, K2
    aes_encrypt(X, key);
    memcpy(K1, AES_Buffer, 16);
    LShift(K1, K1);
    if( (AES_Buffer[0] & 0x80) != 0 ) K1[15] ^= 0x87;
    LShift(K1, K2);
    if( (K1[0] & 0x80) != 0 ) K2[15] ^= 0x87;

    // process buffer
    while (len > 0) {
        for (i=0; i<16 && i<len; i++) X[i] ^= buf[i];
        if (len > 16) {
            len -= 16;
            buf += 16;
            aes_encrypt(X, key);
            memcpy(X, AES_Buffer, 16);
        } else {
            for (i=len; i<16; i++) X[i] ^= (i==len) ? 0x80 : 0x00;
            Xor(K2, X);
            aes_encrypt(X, key);
            memcpy(X, AES_Buffer, 16);
            len = 0;
        }
    }
    memcpy(cmac, X, 16); // return X
}


// #################################################
// ## LoRaMac crypto interface                   ##
// #################################################

void LoRaMacComputeMic( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint32_t *mic ) {
    uint8_t micBuff[16];
    uint8_t B0[16] = { 0 };

    B0[0] = 0x49;
    B0[5] = dir;
    B0[6] = address & 0xFF;
    B0[7] = ( address >> 8 ) & 0xFF;
    B0[8] = ( address >> 16 ) & 0xFF;
    B0[9] = ( address >> 24 ) & 0xFF;
    B0[10] = sequenceCounter & 0xFF;
    B0[11] = ( sequenceCounter >> 8 ) & 0xFF;
    B0[12] = ( sequenceCounter >> 16 ) & 0xFF;
    B0[13] = ( sequenceCounter >> 24 ) & 0xFF;
    B0[15] = size & 0xFF;

    aes_cmac(B0, 16, key, micBuff);
    aes_cmac((uint8_t*)buffer, size, key, micBuff);

    *mic = micBuff[0] | (micBuff[1]<<8) | (micBuff[2]<<16) | (micBuff[3]<<24);
}


void LoRaMacPayloadEncrypt( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *encBuffer ) {
    uint8_t bufferIndex = 0;
    uint16_t i;
    uint8_t sBlock[16] = { 0 };
    uint8_t aBlock[16] = { 0 };

    aBlock[0] = 0x01;
    aBlock[5] = dir;
    aBlock[6] = address & 0xFF;
    aBlock[7] = ( address >> 8 ) & 0xFF;
    aBlock[8] = ( address >> 16 ) & 0xFF;
    aBlock[9] = ( address >> 24 ) & 0xFF;
    aBlock[10] = sequenceCounter & 0xFF;
    aBlock[11] = ( sequenceCounter >> 8 ) & 0xFF;
    aBlock[12] = ( sequenceCounter >> 16 ) & 0xFF;
    aBlock[13] = ( sequenceCounter >> 24 ) & 0xFF;

    while( size > 0 )
    {
        aBlock[15] = ( ( bufferIndex / 16 ) + 1 ) & 0xFF;
        aes_encrypt(aBlock, key);
        memcpy(sBlock, AES_Buffer, 16);

        for( i = 0; i < 16 && size > 0; i++ )
        {
            encBuffer[bufferIndex] = buffer[bufferIndex] ^ sBlock[i];
            size--;
            bufferIndex++;
        }
    }
}
