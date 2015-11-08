#ifndef BICOMC_DETAIL_OBJECT_H__
#define BICOMC_DETAIL_OBJECT_H__

#include "config.h"

#include <typeinfo>

#include "../object.h"

#include "error_detail.h"
#include "method_call.h"
#include "string_util.h"

namespace bcc
{
namespace detail
{
	class ErrorDetail;
	
	template<typename T>
	struct DefaultCallHelper::Helper
	{
		static ErrorDetail* BICOMC_CALL destroy(bcc::Object const volatile& impl, bcc::detail::ReturnHelper<void>::mediator&)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				typedef typename bcc::remove_cv<T>::type Type;
				static_assert(sizeof(Type), "'T' is not complete type.");
				
				delete &bcc::detail::ObjectCaster::cast<Type const volatile>(impl);
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename HelpInterface>
		static ErrorDetail* BICOMC_CALL clone(bcc::Object const& impl, bcc::detail::ReturnHelper<bcc::Object*>::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				typedef typename bcc::remove_cv<T>::type Type;
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
					Guard(Guard const&) BICOMC_DELETE;
					Guard& operator=(Guard const&) BICOMC_DELETE;
			
				private:
					Type* p;
				} result(new Type(bcc::detail::ObjectCaster::cast<Type const>(impl)));
				
				ret = bcc::detail::ReturnHelper<bcc::Object*>::fromReturn(static_cast<HelpInterface*>(result.get()));
				result.release();
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename T>
	typename MethodTypeDeducer<void(), bcc::Object, true, true>::helper* DefaultCallHelper::destroy(T const volatile& impl)
	{
		return &Helper<T>::destroy;
	}

	template<typename HelpInterface, typename T>
	typename MethodTypeDeducer<bcc::Object*(), bcc::Object, true, false>::helper* DefaultCallHelper::clone(T const& impl)
	{
		return &Helper<T>::template clone<HelpInterface>;
	}
		
	inline bcc::uintptr_t ObjectHelper::inheritanceDepth(bcc::Object const& object)
	{
		return reinterpret_cast<bcc::uintptr_t>(object.vftable__[INHERITANCE_DEPTH_INDEX]);
	}

	inline bcc::uintptr_t ObjectHelper::inheritanceDepth(bcc::Object const volatile& object)
	{
		return reinterpret_cast<bcc::uintptr_t>(object.vftable__[INHERITANCE_DEPTH_INDEX]);
	}

	inline bcc::Object* ObjectHelper::first(bcc::Object& object)
	{
		return reinterpret_cast<bcc::Object*>(
			reinterpret_cast<char*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[FIRST_OFFSET_INDEX])
		);
	}

	inline bcc::Object const* ObjectHelper::first(bcc::Object const& object)
	{
		return reinterpret_cast<bcc::Object const*>(
			reinterpret_cast<char const*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[FIRST_OFFSET_INDEX])
		);
	}

