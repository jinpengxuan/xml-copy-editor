#ifndef PARSER_DATA_H
#define PARSER_DATA_H

#include <string>
#include <vector>

class ParserData
{
  public:
    ParserData() { count = 0; }
    ~ParserData() { }
    void push(const std::string& s) { stack.push_back(s); }
    void pop() { stack.pop_back(); }
    void setState(int i) { state = i; ++count; }
    int getState() { return state; }
    int getCount() { return count; }
    std::vector<std::string> getStack() { return stack; }
    std::string getElement()
    {
      return (stack.empty()) ? "" : stack.at(stack.size() - 1);
    }
    std::string getParent()
    {
      return (stack.size() < 2) ? "" : stack.at(stack.size() - 2);
    }
  private:
    int state, count;
    std::vector<std::string> stack;
};

#endif
