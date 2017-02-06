
namespace helper {
	void* loadLibrary(char const* path);
	void closeLibrary(void* handle);
	void* getSymbol(void* handle, char const* name);
} // namespace helper

#ifdef _WIN32
#include <Windows.h>

void* helper::loadLibrary(char const* path)
{
	return ::LoadLibraryA(path);
}

void helper::closeLibrary(void* handle)
{
	::FreeLibrary(reinterpret_cast<HMODULE>(handle));
}

void* helper::getSymbol(void* handle, char const* name)
{
	return ::GetProcAddress(reinterpret_cast<HMODULE>(handle), name);
}

#else
#include <dlfcn.h>

void* helper::loadLibrary(char const* path)
{
	return ::dlopen(path, RTLD_LAZY);
}

void helper::closeLibrary(void* handle)
{
	::dlclose(handle);
}

void* helper::getSymbol(void* handle, char const* name)
{
	return ::dlsym(handle, name);
}

#endif
