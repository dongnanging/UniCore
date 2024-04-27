#pragma once
#include "cryptopp/cryptlib.h"
#include "cryptopp/osrng.h" 
#include "cryptopp/hex.h"
#include "cryptopp/Base64.h"
#include "cryptopp/aes.h"        
#include "cryptopp/seed.h"
#include "cryptopp/des.h"
#include "cryptopp/modes.h"      
#include "cryptopp/filters.h"   




using namespace CryptoPP;
class Cryptonizer
{
public:
	static std::string Encrypt_Base64(const std::string& plain);
	static std::string Decrypt_Base64(const std::string& bcipher);
	static std::string Encrypt_Base64(const std::wstring& plain);
	static std::string Decrypt_Base64(const std::wstring& bcipher);

	static std::wstring Base64ToWstring(const std::string& base64);

	static std::string WstringToBytes(const std::wstring& plain);
	static std::wstring BytesToWstring(const std::string& bytes);

public:
	static std::string Encrypt_SHA256(const std::string& plain);
};