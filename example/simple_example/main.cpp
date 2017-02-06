
#include <cassert>

#include <Simple.h>

namespace helper {
	void* loadLibrary(char const* path);
	void closeLibrary(void* handle);
	void* getSymbol(void* handle, char const* name);
} // namespace helper

int main()
{
	void* module = helper::loadLibrary("library-simple");
	assert(module != nullptr);

	bcc::Object*(*create)() = reinterpret_cast<bcc::Object*(*)()>(helper::getSymbol(module, "CreateObject"));
	assert(create != nullptr);

	bcc::Object* pObject = create();
	assert(pObject);

	Simple* pSimple = bicomc_cast<Simple*>(pObject);
	assert(pSimple);

	pSimple->hello();

	std::cout << "pSimple->number() : " << pSimple->number() << std::endl;
	std::cout << "pSimple->number(__LINE__) : " << pSimple->number(__LINE__) << std::endl;
	std::cout << "pSimple->number() : " << pSimple->number() << std::endl;

	pObject->destroy();
	
	helper::closeLibrary(module);

	return 0;
}
