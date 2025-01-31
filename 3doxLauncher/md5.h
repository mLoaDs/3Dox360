#ifndef _CRYPTO_MD5_H_
#define	_CRYPTO_MD5_H_

#include <xtl.h>
 
#define	MD5_DIGEST_LENGTH	16

/* MD5 context. */
typedef struct {
	UINT32 state[4];	/* state (ABCD) */
	UINT32 count[2];	/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
} MD5_CTX;

extern void __cdecl MD5Init(MD5_CTX *);
extern void __cdecl MD5Update(MD5_CTX *, const void *, unsigned int);
extern void __cdecl MD5Final(unsigned char [MD5_DIGEST_LENGTH], MD5_CTX *);
 

#endif /* _CRYPTO_MD5_H_ */