#include "Stringify.hpp"

union union_test {
  int lol;
  char lol2;
};

class Test1 {
public:
  std::string to_string(void) {
    char * len_name = (char*) typeid(*this).name();
    for (; isdigit(*len_name); len_name++) ;
    std::ostringstream ss (std::ostringstream::out);
    ss << "#<" << len_name << ":FAKE>";
    return ss.str();
  }
};

class Test2 : public Test1 {
public:
  using Test1::to_string;
  std::string to_string(int) {
    return "#<LOL>";
  }
};

class Test6 {
public:
  std::string to_string() {
    std::ostringstream ss (std::ostringstream::out);
    ss << "#<MyTest6:0x" << std::hex << (long)this << '>';
    return ss.str();
  }
};

class Test3 {
};

class Test4 {
public:
  std::string to_string(int) {
    return "#<LOL>";
  }
};

class Test5 {
};

std::ostream& operator<<(std::ostream& s, Test5) {
  s << "YAY";
  return s;
}

std::ostream& operator<<(std::ostream& s, Test6) {
  s << "FAKE";
  return s;
}

int main(void) {
  Test1 test1;
  Test2 test2;
  Test3 test3;
  Test4 test4;
  Test5 test5;
  Test6 test6;
  union_test test7;
  int WAT = 69;

  std::cout << Stringify::to_string(test1) << std::endl;
  std::cout << Stringify::to_string(test2) << std::endl;
  std::cout << Stringify::to_string(test3) << std::endl;
  std::cout << Stringify::to_string(test4) << std::endl;
  std::cout << Stringify::to_string(test5) << std::endl;
  std::cout << Stringify::to_string(test6) << std::endl;
  std::cout << Stringify::to_string(test7) << std::endl;
  std::cout << Stringify::to_string(WAT) << std::endl;

  return 0;
}
