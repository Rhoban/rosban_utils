#pragma once

#include "rosban_utils/xml_tools.h"

namespace rosban_utils
{

///

#define XML_WRITE_SERIALIZABLE(result, truc) {                          \
    result << "<" << # truc << ">" << truc.to_xml() << "</" << # truc << ">"; \
  }

#define XML_READ_SERIALIZABLE(node, truc)           \
  {                                                 \
    try{                                            \
      TiXmlNode * child;                            \
      if ((child = node->FirstChild(# truc)) != 0 ) \
        truc.from_xml(child);                       \
    }                                               \
    catch (...){}                                   \
    }

class Serializable
{
public:
  virtual std::string class_name() const = 0;
  
  /// loads the object from the default file
  /// the default filename is the name of the class + .xml
  virtual void load_file();

  /// loads the object from a given file
  void load_file(const std::string &filename);

  /// serializes and saves to a file using default filename
  void save_file();

  /// serializes and saves to a file using given filename
  void save_file(const std::string &filename);

  /// deserializes from an xml stream
  virtual void from_xml(const std::string &xml_stream);

  /// deserializes from an xml node
  virtual void from_xml(TiXmlNode *node) {
    (void) node;//Suppress unused warning
    throw std::runtime_error("from_xml not implemented");
  };

  /// serializes to an xml stream excluding class name
  virtual std::string to_xml() const { throw std::runtime_error("to_xml not implemented");};


  /// serializes to an xml stream including class name
  std::string to_xml_stream() const;

  /*! pretty print */
  void pretty_print() const;

protected:
  Serializable();

  virtual ~Serializable();
};

/*! not so pretty print */
std::ostream & operator<< (std::ostream & stream, Serializable & obj);

}
