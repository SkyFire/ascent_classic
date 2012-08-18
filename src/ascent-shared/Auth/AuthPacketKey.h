#ifndef AUTH_PACKET_KEY_H
#define AUTH_PACKET_KEY_H

#include "Common.h"

class AutheticationPacketKey
{
	public:
        ASCENT_INLINE static void GenerateKey(uint8 *key, uint8 *sessionkey)
		{
			const uint8 SeedKeyLen = 16;
			uint8 SeedKey[SeedKeyLen] = { 0x38, 0xA7, 0x83, 0x15, 0xF8, 0x92, 0x25, 0x30, 0x71, 0x98, 0x67, 0xB1, 0x8C, 0x4, 0xE2, 0xAA };

			uint8 *firstBuffer = new uint8[64];
			uint8 *secondBuffer = new uint8[64];

			memset(firstBuffer, 0x36, 64);
			memset(secondBuffer, 0x5C, 64);
    
			for(uint8 i = 0; i < SeedKeyLen; ++i)
			{
				firstBuffer[i] = (uint8)(SeedKey[i] ^ firstBuffer[i]);
				secondBuffer[i] = (uint8)(SeedKey[i] ^ secondBuffer[i]);
			}

			Sha1Hash sha1;
			sha1.UpdateData(firstBuffer, 64);
			sha1.UpdateData(sessionkey, 40);
			sha1.Finalize();

			uint8 *tempDigest = sha1.GetDigest();
			Sha1Hash sha2;
			sha2.UpdateData(secondBuffer, 64);
			sha2.UpdateData(tempDigest, 20);
			sha2.Finalize();

			memcpy(key, sha2.GetDigest(), 20);

			delete firstBuffer;
			delete secondBuffer;
		}
};

#endif
