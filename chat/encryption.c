#include "encryption.h"

int encrypt_aes128(
    const unsigned char *plaintext,
    int length,
    const unsigned char *key,
    unsigned char *cipher)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int encrypted_length;

    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        perror("Error: creating EVP_CIPHER_CTX failed\n");
        return -1;
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, NULL))
    {
        perror("Error: initializing AES encryption failed\n");
        return -1;
    }

    if (1 != EVP_EncryptUpdate(ctx, cipher, &len, plaintext, length))
    {
        perror("Error: AES encryption update failed\n");
        return -1;
    }
    encrypted_length = len;

    if (1 != EVP_EncryptFinal_ex(ctx, cipher + len, &len))
    {
        perror("Error: AES encryption finalization failed\n");
        return -1;
    }
    encrypted_length += len;

    EVP_CIPHER_CTX_free(ctx);
    return encrypted_length;
}

int decrypt_aes128(
    const unsigned char *cipher,
    int length,
    const unsigned char *key,
    unsigned char *plain)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int decrypted_length;

    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        perror("Error: creating EVP_CIPHER_CTX failed\n");
        return -1;
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, NULL))
    {
        perror("Error: initializing AES decryption failed\n");
        return -1;
    }

    if (1 != EVP_DecryptUpdate(ctx, plain, &len, cipher, length))
    {
        perror("Error: AES decryption update failed\n");
        return -1;
    }
    decrypted_length = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plain + len, &len))
    {
        perror("Error: AES decryption finalization failed\n");
        return -1;
    }
    decrypted_length += len;

    EVP_CIPHER_CTX_free(ctx);
    return decrypted_length;
}
