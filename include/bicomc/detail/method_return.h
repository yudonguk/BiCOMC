#ifndef BICOMC_DETAIL_METHOD_RETURN_H__
#define BICOMC_DETAIL_METHOD_RETURN_H__

#include "config.h"

#include "../memory.h"

namespace bcc
{
namespace detail
{
	template<typename T>
	struct ReturnHelper
	{
		typedef T type;
		static_assert(sizeof(type), "'T' is not complete type.");

		class Holder
		{
		public:
			enum { DATA_NONE = 0, DATA_ASSIGNED = sizeof(type) };

			static size_t const size = sizeof(type) / sizeof(bcc::uintptr_t)
				+ (sizeof(type) % sizeof(bcc::uintptr_t) == 0 ? 1 : 2);
			
		public:
			Holder()
			{
				state(DATA_NONE);
			}

			~Holder()
			{
				if (state() == DATA_ASSIGNED)
					data().~type();
			}

		private:
			Holder(Holder const&) BICOMC_DELETE;
			Holder& operator=(Holder& rhs) BICOMC_DELETE;

		public:
			// assign operator must be called once
			void operator=(type const& rhs)
			{
				new (mData) type(rhs);
				state(DATA_ASSIGNED);
			}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
			// assign operator must be called once
			void operator=(type&& rhs)
			{
				new (mData) type(std::move(rhs));
				state(DATA_ASSIGNED);
			}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

			// data() must be called after assign operation
			type const& data() const
			{
				if (state() != DATA_ASSIGNED) throw std::runtime_error("'type' is not compatible or data is not assigned.");
				return *reinterpret_cast<type const*>(mData);
			}

			// data() must be called after assign operation
			type& data()
			{
				if (state() != DATA_ASSIGNED) throw std::runtime_error("'type' is not compatible or data is not assigned.");
				return *reinterpret_cast<type*>(mData);
			}

			bcc::uintptr_t state() const
			{
				return mData[size - 1];
			}

			void state(bcc::uintptr_t state_)
			{
				mData[size - 1] = state_;
			}

		private:
			bcc::uintptr_t mData[size];
		};
		static_assert(sizeof(Holder) == Holder::size * sizeof(bcc::uintptr_t)
			, "'Holder' size must equal 'Holder::size * sizeof(bcc::uintptr_t)'.");

		typedef typename bcc::conditional<
			bcc::is_arithmetic<type>::value
			, type, Holder
		>::type mediator;

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		static type const& toReturn(type const& rhs) { return rhs; }
		static type&& toReturn(type&& rhs) { return std::move(rhs); }
		static type const& toReturn(Holder& rhs) { return rhs.data(); }
		static type&& toReturn(Holder&& rhs) { return std::move(rhs.data()); }
		static type const& fromReturn(type const& rhs) { return rhs; }
		static type&& fromReturn(type&& rhs) { return std::move(rhs); }
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		static type const& toReturn(type const& rhs) { return rhs; }
		static type const& toReturn(Holder& rhs) { return rhs.data(); }
		static type const& fromReturn(type const& rhs) { return rhs; }
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	};
	
	template<>
	struct ReturnHelper<void>
	{
		typedef void type;
		typedef void* mediator;

		static type toReturn(mediator rhs) {}
	};

	template<typename T>
	struct ReturnHelper<T*>
	{
		typedef T* type;
		typedef T* mediator;

		static mediator fromReturn(type rhs) { return rhs; }
		static type toReturn(mediator rhs) { return rhs; }
	};

	template<typename T>
	struct ReturnHelper<T&>
	{
		typedef T& type;
		typedef T* mediator;

		static mediator fromReturn(type rhs) { return bcc::addressof(rhs); }
		static type toReturn(mediator rhs) { return *rhs; }
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct ReturnHelper<T&&>
	{
		typedef T&& type;
		typedef T* mediator;

		static mediator fromReturn(type rhs) { return bcc::addressof(rhs); }
		static type toReturn(mediator rhs) { return std::move(*rhs); }
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_METHOD_RETURN_H__
