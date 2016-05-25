#ifndef BICOMC_DETAIL_OBJECT_H__
#define BICOMC_DETAIL_OBJECT_H__

#include "config.h"

#include <typeinfo>

#include "../object.h"

#include "error_detail.h"
#include "method_call.h"
#include "safe_static.h"
#include "string_util.h"

namespace bcc
{
namespace detail
{
	class ErrorDetail;

	struct DefaultCallHelper::Helper
	{
		template<typename Origin, typename CastHelper>
		static ErrorDetail* BICOMC_CALL destroy(bcc::Object const volatile* pImpl, bcc::detail::ReturnHelper<void>::mediator*) BICOMC_NOEXCEPT
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				typedef typename bcc::remove_cv<Origin>::type Type;
				static_assert(sizeof(Type), "'Type' is not complete type.");

				delete &bcc::detail::ObjectCaster::cast<Type const volatile, CastHelper const volatile>(*pImpl);
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Origin, typename CastHelper>
		static ErrorDetail* BICOMC_CALL clone(bcc::Object const* pImpl, bcc::detail::ReturnHelper<bcc::Object*>::mediator* pRet) BICOMC_NOEXCEPT
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				typedef typename bcc::remove_cv<Origin>::type Type;
				class Guard
				{
				public:
					Guard(Type* p_) : p(p_) {}
					~Guard() { delete p; }
					
					Type* get() const { return p; }

					Type* release()
					{
						Type* result = p;
						p = nullptr;
						return result;
					}

				private:
					Guard(Guard const&) : p() { throw std::runtime_error("must not be called."); }
					Guard& operator=(Guard const&) { throw std::runtime_error("must not be called."); }
			
				private:
					Type* p;
				} result(new Type(bcc::detail::ObjectCaster::cast<Type const, typename bcc::remove_cv<CastHelper>::type const>(*pImpl)));

				*pRet = bcc::detail::ReturnHelper<bcc::Object*>::fromReturn(static_cast<typename bcc::remove_cv<CastHelper>::type*>(result.get()));
				result.release();
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interfaces, typename MethodType, size_t size = bcc::tuple_size<Interfaces>::value>
		struct OverrideHelper
		{
			typedef typename bcc::tuple_element<size - 1, Interfaces>::type Interface;

			template<typename Impl>
			static void overrideDestroy(Impl& impl)
			{
				MethodType::template overrideMethod<Interface>(impl, &Helper::destroy<Impl, Interface>);
				OverrideHelper<Interfaces, MethodType, size - 1>::overrideDestroy(impl);
			}
			template<typename Impl>
			static void overrideClone(Impl& impl)
			{
				MethodType::template overrideMethod<Interface>(impl, &Helper::clone<Impl, Interface>);
				OverrideHelper<Interfaces, MethodType, size - 1>::overrideClone(impl);
			}
		};

		template<typename Interfaces, typename MethodType>
		struct OverrideHelper<Interfaces, MethodType, 0>
		{
			template<typename Impl> static void overrideDestroy(Impl& impl) {}
			template<typename Impl> static void overrideClone(Impl& impl) {}
		};
	};

	template<typename Interfaces, typename MethodType, typename Impl>
	void DefaultCallHelper::overrideDestroy(Impl& impl)
	{
		Helper::OverrideHelper<Interfaces, MethodType>::template overrideDestroy(impl);
	}

	template<typename Interfaces, typename MethodType, typename Impl>
	void DefaultCallHelper::overrideClone(Impl& impl)
	{
		Helper::OverrideHelper<Interfaces, MethodType>::template overrideClone(impl);
	}

	inline bcc::uintptr_t ObjectHelper::inheritanceDepth(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::uintptr_t>(object.vftable__[INHERITANCE_DEPTH_INDEX]);
	}

	inline bcc::uintptr_t ObjectHelper::inheritanceDepth(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::uintptr_t>(object.vftable__[INHERITANCE_DEPTH_INDEX]);
	}

	inline bcc::uintptr_t ObjectHelper::version(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::uintptr_t>(object.vftable__[BICOMC_VERSION_INDEX]);
	}

	inline bcc::uintptr_t ObjectHelper::version(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::uintptr_t>(object.vftable__[BICOMC_VERSION_INDEX]);
	}

