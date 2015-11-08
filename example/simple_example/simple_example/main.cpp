#include <Windows.h>

#include <cassert>

#include <bicomc/object.h>

#include <Simple.h>

int main()
{
	HMODULE module = LoadLibraryA("library");
	assert(module != nullptr);

	bcc::Object*(*create)() = reinterpret_cast<bcc::Object*(*)()>(GetProcAddress(module, "CreateObject"));
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
	
	FreeLibrary(module);

	return 0;
}
