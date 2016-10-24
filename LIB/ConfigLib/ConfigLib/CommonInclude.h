#ifndef _COMMONINCLUDE_H_
#define _COMMONINCLUDE_H_

#ifdef CONFIGLIB_EXPORTS
#define CONFIGLIB_API __declspec(dllexport)
#else
#define CONFIGLIB_API __declspec(dllimport)
#endif

#pragma warning( disable : 4290 )

//if higher or equal than visual 2012
#if (_MSC_VER>=1700)
	#define	_NULLPTR_	nullptr
#else
	#define	_NULLPTR_	NULL
#endif

#include <string>
#include <stdexcept>
#include <vector>
#include <cassert>

CONFIGLIB_API enum {
   ERROR_ARGS = 1,
   ERROR_XERCES_INIT,
   ERROR_PARSE,
   ERROR_EMPTY_DOCUMENT
};

class XercesImpl;

#endif