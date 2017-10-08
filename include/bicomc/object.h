/*
 * Copyright 2015 Kangwon National University
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
