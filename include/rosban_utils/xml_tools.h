#pragma once

#include "rosban_utils/string_tools.h"
#include "rosban_utils/io_tools.h"

#include <tinyxml.h>

#include <functional>
#include <map>
#include <ostream>
#include <vector>

namespace rosban_utils
{

class XMLParsingError : public std::runtime_error
{
public:
  XMLParsingError(const std::string &msg);
};

namespace xml_tools
{
template <typename T>
void write_generic(const std::string &key, const T &value, std::ostream &out)
{
  out << "<" << key << ">" << value << "</" << key << ">";
}

template <typename T>
void write(const std::string &key, const T &value, std::ostream &out)
{
  write_generic<T>(key, value, out);
}

template <>
void write<char>(const std::string &key, const char &value, std::ostream &out);

template <>
void write<bool>(const std::string &key, const bool &value, std::ostream &out);

template <>
void write<double>(const std::string &key, const double &value, std::ostream &out);

void write(const std::string &key, const double &value, std::ostream &out, int precision);


template<typename T>
void write_vector(const std::string &key, const std::vector<T> &values, std::ostream &out)
{
  out << "<" << key << ">";
  for (const T &value : values)
  {
    write<T>("v", value, out);
  }
  out << "</" << key << ">";
}

std::string get_element(TiXmlNode * node, const std::string & key);

template<typename T>
T read(TiXmlNode * node, const std::string &key)
{
  return str2<T>(get_element(node, key));
}

/// Fill 'value' reference with the one at given key in the provided node
/// If key is not found, no error is thrown and 'value' reference is not modified
/// If key is found but content cannot be parsed, an error is still raised
template<typename T>
void try_read(TiXmlNode *node, const std::string &key, T &value)
{
  try
  {
    value = read<T>(node, key);
  }
  catch (const XMLParsingError &err)
  {
    return;
  }
}

template <typename T>
std::vector<T> read_vector(TiXmlNode * node, const std::string &key)
{
  if(!node) throw XMLParsingError("Null node when trying to get a vector");
  TiXmlNode* values = node->FirstChild(key);
  if (!values) throw XMLParsingError("Could not find node with label '" + key + "' in node: '"
                                     + node->Value() + "'");
  std::vector<T> result;
  for ( TiXmlNode* child = values->FirstChild(); child != NULL; child = child->NextSibling())
  {
    if(!child->FirstChild())
    {
      throw XMLParsingError("Error while reading element string array with label '" + key +
                            "' in node '" + node->Value() + "'");
    }
    result.push_back(str2<T>(child->FirstChild()->Value()));
  }
  return result;
}

/// Do not throw an error if no node with the given key is found
/// Still throws an error on internal parsing issue or if node is NULL
template <typename T>
void try_read_vector(TiXmlNode * node, const std::string &key, std::vector<T> & res)
{
  if(!node) throw XMLParsingError("Null node when trying to get a vector");
  TiXmlNode* values = node->FirstChild(key);
  if (!values) return;

  res = read_vector<T>(node, key);
}

/// Read a map which has the following structure
/// <map_name>
///   <entry>
///     <key>...</key>
///     <value>...</value>
///   </entry>
///   ... (other entries)
/// </map_name>
template <typename T>
std::map<std::string,T> read_map(TiXmlNode * node,
                                 const std::string & map_name,
                                 std::function<T(TiXmlNode*)> builder)
{
  // Check for NULL node
  if(!node) throw XMLParsingError("Null node when trying to get a vector");
  // Get the root node
  TiXmlNode* values = node->FirstChild(map_name);
  if (!values) throw XMLParsingError("Could not find node with label '" + map_name + "' in node: '"
                                   + node->Value() + "'");
  // For each entry node:
  std::map<std::string, T> result;
  for (TiXmlNode * entry_node = values->FirstChild(); entry_node != NULL;
       entry_node = entry_node->NextSibling()) {
    std::string key = read<std::string>(entry_node, "key");
    TiXmlNode * value_node = entry_node->FirstChild("val");
    if (!value_node) {
      throw XMLParsingError("Error while reading entry, missing val in entry_node '"
                            + std::string(entry_node->Value()) + "'");
    }
    if (result.count(key) > 0) {
      throw XMLParsingError("Key '" + key + "' found more than once in '" + node->Value() + "'");
    }
    result[key] = builder(value_node);
  }
  return result;
}

///@param stream
///@return a non null node on the second sibling of the stream
///and throw an exception if no such node
TiXmlDocument * string_to_doc(const std::string &xml_string);

TiXmlDocument * file_to_doc(const std::string &path);

void doc_to_file(const std::string &path, TiXmlDocument * doc);

///The xml stream is checked then wrote to a file
void string_to_file(const std::string &path, const std::string &xml_string);

}//end of rosban_utils::xml_tools

}
