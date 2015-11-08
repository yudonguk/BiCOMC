#ifndef BICOMC_ARRAY_H__
#define BICOMC_ARRAY_H__

#include "detail/config.h"

#include <utility>

#include "type_traits.h"

#if BICOMC_IS_ARRAY_SUPPORT_COMPILER
#include <array>

namespace bcc
{
	using std::array;
}

#else
#include "stdint.h"

#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace bcc
{
	template<typename T, size_t N>
	class array
	{
	public:
		typedef T value_type;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef value_type& reference;
		typedef value_type const& const_reference;
		typedef value_type* pointer;
		typedef value_type const* const_pointer;
		typedef pointer iterator;
		typedef const_pointer const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	public:
		// element access
		reference at(size_type position)
		{
			if (position >= N) throw std::out_of_range("'position' is out of range");
			return mArray[position];
		}

		BICOMC_CONSTEXPR const_reference at(size_type position) const
		{
			if (position >= N) throw std::out_of_range("'position' is out of range");
			return mArray[position];
		}

		reference operator[](size_type position)
		{
			return mArray[position];
		}

		BICOMC_CONSTEXPR const_reference operator[](size_type position) const
		{
			return mArray[position];
		}

		reference front()
		{
			return mArray[0];
		}

		BICOMC_CONSTEXPR const_reference front() const
		{
			return mArray[0];
		}

		reference back()
		{
			return mArray[N - 1];
		}

		BICOMC_CONSTEXPR const_reference back() const
		{
			return mArray[N - 1];
		}

		pointer data()
		{
			return mArray;
		}

		const_pointer data() const
		{
			return mArray;
		}

	public:
		// iterators
		iterator begin()
		{
			return mArray;
		}

		const_iterator begin() const
		{
			return mArray;
		}

		const_iterator cbegin() const
		{
			return mArray;
		}

		iterator end()
		{
			return mArray + N;
		}

		const_iterator end() const
		{
			return mArray + N;
		}

		const_iterator cend() const
		{
			return mArray + N;
		}

		reverse_iterator rbegin()
		{
			return reverse_iterator(end());
		}

		reverse_iterator rbegin() const
		{
			return const_reverse_iterator(end());
		}

		reverse_iterator crbegin() const
		{
			return const_reverse_iterator(end());
		}

		reverse_iterator rend()
		{
			return reverse_iterator(begin());
		}

		reverse_iterator rend() const
		{
			return const_reverse_iterator(begin());
		}

		reverse_iterator crend() const
		{
			return const_reverse_iterator(begin());
		}

	public:
		// capacity
		BICOMC_CONSTEXPR bool empty() const
		{
			return N == 0;
		}

		BICOMC_CONSTEXPR size_type size() const
		{
			return N;
		}

		BICOMC_CONSTEXPR size_type max_size() const
		{
			return N;
		}

	public:
		// operations
		void fill(const_reference value)
		{
			std::fill_n(mArray, N, value);
		}

		void swap(array& rhs)
		{
			if (this == &rhs) return;
			std::swap_ranges(mArray, mArray + N, rhs.mArray);
		}

	public:
		value_type mArray[N == 0 ? 1 : N];
	};

	template<typename T, size_t N>
	bool operator==(array<T, N> const& lhs, array<T, N> const& rhs)
	{
		if (&lhs == &rhs) return true;
		return std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template<typename T, size_t N>
	bool operator!=(array<T, N> const& lhs, array<T, N> const& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T, size_t N>
	bool operator<(array<T, N> const& lhs, array<T, N> const& rhs)
	{
		if (&lhs == &rhs) return false;
		return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template<typename T, size_t N>
	bool operator<=(array<T, N> const& lhs, array<T, N> const& rhs)
	{
		return !(rhs < lhs);
	}

	template<typename T, size_t N>
	bool operator>(array<T, N> const& lhs, array<T, N> const& rhs)
	{
		return rhs < lhs;
	}

	template<typename T, size_t N>
	bool operator>=(array<T, N> const& lhs, array<T, N> const& rhs)
	{
		return !(lhs < rhs);
	}
} // namespace bcc

namespace std
{
	template<typename T, size_t N>
	void swap(bcc::array<T, N>& lhs, bcc::array<T, N>& rhs)
	{
		lhs.swap(rhs);
	}
} // namespace std

#endif // BICOMC_IS_ARRAY_SUPPORT_COMPILER

namespace bcc
{
	template<typename TupleType>
	struct tuple_size;

	template<typename T, size_t N>
	struct tuple_size<array<T, N> >
		: public bcc::integral_constant<size_t, N>
	{};

	template<size_t index, typename TupleType>
	struct tuple_element;

	template<size_t index, typename T, size_t N>
	struct tuple_element<index, array<T, N> >
	{
		static_assert(index < N, "'index' is out of range");
		typedef T type;
	};

	template<size_t index, typename T, size_t N>
	BICOMC_CONSTEXPR T& get(array<T, N>& a)
	{
		static_assert(index < N, "'index' is out of range");
		return a[index];
	}

	template<size_t index, typename T, size_t N>
	BICOMC_CONSTEXPR T const& get(array<T, N> const& a)
	{
		static_assert(index < N, "'index' is out of range");
		return a[index];
	}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<size_t index, typename T, size_t N>
	BICOMC_CONSTEXPR T&& get(array<T, N>&& a)
	{
		static_assert(index < N, "'index' is out of range");
		return std::move(a[index]);
	}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

} // namespace bcc

#endif // !def BICOMC_ARRAY_H__
