#pragma once

#include "rosban_utils/serializable.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace rosban_utils
{

// TODO:
// - Add read options
//   - std::unique_ptr read(node, name)
//   - std::unique_ptr tryRead(node, name, std::unique_ptr &)
// - Add write options eventually (issues with guarantees of const, maybe only for shared_ptr)

/// This class implements a factory pattern. It can be used for producing
/// various objects inheriting from class T. More specific initialization can be
/// achieved by using data from a xml tree
template <class T>
class Factory
{
public:
  /// XMLBuilder can use xml data
  typedef std::function<std::unique_ptr<T>()> Builder;
  typedef std::function<std::unique_ptr<T>(TiXmlNode *node)> XMLBuilder;

  XMLBuilder getBuilder(const std::string &class_name) const
    {
      try
      {
        return builders.at(class_name);
      }
      catch (const std::out_of_range &exc)
      {
        throw std::out_of_range("Factory: type '" + class_name + "' is not registered"); 
      }
    }

  std::unique_ptr<T> build(const std::string &class_name) const
    {
      XMLBuilder b = getBuilder(class_name);
      return b(NULL);
    }

  /// When building from a node, the expected format of the xml is the following:
  /// <class_name>...</class_name>
  std::unique_ptr<T> build(TiXmlNode *node) const
    {
      // Checking node validity
      if (node == NULL)
      {
        throw std::runtime_error("Factory::build: Trying to build an object from a NULL node");
      }
      // Checking if the node has a child and which is its class
      TiXmlNode * content_node = node->FirstChild();
      if (content_node == NULL)
      {
        std::ostringstream oss;
        oss << "Factory::build: expecting a child to node '" << node->Value() << "'";
        throw std::runtime_error(oss.str());
      }
      std::string class_name = content_node->Value();
      XMLBuilder b = getBuilder(class_name);
      return b(content_node);
    }

  /// path: path to xml_file
  /// node_name: name of the root node in the file
  std::unique_ptr<T> buildFromXmlFile(const std::string &path, const std::string &node_name) const
    {
      TiXmlDocument * doc = xml_tools::file_to_doc(path);
      if(!doc) throw std::runtime_error("Failed to load file " + path);

      TiXmlNode * node = doc->FirstChild(node_name.c_str());
      if(!node) throw std::runtime_error("Failed to find node with tag "
                                         + node_name + " in xml file " + path);

      try
      {
        std::unique_ptr<T> obj = build(node);
        delete doc;
        return obj;
      }
      catch (const std::runtime_error &exc)
      {
        delete doc;
        throw exc;
      }
    }

  static XMLBuilder toXMLBuilder(Builder builder, bool parse_xml)
    {
      return [builder, parse_xml](TiXmlNode * node) -> std::unique_ptr<T>
      {
        std::unique_ptr<T> object(builder());
        if (parse_xml) object->from_xml(node);
        return std::move(object);
      };
    }

  /// Send an error if a builder for the given class_name is already registered
  /// - Automatically transform the builder in XMLBuilder
  void registerBuilder(const std::string &class_name, Builder builder, bool parse_xml = true)
    {
      registerBuilder(class_name, toXMLBuilder(builder, parse_xml));
    }

  /// Send an error if a builder for the given class_name is already registered
  void registerBuilder(const std::string &class_name, XMLBuilder builder)
    {
      if (builders.count(class_name) != 0)
        throw std::runtime_error("Factory: registering a class named '" + class_name
                                 + "' while it already exists");
      builders[class_name] = builder;
    }

private:
  /// Contains a mapping from class names to builders
  std::unordered_map<std::string, XMLBuilder>  builders;
};

}
