
#include <iostream>

#include "Simple.h"

class SimpleImpl : public Simple
{
	BICOMC_OVERRIDE(Simple)
		BICOMC_OVER_METHOD_C(hello, void());
		BICOMC_OVER_METHOD_C(number, bcc::int32_t());
		BICOMC_OVER_METHOD(number, bcc::int32_t(bcc::int32_t));
	BICOMC_OVERRIDE_END()

public:
	SimpleImpl()
		: BICOMC_OVERRIDE_INIT(), mNumber(__LINE__)
	{
		std::cout << "SimpleImpl()" << std::endl;
	}

	~SimpleImpl()
	{
		std::cout << "~SimpleImpl()" << std::endl;
	}

public:
	void hello() const
	{
		std::cout << "Hello world" << std::endl;
	}

	bcc::int32_t number() const
	{
		return mNumber;
	}

	bcc::int32_t number(bcc::int32_t n)
	{
		return mNumber = n;
	}

private:
	bcc::int32_t mNumber;
};

extern "C" BICOMC_EXPORT bcc::Object* CreateObject()
{
	try
	{
		return new SimpleImpl();
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