	inline bcc::Object* ObjectHelper::next(bcc::Object& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::Object*>(
			reinterpret_cast<char*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::Object const* ObjectHelper::next(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::Object const*>(
			reinterpret_cast<char const*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::Object volatile* ObjectHelper::next(bcc::Object volatile& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::Object volatile*>(
			reinterpret_cast<char volatile*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::Object const volatile* ObjectHelper::next(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<bcc::Object const volatile*>(
			reinterpret_cast<char const volatile*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::detail::Hash const& ObjectHelper::hash(bcc::Object const& object, size_t depth) BICOMC_NOEXCEPT
	{
		typedef bcc::detail::Hash Result;
		return reinterpret_cast<Result***>(object.vftable__)[depth + VFTABLE_HEADER_SIZE][INTERFACE_INFO_INDEX][INTERFACE_HASH_INDEX];
	}

	inline bcc::detail::Hash const& ObjectHelper::hash(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return hash(object, inheritanceDepth(object));
	}

	inline bcc::uintptr_t ObjectHelper::methodCount(bcc::Object const& object, size_t depth) BICOMC_NOEXCEPT
	{
		typedef bcc::uintptr_t Result;
		return reinterpret_cast<Result***>(object.vftable__)[depth + VFTABLE_HEADER_SIZE][INTERFACE_INFO_INDEX][INTERFACE_COUNT_INDEX];
	}

	inline bcc::uintptr_t ObjectHelper::methodCount(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return methodCount(object, inheritanceDepth(object));
	}

	inline char const* ObjectHelper::name(bcc::Object const& object, size_t depth) BICOMC_NOEXCEPT
	{
		typedef char const* Result;
		return reinterpret_cast<Result***>(object.vftable__)[depth + VFTABLE_HEADER_SIZE][INTERFACE_INFO_INDEX][INTERFACE_NAME_INDEX];
	}

	inline char const* ObjectHelper::name(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return name(object, inheritanceDepth(object));
	}

	inline char const* const* ObjectHelper::signatures(bcc::Object const& object, size_t depth) BICOMC_NOEXCEPT
	{
		typedef char const* Result;
		return reinterpret_cast<Result***>(object.vftable__)[depth + VFTABLE_HEADER_SIZE][INTERFACE_INFO_INDEX] + INTERFACE_NAME_INDEX;
	}

	inline char const* const* ObjectHelper::signatures(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return signatures(object, inheritanceDepth(object));
	}

	inline void ObjectHelper::setTable(bcc::Object const& object, void** table) BICOMC_NOEXCEPT
	{
		const_cast<void**&>(object.vftable__) = table;
	}

	inline bool ObjectHelper::isMultiple(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return object.vftable__[NEXT_OFFSET_INDEX] != nullptr;
	}

	inline bool ObjectHelper::isMultiple(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		return object.vftable__[NEXT_OFFSET_INDEX] != nullptr;
	}

	inline bool ObjectHelper::isCompatible(bcc::Object const& object, bcc::Object const& target) BICOMC_NOEXCEPT
	{
		bcc::uintptr_t const targetDepth = ObjectHelper::inheritanceDepth(target);
		bcc::uintptr_t const targetVersion = ObjectHelper::version(target);

		if (targetVersion != ObjectHelper::version(object)
			|| targetDepth > ObjectHelper::inheritanceDepth(object))
			return false;

		for (size_t i = 0, size = targetDepth + 1; i < size; ++i)
		{
			bcc::uintptr_t const methodCount = ObjectHelper::methodCount(object, i);
			bcc::uintptr_t const targetMethodCount = ObjectHelper::methodCount(target, i);

			if (targetMethodCount > methodCount)
				return false;

			char const* const* sigItor = ObjectHelper::signatures(object, i);
			char const* const* targetSigItor = ObjectHelper::signatures(target, i);
			char const* const* const targetSigEnd = targetSigItor + targetMethodCount + 1; // include interface name

			for (; targetSigItor != targetSigEnd; ++sigItor, ++targetSigItor)
			{
				if (std::strcmp(*sigItor, *targetSigItor) != 0)
					return false;
			}
		}
		return true;
	}

	inline bool ObjectHelper::isCompatible(bcc::Object const volatile& object, bcc::Object const& target) BICOMC_NOEXCEPT
	{
		return ObjectHelper::isCompatible(const_cast<bcc::Object const&>(object), target);
	}
	
	inline bcc::Object* ObjectHelper::cast(bcc::Object& object, bcc::Object const& target) BICOMC_NOEXCEPT
	{
		bcc::uintptr_t const targetDepth = ObjectHelper::inheritanceDepth(target);
		bcc::uintptr_t const targetVersion = ObjectHelper::version(target);

		bcc::Object* itor = &object;
		do
		{
			if (targetVersion != ObjectHelper::version(*itor)
				|| targetDepth > ObjectHelper::inheritanceDepth(*itor))
			{
				itor = ObjectHelper::next(*itor);
				continue;
			}

			bool isMatched = true;
			for (size_t i = 0, size = targetDepth + 1; i < size; ++i)
			{
				bcc::uintptr_t const methodCount = ObjectHelper::methodCount(*itor, i);
				bcc::uintptr_t const targetMethodCount = ObjectHelper::methodCount(target, i);

				if (targetMethodCount > methodCount)
				{
					isMatched = false;
					break;
				}

				bcc::detail::Hash const& hash = ObjectHelper::hash(*itor, i);
				bcc::detail::Hash const& hash2 = ObjectHelper::hash(target, i);

				if (hash.hash != hash2.hash)
				{
					isMatched = false;
					break;
				}

				if (hash.subhash != hash2.subhash)
				{
					char const* const* sigItor = ObjectHelper::signatures(*itor, i);
					char const* const* targetSigItor = ObjectHelper::signatures(target, i);
					char const* const* const targetSigEnd = targetSigItor + targetMethodCount + 1; // include interface name

					for (; targetSigItor != targetSigEnd; ++sigItor, ++targetSigItor)
					{
						if (std::strcmp(*sigItor, *targetSigItor) != 0)
							break;
					}

					if (targetSigItor != targetSigEnd)
					{
						isMatched = false;
						break;
					}
				}
			}

			if (isMatched) return itor;
			itor = ObjectHelper::next(*itor);
		} while (itor != &object);

		return nullptr;
	}

	inline bcc::Object const* ObjectHelper::cast(bcc::Object const& object, bcc::Object const& target) BICOMC_NOEXCEPT
	{
		return ObjectHelper::cast(const_cast<bcc::Object&>(object), target);
	}

	inline bcc::Object volatile* ObjectHelper::cast(bcc::Object volatile& object, bcc::Object const& target) BICOMC_NOEXCEPT
	{
		return ObjectHelper::cast(const_cast<bcc::Object&>(object), target);
	}

	inline bcc::Object const volatile* ObjectHelper::cast(bcc::Object const volatile& object, bcc::Object const& target) BICOMC_NOEXCEPT
	{
		return ObjectHelper::cast(const_cast<bcc::Object&>(object), target);
	}
	
	template<size_t depth, typename FuntionTypes>
	typename tuple_element<depth, FuntionTypes>::type& ObjectHelper::ownTable(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		typedef typename tuple_element<depth, FuntionTypes>::type FunctionTable;
		return *reinterpret_cast<FunctionTable*>(object.vftable__[depth + VFTABLE_HEADER_SIZE]);
	}

	template<size_t depth, typename FuntionTypes>
	typename tuple_element<depth, FuntionTypes>::type& ObjectHelper::ownTable(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		typedef typename tuple_element<depth, FuntionTypes>::type FunctionTable;
		return *reinterpret_cast<FunctionTable*>(object.vftable__[depth + VFTABLE_HEADER_SIZE]);
	}

	template<size_t index, size_t depth, typename FunctionTables>
	typename Function<index, depth, FunctionTables>::type& ObjectHelper::function(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		typedef typename Function<index, depth, FunctionTables>::type F;
		return reinterpret_cast<F&>(reinterpret_cast<void**>(object.vftable__[depth + VFTABLE_HEADER_SIZE])[index]);
	}
	
	template<size_t index, size_t depth, typename FunctionTables>
	typename Function<index, depth, FunctionTables>::type& ObjectHelper::function(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		typedef typename Function<index, depth, FunctionTables>::type F;
		return reinterpret_cast<F&>(reinterpret_cast<void**>(object.vftable__[depth + VFTABLE_HEADER_SIZE])[index]);
	}

	template<typename T>
	ProbeDeducer::type<bcc::tuple_size<typename T::BiCOMC_Function_Types__>::value> const& ObjectHelper::vftable(T const& object) BICOMC_NOEXCEPT
	{
		return *reinterpret_cast<ProbeDeducer::type<bcc::tuple_size<typename T::BiCOMC_Function_Types__>::value> const*>(object.vftable__);
	}
} // namespace detail

	inline Object::Object()
		: vftable__(nullptr)
	{
		typedef bcc::detail::TableHolder<Object> Holder;
		typedef bcc::detail::SafeStatic<Holder, Object> StaticHolder;

		Holder* pHolder = StaticHolder::get();
		if (!pHolder) pHolder = StaticHolder::init(Holder::initializer(*this));
		bcc::detail::ObjectHelper::setTable(*this, pHolder->vftable.data());
	}

	inline Object::~Object()
	{}

namespace detail
{
	template<typename Target>
	struct bimcomc_cast_impl
	{
		static_assert(true, "Can not cast type.");
	};

	template<typename Target>
	struct bimcomc_cast_impl<Target*>
	{
		template<typename U>
		static Target* cast(U* pObject) BICOMC_NOEXCEPT
		{
			static_assert(bcc::is_interface<Target>::value, "'Target' type must be interface.");
			if (!pObject) return nullptr;
			class Temp : public bcc::remove_cv<Target>::type {} target;
			return static_cast<Target*>(ObjectHelper::cast(*pObject, target));
		}
	};

	template<typename Target>
	struct bimcomc_cast_impl<Target&>
	{
		template<typename U>
		static Target& cast(U& object)
		{
			Target* p = bimcomc_cast_impl<Target*>::cast(bcc::addressof(object));
			if (!p) throw std::bad_cast();
			return *p;
		}
	};
} // namespace detail

	template<typename T, typename U>
	T bicomc_cast(U& object)
	{
		return detail::bimcomc_cast_impl<T>::cast(object);
	}

	template<typename T, typename U>
	T bicomc_cast(U* object) BICOMC_NOEXCEPT
	{
		return detail::bimcomc_cast_impl<T>::cast(object);
	}
} // namespace bcc

#endif // !def BICOMC_DETAIL_OBJECT_H__
