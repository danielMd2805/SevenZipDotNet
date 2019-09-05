#pragma once

//#undef EXTERN_C


#ifdef SEVENZIPCPP_EXPORTS
#define SEVENZIPCPP_API __declspec(dllexport)
#else
#define SEVENZIPCPP_API __declspec(dllimport)
#endif