	inline bcc::Object volatile* ObjectHelper::first(bcc::Object volatile& object)
	{
		return reinterpret_cast<bcc::Object volatile*>(
			reinterpret_cast<char volatile*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[FIRST_OFFSET_INDEX])
		);
	}

	inline bcc::Object const volatile* ObjectHelper::first(bcc::Object const volatile& object)
	{
		return reinterpret_cast<bcc::Object const volatile*>(
			reinterpret_cast<char const volatile*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[FIRST_OFFSET_INDEX])
		);
	}
	
	inline bcc::Object* ObjectHelper::next(bcc::Object& object)
	{
		return reinterpret_cast<bcc::Object*>(
			reinterpret_cast<char*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::Object const* ObjectHelper::next(bcc::Object const& object)
	{
		return reinterpret_cast<bcc::Object const*>(
			reinterpret_cast<char const*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::Object volatile* ObjectHelper::next(bcc::Object volatile& object)
	{
		return reinterpret_cast<bcc::Object volatile*>(
			reinterpret_cast<char volatile*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::Object const volatile* ObjectHelper::next(bcc::Object const volatile& object)
	{
		return reinterpret_cast<bcc::Object const volatile*>(
			reinterpret_cast<char const volatile*>(&object)
			+ reinterpret_cast<bcc::intptr_t>(object.vftable__[NEXT_OFFSET_INDEX])
		);
	}

	inline bcc::detail::Hash const& ObjectHelper::hash(bcc::Object const& object, size_t depth)
	{
		typedef bcc::detail::Hash Result;
		return reinterpret_cast<Result***>(object.vftable__)[depth + VFTABLE_HEADER_SIZE][INTERFACE_INFO_INDEX][INTERFACE_HASH_INDEX];
	}

	inline bcc::detail::Hash const& ObjectHelper::hash(bcc::Object const& object)
	{
		return hash(object, inheritanceDepth(object));
	}

	inline bcc::uintptr_t ObjectHelper::methodCount(bcc::Object const& object, size_t depth)
	{
		typedef bcc::uintptr_t Result;
		return reinterpret_cast<Result***>(object.vftable__)[depth + VFTABLE_HEADER_SIZE][INTERFACE_INFO_INDEX][INTERFACE_COUNT_INDEX];
	}

	inline bcc::uintptr_t ObjectHelper::methodCount(bcc::Object const& object)
	{
		return methodCount(object, inheritanceDepth(object));
	}

	inline char const* ObjectHelper::name(bcc::Object const& object, size_t depth)
	{
		typedef char const* Result;
		return reinterpret_cast<Result***>(object.vftable__)[depth + VFTABLE_HEADER_SIZE][INTERFACE_INFO_INDEX][INTERFACE_NAME_INDEX];
	}

	inline char const* ObjectHelper::name(bcc::Object const& object)
	{
		return name(object, inheritanceDepth(object));
	}

	inline void ObjectHelper::setTable(bcc::Object const& object, void** table)
	{
		const_cast<void**&>(object.vftable__) = table;
	}

	inline bool ObjectHelper::isMultiple(bcc::Object const& object)
	{
		return object.vftable__[NEXT_OFFSET_INDEX] != nullptr;
	}

	inline bool ObjectHelper::isMultiple(bcc::Object const volatile& object)
	{
		return object.vftable__[NEXT_OFFSET_INDEX] != nullptr;
	}

	inline bool ObjectHelper::isCompatible(bcc::Object const& object, bcc::Object const& target)
	{
		bcc::uintptr_t const targetDepth = ObjectHelper::inheritanceDepth(target);

		if (targetDepth > ObjectHelper::inheritanceDepth(object))
			return false;

		for (size_t i = 0, size = targetDepth + 1; i < size; ++i)
		{
			char const* name = ObjectHelper::name(object, i);
			char const* name2 = ObjectHelper::name(target, i);

			if (name == name2)
				continue;

			if (std::strcmp(name, name2) != 0)
				return false;
		}

		for (size_t i = 0, end = targetDepth + 1; i < end; ++i)
		{
			void** info = reinterpret_cast<void***>(object.vftable__[i + VFTABLE_HEADER_SIZE])[INTERFACE_INFO_INDEX];
			void** info2 = reinterpret_cast<void***>(target.vftable__[i + VFTABLE_HEADER_SIZE])[INTERFACE_INFO_INDEX];

			bcc::uintptr_t count = reinterpret_cast<bcc::uintptr_t>(info[INTERFACE_COUNT_INDEX]);
			bcc::uintptr_t count2 = reinterpret_cast<bcc::uintptr_t>(info2[INTERFACE_COUNT_INDEX]);

			if (count2 > count)
				return false;

			for (size_t j = 0; j < count2; ++j)
			{
				char const* name = reinterpret_cast<char const*>(info[INTERFACE_NAME_INDEX + j + 1]);
				char const* name2 = reinterpret_cast<char const*>(info[INTERFACE_NAME_INDEX + j + 1]);
				
				if (name == name2)
					continue;

				if (std::strcmp(name, name2) != 0)
					return false;
			}
		}

		return true;
	}

	inline bool ObjectHelper::isCompatible(bcc::Object const volatile& object, bcc::Object const& target)
	{
		return ObjectHelper::isCompatible(const_cast<bcc::Object const&>(object), target);
	}
	
	inline bcc::Object* ObjectHelper::cast(bcc::Object& object, bcc::Object const& target)
	{
		bcc::uintptr_t const targetDepth = ObjectHelper::inheritanceDepth(target);

		bcc::Object* itor = &object;
		do
		{
			if (targetDepth > ObjectHelper::inheritanceDepth(*itor))
			{
				itor = ObjectHelper::next(*itor);
				continue;
			}

			enum
			{
				HASH_MATCHED
				, HASH_WEAK_MATCHED
				, HASH_NO_MATCHED
			} matched = HASH_MATCHED;

			for (size_t i = 0, size = targetDepth + 1; i < size; ++i)
			{
				if (ObjectHelper::methodCount(target, i) > ObjectHelper::methodCount(*itor, i))
				{
					matched = HASH_NO_MATCHED;
					break;
				}

				bcc::detail::Hash const& hash = ObjectHelper::hash(*itor, i);
				bcc::detail::Hash const& hash2 = ObjectHelper::hash(target, i);

				if (hash.hash != hash2.hash)
				{
					matched = HASH_NO_MATCHED;
					break;
				}

				if (hash.hash != hash2.hash)
				{
					matched = HASH_WEAK_MATCHED;
					break;
				}
			}

			switch (matched)
			{
			case HASH_MATCHED:
				return itor;

			case HASH_WEAK_MATCHED:
				if (ObjectHelper::isCompatible(*itor, target))
					return itor;
				break;
				
			default:
				break;
			}

			itor = ObjectHelper::next(*itor);
		} while (itor != &object);

		return nullptr;
	}

	inline bcc::Object const* ObjectHelper::cast(bcc::Object const& object, bcc::Object const& target)
	{
		return ObjectHelper::cast(const_cast<bcc::Object&>(object), target);
	}

	inline bcc::Object volatile* ObjectHelper::cast(bcc::Object volatile& object, bcc::Object const& target)
	{
		return ObjectHelper::cast(const_cast<bcc::Object&>(object), target);
	}

	inline bcc::Object const volatile* ObjectHelper::cast(bcc::Object const volatile& object, bcc::Object const& target)
	{
		return ObjectHelper::cast(const_cast<bcc::Object&>(object), target);
	}
	
	template<size_t depth, typename FuntionTypes>
	typename tuple_element<depth, FuntionTypes>::type& ObjectHelper::ownTable(bcc::Object const& object)
	{
		typedef typename tuple_element<depth, FuntionTypes>::type FunctionTable;
		return *reinterpret_cast<FunctionTable*>(object.vftable__[depth + VFTABLE_HEADER_SIZE]);
	}

	template<size_t depth, typename FuntionTypes>
	typename tuple_element<depth, FuntionTypes>::type& ObjectHelper::ownTable(bcc::Object const volatile& object)
	{
		typedef typename tuple_element<depth, FuntionTypes>::type FunctionTable;
		return *reinterpret_cast<FunctionTable*>(object.vftable__[depth + VFTABLE_HEADER_SIZE]);
	}

	template<size_t index, size_t depth, typename FunctionTables>
	typename Function<index, depth, FunctionTables>::type& ObjectHelper::function(bcc::Object const& object)
	{
		typedef typename Function<index, depth, FunctionTables>::type F;
		return reinterpret_cast<F&>(reinterpret_cast<void**>(object.vftable__[depth + VFTABLE_HEADER_SIZE])[index]);
	}
	
	template<size_t index, size_t depth, typename FunctionTables>
	typename Function<index, depth, FunctionTables>::type& ObjectHelper::function(bcc::Object const volatile& object)
	{
		typedef typename Function<index, depth, FunctionTables>::type F;
		return reinterpret_cast<F&>(reinterpret_cast<void**>(object.vftable__[depth + VFTABLE_HEADER_SIZE])[index]);
	}

	template<typename T>
	ProbeDeducer::type<bcc::tuple_size<typename T::BiCOMC_Function_Types__>::value> const& ObjectHelper::vftable(T const& object)
	{
		return *reinterpret_cast<ProbeDeducer::type<bcc::tuple_size<typename T::BiCOMC_Function_Types__>::value> const*>(object.vftable__);
	}
} // namespace detail

	inline Object::Object()
		: vftable__(nullptr)
	{
		static bcc::detail::InterfaceTableHolder<Object> holder(*this);
		bcc::detail::ObjectHelper::setTable(*this, holder.vftable.data());

		struct method_impl
		{
			static bcc::detail::ErrorDetail* BICOMC_CALL destroy(Object const volatile&, bcc::detail::ReturnHelper<void>::mediator&)
			{
				return new bcc::detail::RuntimeError("'destroy' is not overrided");
			}

			static bcc::detail::ErrorDetail* BICOMC_CALL clone(Object const&, bcc::detail::ReturnHelper<Object*>::mediator&)
			{
				return new bcc::detail::RuntimeError("'clone' is not overrided");
			}
		};

		BICOMC_METHOD_CV(destroy, void()) = &method_impl::destroy;
		BICOMC_METHOD_C(clone, Object*()) = &method_impl::clone;
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
		static Target* cast(U* pObject)
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
	T bicomc_cast(U* object)
	{
		return detail::bimcomc_cast_impl<T>::cast(object);
	}
} // namespace bcc

#endif // !def BICOMC_DETAIL_OBJECT_H__
