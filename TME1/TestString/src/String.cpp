#include "String.h"
#include <iostream>
namespace pr
{

// TODO: Implement constructor e.g. using initialization list
String::String (const char *s)
{
  data = newcopy(s);
  std::cout << "String constructor called for: " << data << std::endl;
}


String::String(const String& other):String(other.data){}


String::String(String&& other)noexcept{
  data = other.data;
  other.data = nullptr;
}

String::~String ()
{
  std::cout << "String destructor called for: " << (data ? data : "(null)")
      << std::endl;
  delete[] data;
}


// TODO : add other operators and functions
std::ostream& operator<<(std::ostream& os, const String& str){
  return os<<str.data;
}


bool String::operator<(const String& other) const{
  int result = compare(data, other.data);
  return result < 0;
}


String &String::operator=(const String &other){
  if(this != &other){
    delete []data;
    data = newcopy(other.data);
  }
  return *this;
}


String& String::operator=(String&& other) noexcept{
  if(this != &other){
    delete [] data;
    data = other.data;
    other.data = nullptr;
  }
  return *this;
}


bool operator==(const String& a, const String& b){
  return compare(a.data,b.data) == 0;
}


String operator+(const String& a, const String& b){
  char* tmp = newcat(a.data, b.data);  
  String result(tmp);                   
  delete[] tmp;                         
  return result; 
}


}// namespace pr

