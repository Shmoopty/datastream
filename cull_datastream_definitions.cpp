// these definitions are needed in places spread throughout the namespace,
// and could be maintained in more local files, but for ease of maintainence
// they are all here

#include <string>
#include <regex>
#include <map>
#include <type_traits>
#include <memory>
#include <functional>

#include "datastream_definitions.h"

using namespace datastream;

using std::string;
using std::map;
using std::regex;

// bool isBlank (const std::string & line){
// 	return std::regex_match (line,blank_pattern);
// }
//
// bool isComment (const std::string & line){
// 	return std::regex_match(line,comment_pattern);
// }

//typedef std::sregex_token_iterator pattern_iterator;

template<typename T>
T& deref(T &v) {
  return v;
}

template<typename T>
const T& deref(const T& v) {
  return v;
}

template<typename T>
typename std::enable_if<!std::is_function<T>::value, T&>::type deref(T* v) {
  return deref(*v);
}

template<typename T>
const T& deref(const std::shared_ptr<T>& v) {
  return deref(*v);
}

template<typename T>
const T& deref(const std::weak_ptr<T>& v) {
  return deref(*v);
}
