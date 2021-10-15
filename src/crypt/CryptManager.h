#ifndef CRYPTMANAGER_H
#define CRYPTMANAGER_H

#include "Crypt.h"

class CryptManager {
	Crypt crypt;
        unsigned char md5key[16];

public:
	~CryptManager();

	// Set password
        bool Init(QString key);

        bool Encrypt(unsigned char* pBuf, int len);

        bool Decrypt(unsigned char* pBuf, int len);

        bool EncryptAndStore(const unsigned char* pData, int nSize, char* pFileName);

        unsigned char* LoadAndDecrypt(unsigned int* pSize, char* pFileName);

        unsigned char* EncryptBuffer(const unsigned char* pData, int nSize, int *pLen);
        unsigned char* DecryptBuffer(const unsigned char* pCrypted, int nSize);
};

extern "C" {
        void getMD5Sum(unsigned char* md5sum, char* in, int len);
};
#endif

