/*
 * Copyright 2015 Kangwon National University and Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BICOMC_DETAIL_METHOD_RETURN_H__
#define BICOMC_DETAIL_METHOD_RETURN_H__

#include "config.h"

#include <stdexcept>

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

			static std::size_t const size = sizeof(type) / sizeof(bcc::uintptr_t)
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

#if !defined(BICOMC_NO_RVALUE_REFERENCE)
			// assign operator must be called once
			void operator=(type&& rhs)
			{
				new (mData) type(std::move(rhs));
				state(DATA_ASSIGNED);
			}
#endif // !defined(BICOMC_NO_RVALUE_REFERENCE)

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

#if !defined(BICOMC_NO_RVALUE_REFERENCE)
		static type const& toReturn(type const& rhs) { return rhs; }
		static type&& toReturn(type&& rhs) { return std::move(rhs); }
		static type const& toReturn(Holder& rhs) { return rhs.data(); }
		static type&& toReturn(Holder&& rhs) { return std::move(rhs.data()); }
		static type const& fromReturn(type const& rhs) { return rhs; }
		static type&& fromReturn(type&& rhs) { return std::move(rhs); }
#else
		static type const& toReturn(type const& rhs) { return rhs; }
		static type const& toReturn(Holder& rhs) { return rhs.data(); }
		static type const& fromReturn(type const& rhs) { return rhs; }
#endif // !defined(BICOMC_NO_RVALUE_REFERENCE)
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

#if !defined(BICOMC_NO_RVALUE_REFERENCE)
	template<typename T>
	struct ReturnHelper<T&&>
	{
		typedef T&& type;
		typedef T* mediator;

		static mediator fromReturn(type rhs) { return bcc::addressof(rhs); }
		static type toReturn(mediator rhs) { return std::move(*rhs); }
	};
#endif // !defined(BICOMC_NO_RVALUE_REFERENCE)
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_METHOD_RETURN_H__
