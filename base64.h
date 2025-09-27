#ifndef BASE64_H
#define BASE64_H

#include <string>
#include <vector>

// Base64 encoding table
static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

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
        T[(unsigned char)base64_table[i]] = i;
    }

    int val = 0;
    int valb = -8;
    for (unsigned char c : encoded) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            decoded.push_back((val >> valb) & 0xFF);
            valb -= 8;
        }
    }

    return decoded;
}

#endif // BASE64_H
