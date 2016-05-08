#ifndef BICOMC_OBJECT_H__
#define BICOMC_OBJECT_H__

#include "detail/config.h"
#include "detail/object_fwd.h"

#include "error_code.h"

namespace bcc
{
	class Object
	{
	protected:
		Object();
		~Object();

	private:
		Object(Object const& rhs) BICOMC_DELETE; // for non-copyable
		Object& operator=(Object const& rhs) BICOMC_DELETE; // for non-copyable

	protected:
		BICOMC_INTERFACE_BASE_CODE(Object);

	public:
		BICOMC_DECL_METHOD_0_CV(destroy, void());
		BICOMC_DECL_METHOD_0_C(clone, Object*());

	protected:
		BICOMC_INTERFACE_FUNCTION_TABLE(Object);

	private:
		void** const vftable__;
	};

	static_assert(sizeof(Object) == sizeof(void*), "'Object' size must equal 'void*' size.");

} // namespace bcc

BICOMC_SIGNATURE(bcc::Object);

#include "detail/error_code.h"
#include "detail/object.h"

#endif // !def BICOMC_OBJECT_H__
