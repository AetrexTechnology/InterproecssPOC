#pragma once


#ifdef VACLIENTLIB_EXPORTS
#define VACLIENTLIB_API __declspec(dllexport)
#else
#define VACLIENTLIB_API __declspec(dllimport)
#endif


extern "C" VACLIENTLIB_API bool start();
extern "C" VACLIENTLIB_API bool stop();
