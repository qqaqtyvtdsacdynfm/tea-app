#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

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

#define TEA_ROUND		32		// 32 �� �����ϸ�, ���� ���� ����� ������ ����.
#define DELTA			0x9E3779B9	// DELTA �� �ٲ��� ����.

void tea_code(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
{
    register unsigned long y = sy, z = sz, sum = 0;
    unsigned long		n = TEA_ROUND;

    while (n-- > 0)
    {
        y	+= ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
        sum	+= DELTA;
        z	+= ((y << 4 ^ y >> 5) + y) ^ (sum + key[sum >> 11 & 3]);
    }

    *(dest++)	= y;
    *dest	= z;
}

void tea_decode(const unsigned long sz, const unsigned long sy, const unsigned long *key, unsigned long *dest)
{
#pragma warning(disable:4307)
    register unsigned long y = sy, z = sz, sum = DELTA * TEA_ROUND;
#pragma warning(default:4307)

    unsigned long		n = TEA_ROUND;

    while (n-- > 0)
    {
        z -= ((y << 4 ^ y >> 5) + y) ^ (sum + key[sum >> 11 & 3]);
        sum -= DELTA;
        y -= ((z << 4 ^ z >> 5) + z) ^ (sum + key[sum & 3]);
    }

    *(dest++)	= y;
    *dest	= z;
}

int tea_encrypt(unsigned long *dest, const unsigned long *src, const unsigned long * key, int size)
{
    int		i;
    int		resize;

    if (size % 8 != 0)
    {
        resize = size + 8 - (size % 8);
        memset((char *) src + size, 0, resize - size);
    }
    else
        resize = size;

    for (i = 0; i < resize >> 3; i++, dest += 2, src += 2)
        tea_code(*(src + 1), *src, key, dest);

    return (resize);
}

int tea_decrypt(unsigned long *dest, const unsigned long *src, const unsigned long * key, int size)
{
    int		i;
    int		resize;

    if (size % 8 != 0)
        resize = size + 8 - (size % 8);
    else
        resize = size;

    for (i = 0; i < resize >> 3; i++, dest += 2, src += 2)
        tea_decode(*(src + 1), *src, key, dest);

    return (resize);
}

unsigned char* hexStringToBytes(const std::string& hexStr, size_t& outSize) {
    // Remove spaces from the hex string
    std::string hexNoSpaces;
    for (char c : hexStr) {
        if (c != ' ') {
            hexNoSpaces.push_back(c);
        }
    }

    // Calculate the size of the output
    outSize = hexNoSpaces.length() / 2;

    // Allocate memory for the result
    unsigned char* byteArray = new unsigned char[outSize];

    // Convert the hex string to unsigned char*
    for (size_t i = 0; i < outSize; ++i) {
        std::stringstream ss;
        ss << std::hex << hexNoSpaces.substr(i * 2, 2);
        unsigned int byte;
        ss >> byte;
        byteArray[i] = static_cast<unsigned char>(byte);
    }

    return byteArray;
}

std::string bytesToHexString(const unsigned char* byteArray, size_t size) {
    std::stringstream ss;

    // Convert each byte to a two-character hex string
    for (size_t i = 0; i < size; ++i) {
        ss << std::uppercase << std::setw(2) << std::setfill('0')
           << std::hex << static_cast<int>(byteArray[i]) << " ";
    }

    return ss.str();

}



int main(int argc, char* argv[]) {
    if (argc != 2) {
        return 1;
    }

    std::string mode = argv[1];
    auto* buffer = new unsigned char[300000];

    while(true) {
        std::string input;
        std::getline(std::cin, input); // Read a full line
        if(input == "EXIT")
            break;

        std::istringstream iss(input);
        std::vector<std::string> words;
        std::string word;
        while (iss >> word) { // Read words separated by whitespace
            words.push_back(word);
        }
        std::string keyB64 = words[0];
        std::string txtB64 = words[1];
        auto key = base64_decode(keyB64);
        auto txt = base64_decode(txtB64);
        int size;
        if(mode == "DECRYPT")
            size = tea_decrypt(
                    reinterpret_cast<unsigned long*>(buffer),
                    reinterpret_cast<unsigned long*>(txt.data()),
                    reinterpret_cast<const unsigned long*>(key.data()),
                    txt.size()
            );
        else if(mode == "ENCRYPT")
            size = tea_encrypt(
                    reinterpret_cast<unsigned long*>(buffer),
                    reinterpret_cast<unsigned long*>(txt.data()),
                    reinterpret_cast<const unsigned long*>(key.data()),
                    txt.size()
            );
        else {
            std::cout << "" << std::endl;
            continue;
        }

        std::vector<unsigned char> resBytes(buffer, buffer + size);
        std::string resB64 = base64_encode(resBytes);
        std::cout << resB64 << std::endl;
    }

    return 0;
}