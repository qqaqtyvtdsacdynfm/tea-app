# TEA + Base64 Command-Line Tool

A cross-platform C++ implementation of the **Tiny Encryption Algorithm (TEA)** with **Base64 input/output**.  
This tool encrypts and decrypts Base64-encoded data using a Base64-encoded 128-bit key.  

It compiles and runs on **Windows, Linux, and macOS** without modifications.  

---

## ✨ Features
- Implements the **Tiny Encryption Algorithm (TEA)** (32 rounds, 64-bit block, 128-bit key).
- **Base64 input/output** for simple text-based integration.
- Works as a **command-line filter** (reads from `stdin`, writes to `stdout`).
- Cross-platform: builds with **MSVC**, **GCC**, or **Clang**.
- Safe C++17 code, no OS-specific dependencies.

---

## 🔧 Build

### Linux / macOS
```bash
g++ -std=c++17 -O2 main.cpp -o tea
```

### Windows (MSVC)
```powershell
cl /EHsc /O2 main.cpp
```

---

## 🚀 Usage

Run in either `ENCRYPT` or `DECRYPT` mode:

```bash
./tea ENCRYPT
./tea DECRYPT
```

The program expects input lines of the form:

```
<key_base64> <data_base64>
```

and will output the **Base64-encoded result**.  
Type `EXIT` to quit.

---

## 🔑 Example

Key (128-bit, Base64):  
```
AAAAAAAAAAAAAAAAAAAAAA==
```

Plaintext `"Hello World!"` in Base64:  
```
SGVsbG8gV29ybGQh
```

Encrypt:
```bash
echo "AAAAAAAAAAAAAAAAAAAAAA== SGVsbG8gV29ybGQh" | ./tea ENCRYPT
```

Decrypt:
```bash
echo "AAAAAAAAAAAAAAAAAAAAAA== <encrypted_b64>" | ./tea DECRYPT
```

---

## 📚 Notes
- Input is padded automatically to a multiple of 8 bytes.
- Output is always Base64 for portability.
- **This project is for learning/demonstration purposes only.**  
  For production cryptography, use modern ciphers like **AES** with a vetted library (e.g. OpenSSL, libsodium).

---

## 📜 License
MIT License – free to use, modify, and share.
