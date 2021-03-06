#ifndef __nessDB_DEBUG_H
#define __nessDB_DEBUG_H

//#include "internal.h"
#include <stdio.h>
#include <stdarg.h>

enum DEBUG_LEVEL {
	LEVEL_DEBUG = 0,
	LEVEL_INFO,
	LEVEL_WARNING,
	LEVEL_ERROR};

void __debug(char *file, int line,
		enum DEBUG_LEVEL level, const char *fmt, ...);
#ifdef WIN32
	#ifdef INFO
		#define __INFO(format, ...) \
			do { __debug(__FILE__, __LINE__, \
					LEVEL_INFO, format, __VA_ARGS__); \
			} while (0)
	#else
		#define __INFO(format, args, ...) do {} while (0)
	#endif

	#if defined(DEBUG) || defined(INFO)
		#define __DEBUG(format, ...) \
			do { __debug(__FILE__, __LINE__, \
					LEVEL_DEBUG, \
					format, __VA_ARGS__); \
			} while (0)
	#else
		#define __DEBUG(format, args, ...) do {} while (0)
	#endif

	#if defined(WARN) || defined(INFO)
		#define __WARN(format, ...) \
			do { __debug(__FILE__, __LINE__, \
					LEVEL_WARNING,\
					format, __VA_ARGS__); \
			} while (0)
	#else
		#define __WARN(format, args, ...) do {} while (0)
	#endif

	#if defined(ERROR) || defined(DEBUG) || defined(WARN) || defined(INFO)
	#define __ERROR(format, ...)\
		do { __debug(__FILE__, __LINE__,\
				LEVEL_ERROR,\
				format, __VA_ARGS__); \
		} while (0)
	#else
		#define __ERROR(format, args, ...) do {} while (0)
	#endif

	#define __PANIC(format, ...) \
		do { __debug(__FILE__, __LINE__, \
				LEVEL_ERROR,\
				format, __VA_ARGS__); abort(); exit(1); \
		} while (0)

	#endif
#else	
	//linux environment
	#ifdef INFO
	#define __INFO(format, args...) \
		do { __debug(__FILE__, __LINE__, \
		LEVEL_INFO, format, ##args); \
		} while (0)
	#else
	#define __INFO(format, args...) do {} while (0)
	#endif

	#if defined(DEBUG) || defined(INFO)
	#define __DEBUG(format, args...) \
		do { __debug(__FILE__, __LINE__, \
		LEVEL_DEBUG, \
		format, ##args); \
		} while (0)
	#else
	#define __DEBUG(format, args...) do {} while (0)
	#endif

	#if defined(WARN) || defined(INFO)
	#define __WARN(format, args...) \
		do { __debug(__FILE__, __LINE__, \
		LEVEL_WARNING,\
		format, ##args); \
		} while (0)
	#else
	#define __WARN(format, args...) do {} while (0)
	#endif

	#if defined(ERROR) || defined(DEBUG) || defined(WARN) || defined(INFO)
	#define __ERROR(format, args...)\
		do { __debug(__FILE__, __LINE__,\
		LEVEL_ERROR,\
		format, ##args); \
		} while (0)
	#else
	#define __ERROR(format, args...) do {} while (0)
	#endif

	#define __PANIC(format, args...) \
		do { __debug(__FILE__, __LINE__, \
		LEVEL_ERROR,\
		format, ##args); abort(); exit(1); \
		} while (0)
#endif