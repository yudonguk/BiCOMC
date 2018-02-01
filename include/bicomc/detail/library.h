/*
 * Copyright 2018 Kangwon National University and Contributors
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

#ifndef BICOMC_DETAIL_LIBRARY_H__
#define BICOMC_DETAIL_LIBRARY_H__

#include "config.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#if defined(_WIN32)
#	include <Windows.h>

#else
#	include <dlfcn.h>

#endif // defined(_WIN32)

namespace bcc
{
namespace detail
{
#if defined(_WIN32)
	struct DlHelper
	{
		typedef HMODULE Handle;

		static Handle open(char const* path)
		{
			Handle result = LoadLibraryA(path);
			if (!result) throwIfError();
			return result;
		}

		static void* symbol(Handle handle, char const* name)
		{
			void* result = reinterpret_cast<void*>(GetProcAddress(handle, name));
			if (!result) throwIfError();
			return result;
		}

		static void close(Handle handle)
		{
			if (!FreeLibrary(handle))
				throwIfError();
		}

		static void throwIfError()
		{
			DWORD error = GetLastError();
			if (error == ERROR_SUCCESS)
				return;

			std::string message;
			{
				LPSTR buffer = NULL;
				FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
					, NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
					, reinterpret_cast<LPSTR>(&buffer), 0, NULL
				);

				try
				{
					if (buffer)
						message = buffer;
				}
				catch (...)
				{}

				LocalFree(buffer);
			}

			if (!message.empty())
				throw std::runtime_error(message);

			try
			{
				std::stringstream stream;
				stream << "unknown error from code 0x" << std::hex << error;
				message = stream.str();
			}
			catch (...)
			{
				throw std::runtime_error("unknown error");
			}

			throw std::runtime_error(message);
		}
	};

#else
	struct DlHelper
	{
		typedef void* Handle;

		static Handle open(char const* path)
		{
			Handle result = dlopen(path, RTLD_LAZY);
			if (!result) throwIfError();
			return result;
		}

		static void* symbol(Handle handle, char const* name)
		{
			void* result = dlsym(handle, name);
			if (!result) throwIfError();
			return result;
		}

		static void close(Handle handle)
		{
			if (dlclose(handle) != 0)
				throwIfError();
		}

		static void throwIfError()
		{
			if (char const* message = dlerror())
				throw std::runtime_error(message);
		}
	};

#endif // defined(_WIN32)

	class Library
	{
	public:
		Library() : mHandle() {}

		Library(std::string const& path) : mHandle() { open(path); }

		Library(std::string const& name, std::string const& dir) : mHandle() { open(name, dir); }

		explicit Library(DlHelper::Handle handle) : mHandle(handle) {}

		~Library()
		{
			try { close(); } catch (...) {}
		}

	private:
		Library(Library const&) BICOMC_DELETE;
		Library& operator=(Library const&) BICOMC_DELETE;

	public:
		bool operator!() const BICOMC_NOEXCEPT
		{
			return !mHandle;
		}

		typedef void(Library::*explicit_bool_operator_type)(void*) const;
		void explicit_bool_operator(void*) const {}
		operator explicit_bool_operator_type() const BICOMC_NOEXCEPT
		{
			return !mHandle ? NULL : &Library::explicit_bool_operator;
		}

	public:
		void swap(Library& rhs) BICOMC_NOEXCEPT
		{
			if (this == &rhs) return;
			std::swap(mHandle, rhs.mHandle);
		}

	public:
		void open(std::string const& path)
		{
			std::string name = trim(path);
			std::string base = parent(name);
			name = name.substr(base.size());
			open(name, base);
		}

		void open(std::string const& name, std::string const& dir)
		{
#if defined(_WIN32)
			char const* prefix = "";
			char const* extension = ".dll";
#elif defined(__APPLE__)
			char const* prefix = "lib";
			char const* extension = ".dylib";
#else
			char const* prefix = "lib";
			char const* extension = ".so";
#endif // defined(_WIN32)

			DlHelper::Handle handle = DlHelper::Handle();

			try
			{
				handle = DlHelper::open(concatenate(dir, name).c_str());
			}
			catch (...)
			{
				std::string candidates[] = {
					concatenate(dir, prefix + name + extension)
					, concatenate(dir, name + extension)
				};

				std::string* itor = &candidates[0];
				std::string* end = itor + sizeof(candidates) / sizeof(candidates[0]);

				for (; itor != end; ++itor)
				{
					try
					{
						normalize(*itor);
						handle = DlHelper::open(itor->c_str());
						break;
					}
					catch (...)
					{}
				}

				if (!handle)
					throw;
			}

			try
			{
				close();
				mHandle = handle;
			}
			catch (...)
			{
				try { DlHelper::close(handle); } catch (...) {}
				throw;
			}
		}

		template<typename T>
		T symbol(char const* name) const
		{
			if (!mHandle)
				throw std::invalid_argument("Dynamic library did not open.");
			return reinterpret_cast<T>(DlHelper::symbol(mHandle, name));
		}

		void close()
		{
			if (!mHandle) return;
			DlHelper::close(mHandle);
			mHandle = DlHelper::Handle();
		}

	public:
		static void normalize(std::string& path) BICOMC_NOEXCEPT
		{
#if defined(_WIN32)
				char const* seps = "\\/";
#else
				char const* seps = "/";
#endif // defined(_WIN32)

			std::string::size_type pos = path.find_first_of(seps);

			while (pos != std::string::npos)
			{
#if defined(_WIN32)
				path[pos] = '\\';
#else
				path[pos] = '/';
#endif // defined(_WIN32)
				pos = path.find_first_of(seps, pos + 1);
			}
		}

		static std::string parent(std::string const& path)
		{
			std::string::size_type pos = path.find_last_of(
#if defined(_WIN32)
				"\\/"
#else
				"/"
#endif // defined(_WIN32)
			);
			return pos == std::string::npos ? std::string() : path.substr(0, pos);
		}

		static std::string concatenate(std::string const& base, std::string const& child)
		{
			std::string result;
			result.reserve(base.size() + child.size() + 1);

			if (!base.empty())
			{
				result.append(base);
#if defined(_WIN32)
				if (*base.rbegin() != '\\' && *base.rbegin() != '/')
					result.append(1, '\\');
#else
				if (*base.rbegin() != '/')
					result.append(1, '/');
#endif // defined(_WIN32)
			}

			result.append(child);
			return result;
		}

		static std::string trim(std::string const& str)
		{
			char const* tokens = "\t\n\v\f\r ";
			std::string::size_type start = str.find_first_not_of(tokens);
			std::string::size_type last = str.find_last_not_of(tokens);
			if (start == last) return std::string();
			else return str.substr(start, last - start + 1);
		}

	private:
		DlHelper::Handle mHandle;
	};

} // namespace detail
} //namespace bcc

#endif // !def BICOMC_DETAIL_LIBRARY_H__
