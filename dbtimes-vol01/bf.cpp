#include <stdio.h>
#include <string>
int main() {
  std::string code;
  for (int ch;(ch=getchar())!=EOF;)
    code.push_back(ch);
  static int membuf[30000];
  int *mem = membuf;
  int depth = 0;
  for (size_t pc=0;pc<code.size();++pc) {
    switch(code[pc]) {
      case '+': ++*mem; break;
      case '-': --*mem; break;
      case '>': ++mem; break;
      case '<': --mem; break;
      case ',': *mem = getchar(); break;
      case '.': putchar(*mem); break;
      case '[':
        if (*mem == 0) {
          depth = 1;
          while (depth != 0) {
            switch(code[++pc]) {
              case '[': ++depth; break;
              case ']': --depth; break;
            }}}
        break;
      case ']':
        depth = -1;
        while (depth != 0) {
          switch(code[--pc]) {
            case '[': ++depth; break;
            case ']': --depth; break;
          }}
        --pc;
        break;
    }}
  return 0;
}
