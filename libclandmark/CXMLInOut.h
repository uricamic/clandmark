/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Written (W) 2013, 2014, 2015 Michal Uricar
 * Copyright (C) 2013, 2014, 2015 Michal Uricar
 */

#ifndef _CXMLINOUT__H__
#define _CXMLINOUT__H__

#include "rapidxml.hpp"
#include "CTypes.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <stack>
#include <vector>
#include <cstring>

namespace clandmark {

typedef enum {
	FILE_READ,
	FILE_WRITE
} FILE_TYPE;

class XmlNode {

public:

	/**
	 * Default constructor
	 *
	 * @brief XmlNode
	 */
	XmlNode()
	{
		node = 0x0;
	}

	/**
	 * Constructor
	 *
	 * @brief XmlNode
	 * @param node_
	 */
	XmlNode(rapidxml::xml_node<> *node_);

	/**
	 * Destructor
	 */
	~XmlNode();

	/**
	 * @brief empty
	 * @return
	 */
	inline bool empty()
	{
		return node == 0x0 ? true : false;
	}

	/**
	 * @brief getNode
	 * @return
	 */
	inline rapidxml::xml_node<> * getNode()
	{
		return node;
	}

	/**
	 * @brief operator []
	 * @param name
	 * @return
	 */
	XmlNode operator [](const char *name);

	/**
	 * @brief getSet
	 * @param name
	 * @return
	 */
	std::vector<XmlNode> getSet(const char *name);

	/**
	 * @brief readRaw
	 * @param charsize
	 * @return
	 */
	unsigned char * readRaw(int *charsize);

	/**
	 * @brief operator int
	 */
	operator int();

	/**
	 * @brief operator fl_double_t
	 */
	operator fl_double_t();

	/**
	 * @brief operator std::string
	 */
	operator std::string();

private:

	rapidxml::xml_node<> *node;

};

class XmlStorage {

public:

	/**
	 * Constructor
	 *
	 * @brief XmlStorage
	 */
	XmlStorage()
	{
		root = 0x0;
	}

	/**
	 * Constructor
	 *
	 * @brief XmlStorage
	 * @param filename
	 */
	XmlStorage(const std::string filename, FILE_TYPE flag=FILE_READ);

	/**
	 * Destructor
	 *
	 */
	~XmlStorage();

	inline bool empty()
	{
		return root == 0x0 ? true : false;
	}

	/**
	 * @brief operator []
	 * @param name
	 * @return
	 */
	XmlNode operator [](const char *name);

	/**
	 * @brief writeRaw
	 * @param bindata
	 * @param length
	 */
	void writeRaw(const void* bindata, int length);

	/**
	 * @brief release
	 */
	void release(void);

private:

	XmlStorage(const XmlStorage&)
	{}

public:

	typedef enum {
		NAME_EXPECTED,
		VALUE_EXPECTED
	} STORAGE_STATE;

	STORAGE_STATE state;

	rapidxml::xml_node<> *elem;
	std::stack<rapidxml::xml_node<> *> stack;
	rapidxml::xml_document<> doc;

private:

	rapidxml::xml_node<> *root;
	std::string filename;
	char *file_content;

};

/**
 * @brief operator <<
 * @param fs
 * @param content
 * @return
 */
XmlStorage& operator <<(XmlStorage& fs, const std::string &content);

/**
 * @brief operator <<
 * @param fs
 * @param number
 * @return
 */
XmlStorage& operator <<(XmlStorage& fs, const int &number);

/**
 * @brief operator <<
 * @param fs
 * @param number
 * @return
 */
XmlStorage& operator <<(XmlStorage& fs, const fl_double_t &number);

} /* namespace clandmark */

#endif // _CXMLINOUT__H__
