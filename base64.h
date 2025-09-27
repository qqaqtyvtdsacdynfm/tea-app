#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>   // for memset
#include <cstdint>   // for fixed-size integer types

// Base64 encoding table
static const char base64_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Function to encode data to Base64
std::string base64_encode(const std::vector<unsigned char>& data) {
    std::string encoded;
    int val = 0;
    int valb = -6;

    for (unsigned char c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_table[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        encoded.push_back(base64_table[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (encoded.size() % 4) {
        encoded.push_back('=');
    }

    return encoded;
}

// Function to decode Base64 to binary data
std::vector<unsigned char> base64_decode(const std::string& encoded) {
    std::vector<unsigned char> decoded;
    std::vector<int> T(256, -1);

    for (int i = 0; i < 64; i++) {
        T[static_cast<unsigned char>(base64_table[i])] = i;
    }

    int val = 0;
    int valb = -8;
    for (unsigned char c : encoded) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(static_cast<unsigned char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return decoded;
}

#define TEA_ROUND 32
#define DELTA     0x9E3779B9U

void tea_code(uint32_t sz, uint32_t sy, const uint32_t* key, uint32_t* dest) {
    uint32_t y = sy, z = sz, sum = 0;
    uint32_t n = TEA_ROUND;

    while (n-- > 0) {
        y += ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
        sum += DELTA;
        z += ((y << 4 ^ y >> 5) + y) ^ (sum + key[(sum >> 11) & 3]);
    }

    dest[0] = y;
    dest[1] = z;
}

void tea_decode(uint32_t sz, uint32_t sy, const uint32_t* key, uint32_t* dest) {
    uint32_t y = sy, z = sz, sum = DELTA * TEA_ROUND;
    uint32_t n = TEA_ROUND;

    while (n-- > 0) {
        z -= ((y << 4 ^ y >> 5) + y) ^ (sum + key[(sum >> 11) & 3]);
        sum -= DELTA;
        y -= ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
    }

    dest[0] = y;
    dest[1] = z;
}

int tea_encrypt(uint32_t* dest, const uint32_t* src, const uint32_t* key, int size) {
    int resize;

    if (size % 8 != 0) {
        resize = size + 8 - (size % 8);
        std::vector<unsigned char> tmp(resize, 0);
        std::memcpy(tmp.data(), src, size);
        src = reinterpret_cast<const uint32_t*>(tmp.data());
    } else {
        resize = size;
    }

    for (int i = 0; i < resize / 8; i++, dest += 2, src += 2) {
        tea_code(src[1], src[0], key, dest);
    }

    return resize;
}

int tea_decrypt(uint32_t* dest, const uint32_t* src, const uint32_t* key, int size) {
    int resize;

    if (size % 8 != 0) {
        resize = size + 8 - (size % 8);
    } else {
        resize = size;
    }

    for (int i = 0; i < resize / 8; i++, dest += 2, src += 2) {
        tea_decode(src[1], src[0], key, dest);
    }

    return resize;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " ENCRYPT|DECRYPT\n";
        return 1;
    }

    std::string mode = argv[1];
    std::vector<unsigned char> buffer(300000);

    while (true) {
        std::string input;
        if (!std::getline(std::cin, input)) break;
        if (input == "EXIT") break;

        std::istringstream iss(input);
        std::vector<std::string> words;
        std::string word;
        while (iss >> word) {
            words.push_back(word);
        }
        if (words.size() < 2) {
            std::cout << "\n";
            continue;
        }

        std::string keyB64 = words[0];
        std::string txtB64 = words[1];
        auto key = base64_decode(keyB64);
        auto txt = base64_decode(txtB64);

        int size = 0;
        if (mode == "DECRYPT") {
            size = tea_decrypt(
                    reinterpret_cast<uint32_t*>(buffer.data()),
                    reinterpret_cast<const uint32_t*>(txt.data()),
                    reinterpret_cast<const uint32_t*>(key.data()),
                    static_cast<int>(txt.size())
            );
        } else if (mode == "ENCRYPT") {
            size = tea_encrypt(
                    reinterpret_cast<uint32_t*>(buffer.data()),
                    reinterpret_cast<const uint32_t*>(txt.data()),
                    reinterpret_cast<const uint32_t*>(key.data()),
                    static_cast<int>(txt.size())
            );
        } else {
            std::cout << "\n";
            continue;
        }

        std::vector<unsigned char> resBytes(buffer.begin(), buffer.begin() + size);
        std::string resB64 = base64_encode(resBytes);
        std::cout << resB64 << std::endl;
    }
    return 0;
}
