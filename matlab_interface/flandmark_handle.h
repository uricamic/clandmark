/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2012 Oliver Woodford, 2013 Michal Uricar
 * Copyright (C) 2012 Oliver Woodford, 2013 Michal Uricar
 */

#ifndef __FLANDMARK_HANDLE_H__
#define __FLANDMARK_HANDLE_H__

#include "mex.h"

#include <stdint.h>
#include <string>
#include <cstring>
#include <typeinfo>

#define CLASS_HANDLE_SIGNATURE 0xFF00F0A5
template<class base>
class class_handle
{
public:

	/**
	 * Constructor
	 *
	 * @brief class_handle
	 * @param ptr
	 */
	class_handle(base *ptr) : ptr_m(ptr), name_m(typeid(base).name())
	{
		signature_m = CLASS_HANDLE_SIGNATURE;
	}

	/**
	 * Destructor
	 */
	~class_handle(void)
	{
		signature_m = 0;
		delete ptr_m;
	}

	/**
	 * @brief isValid
	 * @return
	 */
	bool isValid(void)
	{
		return ((signature_m == CLASS_HANDLE_SIGNATURE) && !strcmp(name_m.c_str(), typeid(base).name()));
	}

	/**
	 * @brief ptr
	 * @return
	 */
	base *ptr(void)
	{
		return ptr_m;
	}

private:

	uint32_t signature_m;	/**< */
	std::string name_m;		/**< */
	base *ptr_m;			/**< */
};


/**
 * @brief convertPtr2Mat
 * @param ptr
 * @return
 */
template<class base>
inline mxArray *convertPtr2Mat(base *ptr)
{
	mexLock();
	mxArray *out = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
	*((uint64_t *)mxGetData(out)) = reinterpret_cast<uint64_t>(new class_handle<base>(ptr));
	return out;
}

/**
 * @brief convertMat2HandlePtr
 * @param in
 * @return
 */
template<class base>
inline class_handle<base> *convertMat2HandlePtr(const mxArray *in)
{
	if (mxGetNumberOfElements(in) != 1 || mxGetClassID(in) != mxUINT64_CLASS || mxIsComplex(in))
		mexErrMsgTxt("Input must be a real uint64 scalar.");

	class_handle<base> *ptr = reinterpret_cast<class_handle<base> *>(*((uint64_t *)mxGetData(in)));

	if (!ptr->isValid())
		mexErrMsgTxt("Handle not valid.");
	return ptr;
}

/**
 * @brief convertMat2Ptr
 * @param in
 * @return
 */
template<class base>
inline base *convertMat2Ptr(const mxArray *in)
{
	return convertMat2HandlePtr<base>(in)->ptr();
}

/**
 * @brief destroyObject
 * @param in
 */
template<class base>
inline void destroyObject(const mxArray *in)
{
	delete convertMat2HandlePtr<base>(in);
	mexUnlock();
}

#endif // __FLANDMARK_HANDLE_H__
