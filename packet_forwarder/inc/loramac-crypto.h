/********************************************************************************
 *      Copyright:  (C) 2025 Chen Chongyuan<1309010124@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  loramac-crypto.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(19/08/25)
 *         Author:  lorawan  <m15540097929@163.com>
 *      ChangeLog:  1, Release initial version on "19/08/25 20:27:15"
 *                 
 ********************************************************************************/

#ifndef __LORAMAC_CRYPTO_H__
#define __LORAMAC_CRYPTO_H__

#include <stdint.h>

/* !
 *  * Computes the LoRaMAC frame MIC field
 *  *
 *  * \param [IN]  buffer          - Data buffer
 *  * \param [IN]  size            - Data buffer size
 *  * \param [IN]  key             - AES key to be used
 *  * \param [IN]  address         - Frame address
 *  * \param [IN]  dir             - Frame direction [0: uplink, 1: downlink]
 *  * \param [IN]  sequenceCounter - Frame sequence counter
 *  * \param [OUT] mic             - Computed MIC field
 *  */
void LoRaMacComputeMic( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint32_t *mic );

/* !
 *  * Encrypts the payload
 *  *
 *  * \param [IN]  buffer          - Data buffer
 *  * \param [IN]  size            - Data buffer size
 *  * \param [IN]  key             - AES key to be used
 *  * \param [IN]  address         - Frame address
 *  * \param [IN]  dir             - Frame direction [0: uplink, 1: downlink]
 *  * \param [IN]  sequenceCounter - Frame sequence counter
 *  * \param [OUT] encBuffer       - Encrypted buffer
 *  */
void LoRaMacPayloadEncrypt( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *encBuffer );

#endif // __LORAMAC_CRYPTO_H__*/*/
