#ifndef BICOMC_DETAIL_HASH_H__
#define BICOMC_DETAIL_HASH_H__

#include "config.h"

#include "../array.h"
#include "../stdint.h"

namespace bcc
{
namespace detail
{
	struct Hash
	{
		union
		{
			bcc::uint8_t raws[16];
			struct
			{
				bcc::uint64_t hash;
				bcc::uint64_t subhash;
			};
		};
	};
	static_assert(sizeof(Hash) == 16, "'Hash' size must equal 16.");

	inline bool operator==(Hash const& lhs, Hash const& rhs)
	{
		return lhs.hash == rhs.hash && lhs.subhash == rhs.subhash;
	}

	inline bool operator!=(Hash const& lhs, Hash const& rhs)
	{
		return lhs.hash != rhs.hash || lhs.subhash != rhs.subhash;
	}

	template<typename T>
	struct Hasher;
	//	static bcc::uint64_t hash()
	//	static bcc::uint64_t subhash()

	template<typename T>
	struct Hasher<T const>
	{
		static bcc::uint64_t hash()
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash()
		{
			return Hasher<T>::subhash();
		}
	};

	template<typename T>
	struct Hasher<T volatile>
	{
		static bcc::uint64_t hash()
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash()
		{
			return Hasher<T>::subhash();
		}
	};

	template<typename T>
	struct Hasher<T const volatile>
	{
		static bcc::uint64_t hash()
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash()
		{
			return Hasher<T>::subhash();
		}
	};

	template<typename T>
	struct Hasher<T&>
	{
		static bcc::uint64_t hash()
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash()
		{
			return Hasher<T>::subhash();
		}
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct Hasher<T&&>
	{
		static bcc::uint64_t hash()
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash()
		{
			return Hasher<T>::subhash();
		}
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

	template<typename T>
	struct Hasher<T*>
	{
		static bcc::uint64_t hash()
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash()
		{
			return Hasher<T>::subhash();
		}
	};

	class Sha1
	{
	public:
		Sha1()
		{
			clear();
		}

	public:
		void clear()
		{
			mHash[0] = 0x67452301;
			mHash[1] = 0xEFCDAB89;
			mHash[2] = 0x98BADCFE;
			mHash[3] = 0x10325476;
			mHash[4] = 0xC3D2E1F0;

			mBlockIndex = 0;
		}

		void calculate(void const* pData, size_t size)
		{
			calculate(pData, static_cast<bcc::uint8_t const*>(pData) + size);
		}

		void calculate(void const* first, void const* last)
		{
			calculate(static_cast<bcc::uint8_t const*>(first), static_cast<bcc::uint8_t const*>(last));
		}

		void calculate(bcc::uint8_t const* first, bcc::uint8_t const* last)
		{
			clear();

			for (bcc::uint8_t const* itor = first; itor != last; ++itor)
				append(*itor);

			append(0x80);
			if (mBlockIndex > 56)
			{
				for (; mBlockIndex != 0;)
					append(0);
			}
			for (; mBlockIndex < 56;)
				append(0);

			bcc::uint64_t bitSize = (last - first) * 8;

			append(static_cast<bcc::uint8_t>(bitSize >> 56));
			append(static_cast<bcc::uint8_t>(bitSize >> 48));
			append(static_cast<bcc::uint8_t>(bitSize >> 40));
			append(static_cast<bcc::uint8_t>(bitSize >> 32));
			append(static_cast<bcc::uint8_t>(bitSize >> 24));
			append(static_cast<bcc::uint8_t>(bitSize >> 16));
			append(static_cast<bcc::uint8_t>(bitSize >> 8));
			append(static_cast<bcc::uint8_t>(bitSize));
		}

		bcc::array<bcc::uint32_t, 5> get() const
		{
			return mHash;
		}

	private:
		static bool isLittleEndian()
		{
			bcc::uint32_t determiner = 1;
			return *reinterpret_cast<bcc::uint8_t*>(&determiner) != 0;
		}

		static bcc::uint32_t lefRotate(bcc::uint32_t value, size_t count)
		{
			return (value << count) ^ (value >> (8 * sizeof(value) - count));
		}

		static bcc::uint16_t toBigEndian(bcc::uint16_t value)
		{
			if (!isLittleEndian()) return value;
			bcc::uint8_t const* pCursor = reinterpret_cast<bcc::uint8_t const*>(&value);
			return static_cast<bcc::uint16_t>(pCursor[0]) << 8 | pCursor[1];
		}

		inline static bcc::uint32_t toBigEndian(bcc::uint32_t value)
		{
			if (!isLittleEndian()) return value;
			uint8_t const* pCursor = reinterpret_cast<bcc::uint8_t const*>(&value);
			return static_cast<bcc::uint32_t>(pCursor[0]) << 24
				| static_cast<bcc::uint32_t>(pCursor[1]) << 16
				| static_cast<bcc::uint32_t>(pCursor[2]) << 8
				| pCursor[3];
		}

		inline static bcc::uint64_t toBigEndian(bcc::uint64_t value)
		{
			if (!isLittleEndian()) return value;
			bcc::uint8_t const* pCursor = reinterpret_cast<bcc::uint8_t const*>(&value);
			return static_cast<bcc::uint64_t>(pCursor[0]) << 56
				| static_cast<bcc::uint64_t>(pCursor[1]) << 48
				| static_cast<bcc::uint64_t>(pCursor[2]) << 40
				| static_cast<bcc::uint64_t>(pCursor[3]) << 32
				| static_cast<bcc::uint64_t>(pCursor[4]) << 24
				| static_cast<bcc::uint64_t>(pCursor[5]) << 16
				| static_cast<bcc::uint64_t>(pCursor[6]) << 8
				| pCursor[7];
		}

		void append(bcc::uint8_t data)
		{
			mBlock[mBlockIndex++] = data;
			if (mBlockIndex == mBlock.size())
			{
				mBlockIndex = 0;

				bcc::uint32_t words[80];
				for (int i = 0; i < 16; ++i)
					words[i] = toBigEndian(*reinterpret_cast<bcc::uint32_t const*>(mBlock.data() + i * sizeof(bcc::uint32_t)));
				for (int i = 16; i < 80; ++i)
					words[i] = lefRotate(words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16], 1);

				bcc::uint32_t a = mHash[0];
				bcc::uint32_t b = mHash[1];
				bcc::uint32_t c = mHash[2];
				bcc::uint32_t d = mHash[3];
				bcc::uint32_t e = mHash[4];

				for (int i = 0; i < 80; ++i)
				{
					bcc::uint32_t f, k;
					switch (i / 20)
					{
					case 0:
						f = (b & c) | (~b & d);
						k = 0x5A827999;
						break;
					case 1:
						f = b ^ c ^ d;
						k = 0x6ED9EBA1;
						break;
					case 2:
						f = (b & c) | (b & d) | (c & d);
						k = 0x8F1BBCDC;
						break;
					default:
						f = b ^ c ^ d;
						k = 0xCA62C1D6;
						break;
					}

					bcc::uint32_t temp = lefRotate(a, 5) + f + e + k + words[i];
					e = d;
					d = c;
					c = lefRotate(b, 30);
					b = a;
					a = temp;
				}

				mHash[0] += a;
				mHash[1] += b;
				mHash[2] += c;
				mHash[3] += d;
				mHash[4] += e;
			}
		}

	private:
		bcc::array<bcc::uint32_t, 5> mHash;
		bcc::array<bcc::uint8_t, 64> mBlock;
		size_t mBlockIndex;
	};
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_HASH_H__
