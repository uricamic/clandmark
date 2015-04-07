#ifndef _MSVC_COMPAT__
#define _MSVC_COMPAT__
#ifdef _MSC_VER

typedef unsigned char uint8_t;
//typedef char int8_t;
typedef unsigned __int16 uint16_t;
typedef __int16 int16_t;
typedef unsigned __int32 uint32_t;
typedef __int32 int32_t;
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;

/*
#include <ctime>

//#include "stdafx.h"
#include <time.h>
#include <windows.h>
#include <cmath>

const __int64 DELTA_EPOCH_IN_MICROSECS= 11644473600000000;

struct timezone
{
	__int32  tz_minuteswest;	// minutes W of Greenwich //
	bool  tz_dsttime;			// type of dst correction //
};

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	__int64 tmpres = 0;
	TIME_ZONE_INFORMATION tz_winapi;
	int rez=0;

	ZeroMemory(&ft,sizeof(ft));
	ZeroMemory(&tz_winapi,sizeof(tz_winapi));

	GetSystemTimeAsFileTime(&ft);

	tmpres = ft.dwHighDateTime;
	tmpres <<= 32;
	tmpres |= ft.dwLowDateTime;

	//converting file time to unix epoch//
	tmpres /= 10;  //convert into microseconds//
	tmpres -= DELTA_EPOCH_IN_MICROSECS;
	tv->tv_sec = (__int32)(tmpres*0.000001);
	tv->tv_usec =(tmpres%1000000);


	//_tzset(),don't work properly, so we use GetTimeZoneInformation
	rez=GetTimeZoneInformation(&tz_winapi);
	tz->tz_dsttime=(rez==2)?true:false;
	tz->tz_minuteswest = tz_winapi.Bias + ((rez==2)?tz_winapi.DaylightBias:0);

	return 0;
}

double round(double d)
{
	return floor(d + 0.5);
}
*/

#else
	#include <stdint.h>
	#include <sys/time.h>
#endif

#include <cmath>

namespace clandmark {
// index row-order matrices
#define INDEX(ROW, COL, NUM_ROWS) ((COL)*(NUM_ROWS)+(ROW))
#define CLANDMARK_MIN(A,B) ((A) > (B) ? (B) : (A))
#define CLANDMARK_MAX(A,B) ((A) > (B) ? (A) : (B))
#define CLANDMARK_ROW(IDX, ROWS) ((IDX) % (ROWS))
#define CLANDMARK_COL(IDX, ROWS) ((IDX) / (ROWS))

//#define RADTODEG 180.0/M_PI;
#define RADTODEG 180.0/3.14159265358979323846

template <class T>
inline T square(const T &x) { return x*x; }
}

#endif
