#include "CXMLInOut.h"

#include "base64.h"

#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

using namespace clandmark;

XmlNode::XmlNode(rapidxml::xml_node<> *node_)
{
	node = node_;
}

XmlNode::~XmlNode()
{
	;
}

XmlNode XmlNode::operator [](const char *name)
{
	if (node)
		return XmlNode(node->first_node(name));
	else
		return XmlNode();
}

std::vector<XmlNode> XmlNode::getSet(const char* name)
{
	std::vector<XmlNode> vector;
	rapidxml::xml_node<> *current_node;

	if (node)
	{
		if (node->first_node(name))
		{
			current_node = node->first_node(name);
			//vector.push_back(XmlNode(node->first_node(name)));

			while (current_node->next_sibling())
			{
				vector.push_back(XmlNode(current_node));
				current_node = current_node->next_sibling();
			}

			vector.push_back(XmlNode(current_node));

		}
	}

	return vector;
}

unsigned char * XmlNode::readRaw(int *charsize)
{
	if (node)
	{
		if (node->value())
		{
			int recoveredLen;
			unsigned char* recoveredData;
			recoveredData = unbase64(node->value(), node->value_size(), &recoveredLen);
			*charsize = recoveredLen;
			return recoveredData;
		}
	}

	*charsize = 0;

	return 0x0;
}

XmlNode::operator int()
{
	int value = 0x0;
	std::stringstream ss(node->value());

	if ((ss >> value).fail())
	{
		;
	} else {
		return value;
	}

	return value;
}

XmlNode::operator fl_double_t()
{
	fl_double_t value = 0x0;

	std::stringstream ss(node->value());
	if ((ss >> value).fail())
	{
		;
	} else {
		return value;
	}

	return value;
}

XmlNode::operator std::string()
{
	return node->value();
}

XmlStorage::XmlStorage(const std::string filename, FILE_TYPE flag)
{
	this->filename = filename;
	this->file_content = 0x0;

	if (flag==FILE_READ)
	{
		try
		{
			rapidxml::file<> xmlFile(filename.c_str());

			file_content = new char[xmlFile.size()];
			memcpy(file_content, xmlFile.data(), xmlFile.size());

			doc.parse<0>(file_content);
			root = doc.first_node();
		} catch (std::runtime_error e) {
			std::cerr << e.what() << std::endl;
			root = 0x0;
			return;
		}
	} else if (flag==FILE_WRITE) {
		// declaration
		rapidxml::xml_node<> *declaration = doc.allocate_node(rapidxml::node_declaration);
		declaration->append_attribute(doc.allocate_attribute("version", "1.0"));
		declaration->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
		doc.append_node(declaration);

		// create root node
		root = doc.allocate_node(rapidxml::node_element, "clandmark");
		doc.append_node(root);

		state = NAME_EXPECTED;
		stack.push(root);
	} else {
		root = 0x0;
		return;
	}
}

XmlStorage::~XmlStorage()
{
	if (file_content)
		delete [] file_content;
}

XmlNode XmlStorage::operator [](const char *name)
{
	return XmlNode(root->first_node(name));
}

void XmlStorage::writeRaw(const void *bindata, int length)
{
	char *base64Ascii;
	int base64AsciiLen;
	base64Ascii = base64(bindata, length, &base64AsciiLen);

	if (state == VALUE_EXPECTED)
	{
		elem->append_node(doc.allocate_node(rapidxml::node_data, 0x0, doc.allocate_string(base64Ascii)));
		state = NAME_EXPECTED;
	} else {
		; // TODO e.g. create implicit node called binary
	}
}

void XmlStorage::release()
{
	std::string xmlstring;
	rapidxml::print(std::back_inserter(xmlstring), doc);
	std::ofstream file;
	file.open(filename.c_str());
	file << xmlstring;
	file.close();
}

XmlStorage& clandmark::operator <<(XmlStorage& fs, const std::string &content)
{
	if (content.compare("[") == 0)
	{
		fs.stack.push(fs.elem);
		fs.state = fs.NAME_EXPECTED;
	} else if (content.compare("]") == 0) {
		fs.stack.pop();
		fs.state = fs.NAME_EXPECTED;
	} else if (content.compare("{") == 0) {
		fs.stack.push(fs.elem);
		fs.state = fs.NAME_EXPECTED;
	} else if (content.compare("}") == 0) {
		fs.stack.pop();
		fs.state = fs.NAME_EXPECTED;
	} else if (fs.state == fs.NAME_EXPECTED) {
		fs.state = fs.VALUE_EXPECTED;
		fs.elem = fs.stack.top();
		fs.elem->append_node(fs.doc.allocate_node(rapidxml::node_element, fs.doc.allocate_string(content.c_str())));
		fs.elem = fs.elem->last_node();
	} else if (fs.state == fs.VALUE_EXPECTED) {
		fs.state = fs.NAME_EXPECTED;
		fs.elem->append_node(fs.doc.allocate_node(rapidxml::node_data, 0x0, fs.doc.allocate_string(content.c_str())));
	}

	return fs;
}

XmlStorage& clandmark::operator <<(XmlStorage& fs, const int &number)
{
	std::ostringstream convert;
	convert << number;

	if (fs.state == fs.VALUE_EXPECTED)
	{
		fs.state = fs.NAME_EXPECTED;
		fs.elem->append_node(fs.doc.allocate_node(rapidxml::node_data, 0x0, fs.doc.allocate_string(convert.str().c_str())));
	}

	return fs;
}

XmlStorage& clandmark::operator <<(XmlStorage& fs, const fl_double_t &number)
{
	std::ostringstream convert;
	convert << number;

	if (fs.state == fs.VALUE_EXPECTED)
	{
		fs.state = fs.NAME_EXPECTED;
		fs.elem->append_node(fs.doc.allocate_node(rapidxml::node_data, 0x0, fs.doc.allocate_string(convert.str().c_str())));
	}

	return fs;
}

