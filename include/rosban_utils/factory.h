#pragma once

#include "rosban_utils/io_tools.h"
#include "rosban_utils/serializable.h"

#include <fstream>
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
  /// Builder which takes no argument
  typedef std::function<std::unique_ptr<T>()> Builder;
  /// XMLBuilder can use xml data
  typedef std::function<std::unique_ptr<T>(TiXmlNode *node)> XMLBuilder;
  /// Builder using an input stream to customize the created object @see StreamSerializable
  typedef std::function<std::unique_ptr<T>(std::istream & in,
                                           int * nb_bytes_read)> StreamBuilder;

  XMLBuilder getBuilder(const std::string &class_name) const
    {
      try
      {
        return xml_builders.at(class_name);
      }
      catch (const std::out_of_range &exc)
      {
        throw std::out_of_range("Factory: type '" + class_name + "' is not registered"); 
      }
    }

  Builder getBuilder(int id) const
    {
      try
      {
        return builders_by_id.at(id);
      }
      catch (const std::out_of_range &exc)
      {
        std::ostringstream oss;
        oss << "Factory: id '" << id << "' is not registered";
        throw std::out_of_range(oss.str()); 
      }
    }

  StreamBuilder getStreamBuilder(int id) const
    {
      try
      {
        return stream_builders_by_id.at(id);
      }
      catch (const std::out_of_range &exc)
      {
        std::ostringstream oss;
        oss << "Factory: id '" << id << "' is not registered";
        throw std::out_of_range(oss.str()); 
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

  std::unique_ptr<T> build(int id) const
    {
      return getBuilder(id)();
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

  std::unique_ptr<T> read(TiXmlNode * node, const std::string & key)
    {
      if(!node) throw XMLParsingError("Null node when trying to read from a factory");
      TiXmlNode * child = node->FirstChild(key);
      if (!child) throw XMLParsingError("Could not find node with label '" + key + "' in node: '"
                                        + node->Value() + "'");
      return build(child);
    }

  /// Return the number of bytes read
  int read(std::istream & in, std::unique_ptr<T> & ptr)
    {
      int bytes_read = 0;
      int id;
      bytes_read += rosban_utils::read<int>(in, &id);
      int builder_bytes_read;
      ptr = getStreamBuilder(id)(in, &builder_bytes_read);
      bytes_read += builder_bytes_read;
      return bytes_read;
    }

  /// Return the number of bytes read
  int loadFromFile(const std::string & filename, std::unique_ptr<T> & ptr)
    {
      std::ifstream in(filename, std::ios::binary);
      if (!in) {
        std::ostringstream oss;
        oss << "Failed to open '" << filename << "' for binary reading";
        throw std::runtime_error(oss.str());
      }
      int bytes_read = read(in, ptr);
      in.close();
      return bytes_read;
    }

  /// Fill 'ptr' with a generated object with the given node as argument
  /// If node is null or key is not found:
  /// - no error is thrown and 'ptr' content is not modified
  /// If key is found but content cannot be parsed:
  /// - an error is still raised and the 'ptr' is not overwritten
  /// If key is found and content is parsed properly:
  /// - 'ptr' is overwritten (and thus, old value is deleted according to unique_ptr)
  void tryRead(TiXmlNode * node, const std::string & key, std::unique_ptr<T> & ptr)
    {
      TiXmlNode * child;
      try {
        if(!node) throw XMLParsingError("");
        child = node->FirstChild(key);
        if (!child) throw XMLParsingError("");
      }
      // Abort reading if node was null or child was not found
      catch (const XMLParsingError & err) { return; }
      ptr = build(child);
    }

  static XMLBuilder toXMLBuilder(Builder builder, bool parse_xml)
    {
      return [builder, parse_xml](TiXmlNode * node) -> std::unique_ptr<T>
      {
        std::unique_ptr<T> object(builder());
        if (node != nullptr && parse_xml) object->from_xml(node);
        return std::move(object);
      };
    }

  static StreamBuilder toStreamBuilder(Builder builder)
    {
      return [builder](std::istream & in,
                       int * nb_bytes_read) -> std::unique_ptr<T>
      {
        std::unique_ptr<T> object(builder());
        int tmp = object->read(in);
        if (nb_bytes_read != nullptr) *nb_bytes_read = tmp;
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
      if (xml_builders.count(class_name) != 0)
        throw std::runtime_error("Factory: registering a class named '" + class_name
                                 + "' while it already exists");
      xml_builders[class_name] = builder;
    }

  /// Send an error if a builder for the given id is already registered
  /// if autocreate_streambuilder is true, then it also create and register a streambuilder
  void registerBuilder(int id, Builder builder, bool autocreate_streambuilder = true)
    {
      if (autocreate_streambuilder) {
        registerBuilder(id, toStreamBuilder(builder));
      }
      if (builders_by_id.count(id) != 0) {
        std::ostringstream oss;
        oss << "Factory: registering a class with id '" << id
            << "' while it is already used";
        throw std::runtime_error(oss.str());
      }
      builders_by_id[id] = builder;
    }

  /// Send an error if a builder for the given id is already registered
  void registerBuilder(int id, StreamBuilder builder)
    {
      if (stream_builders_by_id.count(id) != 0) {
        std::ostringstream oss;
        oss << "Factory: registering a class with id '" << id
            << "' while it is already used";
        throw std::runtime_error(oss.str());
      }
      stream_builders_by_id[id] = builder;
    }

private:
  /// Contains a mapping from class names to builders
  std::unordered_map<std::string, XMLBuilder>  xml_builders;

  /// Contains a mapping from class names to builders
  std::unordered_map<int, Builder>  builders_by_id;
  /// Contains a mapping from class names to builders
  std::unordered_map<int, StreamBuilder>  stream_builders_by_id;
};

}
