#pragma once

#include "rosban_utils/serializable.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace rosban_utils
{

/// This class implements a factory pattern. It can be used for producing
/// various objects inheriting from class T. More specific initialization can be
/// achieved by using data from a xml tree
template <class T>
class Factory
{
public:
  /// Builder can use xml data
  typedef std::function<T *(TiXmlNode *node)> Builder;

  Builder getBuilder(const std::string &class_name) const
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

  T * build(const std::string &class_name) const
    {
      Builder b = getBuilder(class_name);
      return b(NULL);
    }

  T * build(TiXmlNode *node) const
    {
      std::string class_name = xml_tools::read<std::string>(node, "class_name");
      Builder b = getBuilder(class_name);
      return b(node);
    }

  /// Send an error if a builder for the given class_name is already registered
  void registerBuilder(const std::string &class_name, Builder builder)
    {
      if (builders.count(class_name) != 0)
        throw std::runtime_error("Factory: registering a class named '" + class_name
                                 + "' while it already exists");
      builders[class_name] = builder;
    }

private:
  /// Contains a mapping from class names to builders
  std::unordered_map<std::string, Builder>  builders;
};

}
