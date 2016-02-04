#include "rosban_utils/serializable.h"

namespace rosban_utils
{

Serializable::Serializable()
{
}

Serializable::~Serializable()
{
}

void Serializable::load_file()
{
  load_file(class_name() + ".xml");
}

void Serializable::load_file(const std::string &path)
{
  TiXmlDocument * doc = xml_tools::file_to_doc(path);
  if(!doc) throw std::runtime_error("Failed to load file " + path);

  TiXmlNode * node = doc->FirstChild(class_name().c_str());
  if(!node) throw std::runtime_error("Failed to find node with tag "
                                     + class_name()+ " in xml file " + path);
  try
  {
    from_xml(node);
  }
  catch (const std::runtime_error &exc)
  {
    delete doc;
    throw exc;
  }
}

//serializes to an xml stream including class name
std::string Serializable::to_xml_stream() const
{
  return "<"+ class_name() + ">" + to_xml() + "</"+ class_name() + ">";
}

void Serializable::save_file()
{
  save_file(class_name() + ".xml");
}

void Serializable::save_file(const std::string &path)
{
  std::string result = to_xml();
  std::string cn = class_name();
  std::ostringstream oss;
  oss << "<" << cn << ">" << result << "</" << cn << ">";

  xml_tools::string_to_file(path, oss.str());
}

void Serializable::from_xml(const std::string &xml_string)
{
	TiXmlDocument * doc = 0;
	TiXmlNode * node = 0;

	try
	{
    doc = xml_tools::string_to_doc(xml_string);
    if(!doc) throw std::runtime_error("Failed to parse xml stream:\n\t" + xml_string);

    node = doc->FirstChild();
    if(!node) throw std::runtime_error("Failed to find node in xml stream\n\t" + xml_string);

    from_xml(node);
	}
	catch (const std::runtime_error & exc)
	{
    delete doc;
    throw std::runtime_error("Failed to extract data from xml stream:\n\t"
                             + std::string(exc.what()));
	}

}

void Serializable::pretty_print() const
{
  TiXmlDocument * doc = xml_tools::string_to_doc(to_xml_stream());
  if(!doc)
  {
    std::cout << " Failed to turn object to xml doc" << std::endl;
    return;
  }

  std::cout << std::endl;
  doc->Print();
  std::cout << std::endl;
  delete doc;
}

std::ostream & operator<< (std::ostream & os, Serializable & obj)
{
  os << obj.to_xml();
  return os;
}

}
