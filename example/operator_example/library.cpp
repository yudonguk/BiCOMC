
#include <iostream>

#include "Callable.h"

class CallableImpl : public Callable
{
	BICOMC_OVERRIDE(Callable)
		BICOMC_OVER_OPERATOR_ASSIGN(void(bcc::Object* arg));
		BICOMC_OVER_OPERATOR_DEREFERENCE_C(bcc::Object*());

		BICOMC_OVER_OPERATOR_CALL(bcc::Object*());
		BICOMC_OVER_OPERATOR_CALL(bcc::Object*(bcc::Object* arg));

		BICOMC_OVER_OPERATOR_EQUAL_C(bcc::uint8_t(Callable const& rhs));
	BICOMC_OVERRIDE_END()

public:
	CallableImpl()
		: BICOMC_OVERRIDE_INIT(), mpObject()
	{
		std::cout << "CallableImpl()" << std::endl;
	}

	~CallableImpl()
	{
		std::cout << "~CallableImpl()" << std::endl;
	}

public:
	void operator=(bcc::Object* p)
	{
		mpObject = p;
	}
	
	bcc::Object* operator*() const
	{
		return mpObject;
	}

	bcc::Object* operator()()
	{
		return mpObject;
	}

	bcc::Object* operator()(bcc::Object* p)
	{
		return mpObject = p;
	}

	bcc::uint8_t operator==(Callable const& rhs) const
	{
		return static_cast<bcc::uint8_t>(this == &rhs);
	}

private:
	bcc::Object* mpObject;
};

extern "C" BICOMC_EXPORT bcc::Object* CreateObject()
{
	try
	{
		return new CallableImpl();
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "unknown error" << std::endl;
	}

	return nullptr;
}
