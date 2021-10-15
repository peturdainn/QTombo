#ifndef CRYPT_H
#define CRYPT_H

class Crypt {
	void *handle;
public:
	Crypt();
	~Crypt();

        bool ResetStream(unsigned char* key, unsigned int len);
        bool Encrypt(unsigned char* chipher, unsigned char* plain, int len);
        bool Decrypt(unsigned char* plain, unsigned char* chipher);
};

#endif
