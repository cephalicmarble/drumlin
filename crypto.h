#include <sha2.h>

#include <array>
#include <string>
#include "gtypes.h"

namespace drumlin {

namespace crypto {

class Crypto {
    Crypto();
public:
    virtual ~Crypto();

    static std::array<uint8_t, SHA256_DIGEST_LENGTH> sha256(void *buf, guint32 length);
    static std::string sha256(std::string const& data);
};

} // namespace crypto

} // namespace drumlin
