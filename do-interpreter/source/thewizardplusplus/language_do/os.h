#ifndef WIZARD_BASIC_3_OS_HEADER
#define WIZARD_BASIC_3_OS_HEADER

// http://sourceforge.net/p/predef/wiki/OperatingSystems/
#if \
	defined(__gnu_linux__) \
	|| defined(__linux__) \
	|| defined(linux) \
	|| defined(__linux)
	#define DO_OS_LINUX
#elif \
	defined(_WIN32) \
	|| defined(_WIN64) \
	|| defined(__WIN32__) \
	|| defined(__TOS_WIN__) \
	|| defined(__WINDOWS__)
	#define DO_OS_WINDOWS
#else
	#error: an attempt to compile for an unsupported platform; \
	supports Linux and Windows x86 and x86-64.
#endif
#endif
