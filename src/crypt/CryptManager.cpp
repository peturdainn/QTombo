#include <QtCore>
#include <qfile.h>
#include <stdio.h>
#include <stdlib.h>
#include "CryptManager.h"

// ported from Tombo to Qt (as little as possible) to get encryption in a working state.
// To be replaced by Qt encryption framework later....


void WipeOutAndDelete(char *p, unsigned int len);

bool CryptManager::Init(QString key)
{
        getMD5Sum(md5key, key.toLatin1().begin(), key.length());
        return true;
}

//////////////////////////////////////////////////
// ƒfƒXƒgƒ‰ƒNƒ^
//////////////////////////////////////////////////
// ƒZƒLƒ…ƒٹƒeƒBڈمپA•غژ‌‚µ‚ؤ‚¢‚½md5key‚ًڈء‹ژ‚·‚éپB

CryptManager::~CryptManager()
{
        for (unsigned int i = 0; i < 16; i++) {
		md5key[i] = 0;
	}
}

//////////////////////////////////////////////////
// ƒfپ[ƒ^‚جˆأچ†‰»
//////////////////////////////////////////////////

bool CryptManager::Encrypt(unsigned char* pBuf, int len)
{
        if (len == 0) return false;

        if (!crypt.ResetStream(md5key, 16)) return false;
        unsigned char buf[8];
        unsigned char* p = pBuf;
	int n = len;
	int i;
	while (n > 8) {
		for (i = 0; i < 8; i++) {
			buf[i] = p[i];
		}
		crypt.Encrypt(p, buf, 8);
		p += 8;
		n -= 8;
	}
	if (n > 0) {
		for (i = 0; i < n; i++) {
			buf[i] = p[i];
		}
		crypt.Encrypt(p, buf, n);
	}

	for (i = 0; i < 8; i++) buf[i] = 0;
        return true;
}

//////////////////////////////////////////////////
// ƒfپ[ƒ^‚ج•œچ†
//////////////////////////////////////////////////

bool CryptManager::Decrypt(unsigned char* pBuf, int len)
{
        if (len == 0) return false;

        if (!crypt.ResetStream(md5key, 16)) return false;

        unsigned char buf[8];
        unsigned char* p = pBuf;
	int n = len;
	int i;
	while (n >= 8) {
		for (i = 0; i < 8; i++) {
			buf[i] = p[i];
		}
		crypt.Decrypt(p, buf);
		p += 8;
		n -= 8;
	}

	for (i = 0; i < 8; i++) buf[i] = 0;
        return true;
}

//////////////////////////////////////////////////
// ƒfپ[ƒ^‚جˆأچ†‰»‚ئƒtƒ@ƒCƒ‹‚ض‚ج•غ‘¶
//////////////////////////////////////////////////

bool CryptManager::EncryptAndStore(const unsigned char* pData, int nSize, char* pFileName)
{
	int len;
        unsigned char* pBuf = EncryptBuffer(pData, nSize, &len);
	// ƒtƒ@ƒCƒ‹‚ض‚ج•غ‘¶
        QFile outf;
        qint64 writelen = len;
        qint64 writtenlen = 0;

        outf.setFileName(pFileName);
        if (!outf.open(QIODevice::ReadWrite)) {
		WipeOutAndDelete((char*)pBuf, len);
                return false;
	}

        writtenlen = outf.write((const char*)pBuf,writelen);
        if (writelen != writtenlen)
        {
                //TCHAR buf[1024];
                //wsprintf(buf, TEXT("CryptManager::EncryptAndStore write failed %d"), GetLastError());
                //MessageBox(NULL, buf, TEXT("DEBUG"), MB_OK);
	}
        //if (!outf.SetEOF()) return false;
        //outf.Close();

	WipeOutAndDelete((char*)pBuf, len);
        return true;
}

//////////////////////////////////////////////////
// Encrypt data and add header
//////////////////////////////////////////////////
// CryptManager‚ة‚و‚éˆأچ†‰»ƒtƒ@ƒCƒ‹‚جƒtƒHپ[ƒ}ƒbƒg
// The format of the container is:
// 0-3  : BF01(4 unsigned chars)
// 4-7  : data length (include randum area + md5sum)(4 unsigned chars)
// 8-15 :* random data(8 unsigned chars)
//16-31 :* md5sum of plain text(16 unsigned chars)
//32-   :* data

// '*' is encrypted.
// 

