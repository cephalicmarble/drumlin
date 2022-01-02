#include "crypto.h"

// #include <openssl/conf.h>
// #include <openssl/err.h>
// #include <openssl/evp.h>
#include <sha2.h>

#include <array>
#include <algorithm>

namespace drumlin {

namespace crypto {

Crypto::Crypto()
{
    // /* Load the human readable error strings for libcrypto */
    // ERR_load_crypto_strings();
    //
    // /* Load all digest and cipher algorithms */
    // OpenSSL_add_all_algorithms();
    //
    // /* Load config file, and other important initialisation */
    // OPENSSL_config(NULL);
}

Crypto::~Crypto()
{
    // /* Removes all digests and ciphers */
    // EVP_cleanup();
    //
    // /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
    // CRYPTO_cleanup_all_ex_data();
    //
    // /* Remove error strings */
    // ERR_free_strings();
}

std::array<uint8_t, SHA256_DIGEST_LENGTH> Crypto::sha256(void *buf, guint32 length)
{
    SHA2_CTX ctx;
    std::array<uint8_t, SHA256_DIGEST_LENGTH> results;
    SHA256Init(&ctx);
    SHA256Update(&ctx, (uint8_t *)buf, length);
    SHA256Final(results.begin(), &ctx);
    return results;
}

std::string Crypto::sha256(std::string const& data)
{
    std::string str;
    auto chars(sha256((void*)data.c_str(), data.length()));
    std::copy(chars.begin(), chars.end(), std::back_inserter<std::string>(str));
    return str;
}

} // namespace Crypto

} // namespace drumlin
