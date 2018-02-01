
#include <cassert>

#include <Callable.h>

#include <bicomc/detail/library.h>

int main(int argc, char* argv[])
{
	using bcc::detail::Library;
	Library module;
	if (argc == 2)
		module.open(argv[1]);
	else
		module.open("library-callable", Library::parent(argv[0]));

	assert(module != NULL);

	bcc::Object*(*create)() = module.symbol<bcc::Object*(*)()>("CreateObject");
	assert(create != NULL);

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
	callable = NULL;

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

	return 0;
}
