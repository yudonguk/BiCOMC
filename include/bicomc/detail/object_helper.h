#ifndef BICOMC_DETAIL_OBJECT_HELPER_H__
#define BICOMC_DETAIL_OBJECT_HELPER_H__

#include "config.h"

#include <iostream>
#include <iomanip>

#include "../stdint.h"

#include "hash.h"

namespace bcc
{
	class Object;

namespace detail
{
	struct ProbeDeducer
	{
		static size_t const MAX_PROBE_SIZE = 100;

		struct RawHash
		{
			bcc::uint8_t hash[8];
			bcc::uint8_t subhash[8];
		};
		static_assert(sizeof(RawHash) == 16, "'Hash' size must equal 16.");

		typedef bcc::conditional <
			sizeof(Hash) <= sizeof(bcc::uintptr_t)
			, bcc::uintptr_t
			, bcc::uintptr_t[sizeof(Hash) / sizeof(bcc::uintptr_t) + (sizeof(Hash) % sizeof(bcc::uintptr_t) == 0 ? 0 : 1)]
		>::type HashAligner;

		struct Info
		{
			union
			{
				RawHash hash;
				HashAligner _____aligner_____;
			};
			bcc::uintptr_t count;
			char const* names[MAX_PROBE_SIZE];
		};
		static_assert(sizeof(Info) == sizeof(HashAligner) + sizeof(bcc::uintptr_t) * (MAX_PROBE_SIZE + 1)
			, "'Info' size must equal 'sizeof(HashAligner) + sizeof(bcc::uintptr_t) * (MAX_PROBE_SIZE + 1)'.");

		struct Table
		{
			Info& info;
			void(*functions[MAX_PROBE_SIZE])();
		};

		template<size_t size>
		struct type
		{
			bcc::uintptr_t depth;
			bcc::intptr_t first;
			bcc::intptr_t next;
			Table* tables[size];
		};
	};

	template<size_t tableSize>
	std::ostream& operator<<(std::ostream& stream, ProbeDeducer::type<tableSize> const& probe)
	{
		std::ostream::fmtflags const flags = stream.flags();

		stream << std::dec;
		stream << "depth : " << probe.depth << std::endl;
		stream << "first : " << probe.first << std::endl;
		stream << "next  : " << probe.next << std::endl;

		for (size_t i = 0, size = probe.depth + 1; i < size; ++i)
		{
			ProbeDeducer::Table& table = *probe.tables[i];
			ProbeDeducer::RawHash& hash = table.info.hash;

			stream << "table of '" << table.info.names[0] << "'" << std::endl;
			stream << "    " << "hash    : ";
			stream << std::hex;
			for (size_t j = 0; j < sizeof(hash.hash); ++j)
				stream << std::setfill('0') << std::setw(2) << std::uppercase << static_cast<int>(hash.hash[j]);
			stream << std::endl;

			stream << "    " << "subhash : ";
			stream << std::hex;
			for (size_t j = 0; j < sizeof(hash.subhash); ++j)
				stream << std::setfill('0') << std::setw(2) << std::uppercase << static_cast<int>(hash.subhash[j]);
			stream << std::endl;

			bcc::uintptr_t const count = table.info.count;
			stream << std::dec;
			stream << "    " << "count   : " << count << std::endl;

			for (bcc::uintptr_t j = 0; j < count; ++j)
			{
				stream << "    " << "0x";
				stream << std::hex;
				stream << std::setfill('0') << std::setw(2 * sizeof(void*)) << std::uppercase;
				stream << reinterpret_cast<bcc::uintptr_t>(table.functions[j]) << " : ";
				stream << table.info.names[j + 1] << std::endl;
			}
		}

		stream.flags(flags);
		return stream;
	}

	template<size_t index, size_t depth, typename FunctionTypes>
	struct Function
	{
		typedef typename bcc::tuple_element<index, typename bcc::tuple_element<depth, FunctionTypes>::type>::type MethodType;
		typedef typename MethodType::deducer Deducer;
		typedef typename Deducer::helper* type;
	};

	class ObjectHelper
	{
	public:
		static size_t const VFTABLE_HEADER_SIZE = 3; // because of depth, first, next
		static size_t const INHERITANCE_DEPTH_INDEX = 0;
		static size_t const FIRST_OFFSET_INDEX = 1;
		static size_t const NEXT_OFFSET_INDEX = 2;

		static size_t const INTERFACE_INFO_INDEX = 0;

		static size_t const INTERFACE_HASH_INDEX = 0;
		static size_t const INTERFACE_COUNT_INDEX = sizeof(bcc::detail::Hash) / sizeof(bcc::uintptr_t) + (sizeof(bcc::detail::Hash) % sizeof(bcc::uintptr_t) == 0 ? 0 : 1);
		static size_t const INTERFACE_NAME_INDEX = INTERFACE_COUNT_INDEX + 1;

	public:
		static bcc::uintptr_t inheritanceDepth(bcc::Object const& object);
		static bcc::uintptr_t inheritanceDepth(bcc::Object const volatile& object);

		static bcc::Object* first(bcc::Object& object);
		static bcc::Object const* first(bcc::Object const& object);
		static bcc::Object volatile* first(bcc::Object volatile& object);
		static bcc::Object const volatile* first(bcc::Object const volatile& object);

		static bcc::Object* next(bcc::Object& object);
		static bcc::Object const* next(bcc::Object const& object);
		static bcc::Object volatile* next(bcc::Object volatile& object);
		static bcc::Object const volatile* next(bcc::Object const volatile& object);

		static bcc::detail::Hash const& hash(bcc::Object const& object, size_t depth);
		static bcc::detail::Hash const& hash(bcc::Object const& object);

		static bcc::uintptr_t methodCount(bcc::Object const& object, size_t depth);
		static bcc::uintptr_t methodCount(bcc::Object const& object);

		static char const* name(bcc::Object const& object, size_t depth);
		static char const* name(bcc::Object const& object);

		static void setTable(bcc::Object const& object, void** table);

		static bool isMultiple(bcc::Object const& object);
		static bool isMultiple(bcc::Object const volatile& object);

		static bool isCompatible(bcc::Object const& object, bcc::Object const& target);
		static bool isCompatible(bcc::Object const volatile& object, bcc::Object const& target);

		static bcc::Object* cast(bcc::Object& object, bcc::Object const& target);
		static bcc::Object const* cast(bcc::Object const& object, bcc::Object const& target);
		static bcc::Object volatile* cast(bcc::Object volatile& object, bcc::Object const& target);
		static bcc::Object const volatile* cast(bcc::Object const volatile& object, bcc::Object const& target);

		template<size_t depth, typename FuntionTypes>
		static typename bcc::tuple_element<depth, FuntionTypes>::type& ownTable(bcc::Object const& object);

		template<size_t depth, typename FuntionTypes>
		static typename bcc::tuple_element<depth, FuntionTypes>::type& ownTable(bcc::Object const volatile& object);

		template<size_t index, size_t depth, typename FunctionTables>
		static typename Function<index, depth, FunctionTables>::type& function(bcc::Object const& object);

		template<size_t index, size_t depth, typename FunctionTables>
		static typename Function<index, depth, FunctionTables>::type& function(bcc::Object const volatile& object);

		template<typename T>
		static ProbeDeducer::type<bcc::tuple_size<typename T::BiCOMC_Function_Types__>::value> const& vftable(T const& object);
	};
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_OBJECT_HELPER_H__