unsigned char* CryptManager::EncryptBuffer(const unsigned char* pData, int nSize, int *pLen)
{
	int i=0;
	int len = ((nSize >> 3) + 1) * 8;
	len += 24;

	len += 8;

        unsigned char* pBufF = new unsigned char[len];
	if (pBufF == NULL) {
                //SetLastError(ERROR_NOT_ENOUGH_MEMORY);
		return NULL;
	}
        unsigned char* pBuf = pBufF + 8;

	// set random number
	for (i = 0; i < 8; i++) {
                pBuf[i] = (unsigned char)(rand() & 0xFF);
	}

	strncpy((char*)pBufF, "BF01", 4);
	*(int*)(pBufF + 4) = nSize;

	// get md5sum of plain data
        getMD5Sum(pBuf + 8, (char*)pData, nSize);

	// copy plain data
        unsigned char* p = pBuf + 24;
        const unsigned char *q = pData;
	for (i = 0; i < nSize; i++) {
		*p++ = *q++;
	}

	// encryption
	if (!Encrypt(pBuf, nSize + 24)) {
		WipeOutAndDelete((char*)pBufF, len);
		return NULL;
	}
	*pLen = len;
	return pBufF;
}

//////////////////////////////////////////////////
// Load from file and decrypt data
//////////////////////////////////////////////////

unsigned char* CryptManager::LoadAndDecrypt(unsigned int* pSize, char* pFileName)
{
    QFile inf;
    inf.setFileName(pFileName);
    if (!inf.open(QIODevice::ReadWrite)) {
            return NULL;
    }

    unsigned int nFileSize = inf.size();
    if(nFileSize <= 24)
        return NULL; // protect agains bad files

    char version[5];
    qint64 n;
    unsigned int nDataSize;

    n = 4;
    inf.read(version, n);
    version[4] = '\0';
    if (strcmp(version, "BF01") != 0) {
            return NULL;
    }

    n = sizeof(nDataSize);
    inf.read((char*)&nDataSize, n);

    if(nDataSize >= (nFileSize-4))
        return NULL; // protect agains bad files

    unsigned char* pBuf = new unsigned char[nFileSize + 1];
    n = nFileSize - 4 - sizeof(nDataSize);
    inf.read((char*)pBuf, n);

    if (!Decrypt(pBuf, n)) {
            WipeOutAndDelete((char*)pBuf, nFileSize + 1);
            return NULL; // decryption failure (wrong key?)
    }

    // MD5SUM
    unsigned char decriptsum[16];
    getMD5Sum(decriptsum, (char*)pBuf + 24, nDataSize);

    // check md5
    for (int i = 0; i < 16; i++) {
            if (pBuf[8 + i] != decriptsum[i]) {
                    WipeOutAndDelete((char*)pBuf, nFileSize + 1);
                    return NULL;
            }
    }
    pBuf[nDataSize + 24] = '\0';
    *pSize = nDataSize;

    unsigned char* pData = new unsigned char[nDataSize + 2];
    if (pData == NULL) {
            WipeOutAndDelete((char*)pBuf, nFileSize + 2);
            return NULL;
    }
    memcpy(pData, pBuf + 24, nDataSize);
    pData[nDataSize] = '\0';
    pData[nDataSize + 1] = '\0';	// sentinel for the file is UTF16
    WipeOutAndDelete((char*)pBuf, nFileSize + 1);
    return pData;
}

unsigned char* CryptManager::DecryptBuffer(const unsigned char* pCrypted, int nSize)
{
	if (nSize % 8 != 0) {
		return NULL;
	}

        unsigned char* pBuf = new unsigned char[nSize];
	if (pBuf == NULL) return NULL;

	memcpy(pBuf, pCrypted, nSize);

	if (!Decrypt(pBuf + 8, nSize - 8)) {
		WipeOutAndDelete((char*)pBuf, nSize);
		return NULL;
	}

        unsigned int n = *(unsigned int*)(pBuf + 4);

        unsigned char decriptsum[16];
	getMD5Sum(decriptsum, (char*)pBuf + 32, n);

	for (int i = 0; i < 16; i++) {
		if (pBuf[16 + i] != decriptsum[i]) {
			WipeOutAndDelete((char*)pBuf, nSize);
			return NULL;
		}
	}

        unsigned char* pData = new unsigned char[n + 1];
	if (pData == NULL) {
		WipeOutAndDelete((char*)pBuf, nSize);
		return NULL;
	}
	memcpy(pData, pBuf + 32, n);
	pData[n] = 0;

	WipeOutAndDelete((char*)pBuf, nSize);
	return pData;
}

void WipeOutAndDelete(char* p, unsigned int len)
{
        for (unsigned int i = 0; i < len; i++) p[i] = '\0';
	delete [] p;
}
