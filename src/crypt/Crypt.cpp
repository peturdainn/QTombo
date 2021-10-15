#include <stdio.h>
#include <stdlib.h>
#include "Crypt.h"

extern "C" {
        void *BF_Init(unsigned char *key, unsigned keylen);
        void BF_Enc(void *handle, unsigned char *chipher, unsigned char *plain, int len);
        void BF_Dec(void *handle, unsigned char *plain, unsigned char *chipher);
	void BF_Free(void *handle);
};


Crypt::Crypt() : handle(NULL)
{
}

Crypt::~Crypt()
{
	if (handle != NULL) {
		BF_Free(handle);
	}
}

bool Crypt::ResetStream(unsigned char *key, unsigned int len)
{
	if (handle != NULL) {
		BF_Free(handle);
		handle = NULL;
	}

	handle = BF_Init(key, len);
        if (handle == NULL) return false;

        return true;
}

bool Crypt::Encrypt(unsigned char *chipher, unsigned char *plain, int len)
{
	if (handle == NULL) {
                return false;
	}
	BF_Enc(handle, chipher, plain, len);
        return true;
}

bool Crypt::Decrypt(unsigned char *plain, unsigned char *chipher)
{
	if (handle == NULL) {
                return false;
	}
	BF_Dec(handle, plain, chipher);
        return true;
}
