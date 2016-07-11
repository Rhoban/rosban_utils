#pragma once

#include <iostream>
#include <string>

namespace rosban_utils
{

/// Return the content of the whole file as a string
std::string file2string(const std::string &path);

/// WARNING:
/// Those versions are implemented with the quick and dirty approach.
/// - endianness is ignored
/// - errors when writing are not handled

/// Return the number of bytes written
int writeInt(std::ostream & out, int val);
/// Return the number of bytes written
int writeDouble(std::ostream & out, double val);
/// Return the number of bytes written
int writeIntArray(std::ostream & out, int * values, int nb_values);
/// Return the number of bytes written
int writeDoubleArray(std::ostream & out, double * values, int nb_values);

}
