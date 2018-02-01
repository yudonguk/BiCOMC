
#include <cassert>

#include <multiple.h>

#include <bicomc/detail/library.h>

int main(int argc, char* argv[])
{
	using bcc::detail::Library;
	Library module;
	if (argc == 2)
		module.open(argv[1]);
	else
		module.open("library-multiple", Library::parent(argv[0]));

	assert(module != NULL);

	bcc::Object*(*create)() = module.symbol<bcc::Object*(*)()>("CreateObject");
	assert(create != NULL);

	bcc::Object* pObject = create();
	assert(pObject);

	Simple* pSimple = bicomc_cast<Simple*>(pObject);
	assert(pSimple);

	pSimple->hello();

	std::cout << "pSimple->number() : " << pSimple->number() << std::endl;
	std::cout << "pSimple->number(__LINE__) : " << pSimple->number(__LINE__) << std::endl;
	std::cout << "pSimple->number() : " << pSimple->number() << std::endl;

	OverlapHello* pOverlap = bicomc_cast<OverlapHello*>(pSimple);
	assert(pOverlap);

	pOverlap->hello();

	pObject->destroy();

	return 0;
}
