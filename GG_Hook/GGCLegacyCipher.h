#pragma once
namespace COGG {
	class GGCLegacyCipher
	{
	private:
		uint8_t* BufIV;
		uint8_t* BufKey;
		uint16_t EncryptCounter;
		uint16_t DecryptCounter;
	public:
		void GenerateIV(int32_t P, int32_t G);
		void Encrypt(uint8_t* pBuf, int32_t Length);
		void Decrypt(uint8_t* pBuf, int32_t Length);
		void ResetCounters();
		GGCLegacyCipher();
		~GGCLegacyCipher();
	};
}

