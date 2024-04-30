#include <stdio.h>
#include <stdlib.h>
#include "rat.h"

#define BUF_LEN 1024

rat_t parse_rat(const char *s, size_t len) {
  size_t slashpos = 0;
  for (size_t i = 0; i < len; i++) {
    if (s[i] == '/') {
      slashpos = i;
      break;
    }
    if (s[i] == '\0') break;
  }
  if (slashpos == 0 || slashpos >= len-1) return (rat_t) {0, 0};
  return (rat_t) {atoll(s), atoll(s+slashpos+1)};
}

int main() {
  rat_t rtest = rat_from_double(3.1415926535897932384626);
  printf("%lld/%lld\n", rtest.num, rtest.div);

  char buf[BUF_LEN] = {0};
  char *s;
  while (1) {
    printf("(a/b): ");
    s = fgets(buf, BUF_LEN, stdin);
    if (s == NULL) return 1;
    rat_t a = parse_rat(s, BUF_LEN);
    if (a.div == 0) return 0;
    
    printf("(c/d): ");
    s = fgets(buf, BUF_LEN, stdin);
    if (s == NULL) return 1;
    rat_t b = parse_rat(s, BUF_LEN);
    if (b.div == 0) return 0;
    
    int op = 0;
    while (op == 0) {
      printf("operation (+, -, *, /, %%): ");
      s = fgets(buf, BUF_LEN, stdin);
      if (s == NULL) return 1;
      if (s[0] == 0) return 0;
      for (size_t i = 0; i < BUF_LEN; i++) {
        char c = s[i];
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
          op = c;
          break;
        }
        if (c == '\n' || c == '\0') return 0;
      }
    }
    rat_t result;
    switch(op) {
      case '+': result = rat_add(a, b); break;
      case '-': result = rat_sub(a, b); break;
      case '*': result = rat_mul(a, b); break;
      case '/': result = rat_div(a, b); break;
      case '%': result = rat_mod(a, b); break;
      default: result = (rat_t) {0, 0};
    }
    printf("%lld/%lld %c %lld/%lld = %lld/%lld\n", a.num, a.div, op, b.num, b.div, result.num, result.div);
  }
}
