#include <Windows.h>

#include <cassert>

#include <Callable.h>

int main()
{
	HMODULE module = LoadLibraryA("library");
	assert(module != nullptr);

	bcc::Object*(*create)() = reinterpret_cast<bcc::Object*(*)()>(GetProcAddress(module, "CreateObject"));
	assert(create != nullptr);

	bcc::Object* pObject = create();
	assert(pObject);

	Callable& callable = bicomc_cast<Callable&>(*pObject);
	std::cout << "pObject : " << pObject << std::endl;
	
	std::cout << "callable() : ";
	std::cout << callable() << std::endl;

	std::cout << "callable = pObject" << std::endl;
	callable = pObject;

	std::cout << "callable() : ";
	std::cout << callable() << std::endl;
	
	std::cout << "callable = nullptr" << std::endl;
	callable = nullptr;

	std::cout << "callable() : ";
	std::cout << callable() << std::endl;

	std::cout << "callable(pObject) : ";
	std::cout << callable(pObject) << std::endl;

	std::cout << "callable() : ";
	std::cout << callable() << std::endl;
	
	std::cout << "*callable : ";
	std::cout << *callable << std::endl;
	
	std::cout << "callable == callable : ";
	if (callable == callable)
		std::cout << "Callable is same." << std::endl;
	else
		std::cout << "Callable is not same." << std::endl;
	
	pObject->destroy();
	
	FreeLibrary(module);

	return 0;
}
