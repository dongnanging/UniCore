#include "stdafx.h"
#include "Cryptonizer.h"

std::string Cryptonizer::Encrypt_Base64(const std::string& plain)
{
    std::string bcipher;
    StringSource ss(plain, true,
        new Base64Encoder(
            new StringSink(bcipher)
        ) // Base64Decoder
    ); // StringSource

    return bcipher;
}

std::string Cryptonizer::Decrypt_Base64(const std::string& bcipher)
{
    std::string plain;
    StringSource ss(bcipher, true,
        new Base64Decoder(
            new StringSink(plain)
        ) // Base64Decoder
    ); // StringSource

    return plain;
}

std::string Cryptonizer::Encrypt_Base64(const std::wstring& plain)
{
    std::string nw_plain(reinterpret_cast<const char*>(plain.c_str()), plain.size() * sizeof(wchar_t));
    //std::string nw_plain(reinterpret_cast<const char*>(plain.c_str()), plain.size() * sizeof(wchar_t));

    std::string bcipher;
    StringSource ss(nw_plain, true,
        new Base64Encoder(
            new StringSink(bcipher)
        ) // Base64Decoder
    ); // StringSource

    return bcipher;
}

std::string Cryptonizer::Decrypt_Base64(const std::wstring& bcipher)
{
    std::string plain;
    StringSource ss(reinterpret_cast<const char*>(bcipher.c_str()), true,
        new Base64Decoder(
            new StringSink(plain)
        ) // Base64Decoder
    ); // StringSource

    return plain;
}

std::wstring Cryptonizer::Base64ToWstring(const std::string& base64)
{
    const auto& plain = Decrypt_Base64(base64);
    return std::wstring(reinterpret_cast<const wchar_t*>(plain.c_str()), plain.size() / 2);
}

std::string Cryptonizer::WstringToBytes(const std::wstring& plain)
{
    return  std::string(reinterpret_cast<const char*>(plain.c_str()), plain.size() * sizeof(wchar_t));
}

std::wstring Cryptonizer::BytesToWstring(const std::string& bytes)
{
    return std::wstring(reinterpret_cast<const wchar_t*>(bytes.c_str()), bytes.size() / 2);
}

std::string Cryptonizer::Encrypt_SHA256(const std::string& plain)
{
    std::string digest;

    static SHA256 hash;
    StringSource ss(plain, true,
        new HashFilter(hash,
            new StringSink(digest)));

    return digest;
}
