
#include <cassert>

#include <Callable.h>

namespace helper {
	void* loadLibrary(char const* path);
	void closeLibrary(void* handle);
	void* getSymbol(void* handle, char const* name);
} // namespace helper

int main()
{
	void* module = helper::loadLibrary("library-callable");
	assert(module != nullptr);

	bcc::Object*(*create)() = reinterpret_cast<bcc::Object*(*)()>(helper::getSymbol(module, "CreateObject"));
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
	
	helper::closeLibrary(module);

	return 0;
}
