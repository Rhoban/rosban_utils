#include "rosban_utils/xml_tools.h"

namespace rosban_utils
{

XMLParsingError::XMLParsingError(const std::string &msg)
  : std::runtime_error("XMLParsingError: " + msg)
{
}

namespace xml_tools
{

template <>
void write<char>(const std::string &key, const char &value, std::ostream &out)
{
  write_generic<int>(key, (int)value, out);
}

template <>
void write<bool>(const std::string &key, const bool &value, std::ostream &out)
{
  write_generic<std::string>(key, value ? "true" : "false", out);
}

template <>
void write<double>(const std::string &key, const double &value, std::ostream &out)
{
  out.precision(10);
  write_generic<std::string>(key, value ? "true" : "false", out);
}


std::string get_element(TiXmlNode * node, const std::string & key)
{
  if (!node) throw XMLParsingError("Get element on null node");
  TiXmlNode* father = node->FirstChild(key.c_str());
  if (father)
  {
    TiXmlNode* child = father->FirstChild();
    if (child) return std::string(child->Value());
  }
  throw XMLParsingError("Could not get value for label '" + key + "' in node '"
                        + node->Value() + "'");
}
TiXmlDocument * string_to_doc(const std::string &xml_string)
{
  TiXmlDocument * doc = new TiXmlDocument();
  doc->Parse(xml_string.c_str());
  if(doc->Error()){
    std::ostringstream oss;
    oss << "failed to parse xml stream: '" << doc->ErrorDesc() << "'\n"
        << xml_string;
    throw std::runtime_error(oss.str());
  }
  
  TiXmlNode* firstnode = doc->FirstChild();
  if(!firstnode)
    throw std::runtime_error("no child in the stream.");
  
  //we remove the declaration
  if(firstnode->Type() == TiXmlNode::TINYXML_DECLARATION)
    doc->RemoveChild(firstnode);

  return doc;
}

TiXmlDocument * file_to_doc(const std::string &path)
{
  std::string file_content = file2string(path);
  return string_to_doc(file_content);
}

void doc_to_file(const std::string &path, TiXmlDocument * doc)
{
  if(!doc) throw std::runtime_error("xml_tools: Null document to file");

  bool ok =  doc->SaveFile(path.c_str());

  if(!ok) throw std::runtime_error("Could not save xml to file '" + path + "'");
}

/*!
 * The xml stream is checked then wrote to a file
 */
void string_to_file(const std::string &path, const std::string &xml_string)
{
  TiXmlDocument * doc = string_to_doc(xml_string);
  if(!doc) throw std::runtime_error("Could not parse stream");

  TiXmlDeclaration decl("1.0", "" , "");
  doc->InsertBeforeChild(doc->FirstChild(), decl);

  try
  {
    doc_to_file(path, doc);
  }
  catch(const std::string & st)
  {
    delete doc;
    throw st;
  }
  delete doc;
}

}//End of rosban_utils::xml_tools
}
