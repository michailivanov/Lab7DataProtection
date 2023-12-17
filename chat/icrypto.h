#include <string.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

int encrypt_aes128(const unsigned char *, int, const unsigned char *, unsigned char *);
int decrypt_aes128(const unsigned char *, int, const unsigned char *, unsigned char *);

#define ENABLE_CRYPTO
