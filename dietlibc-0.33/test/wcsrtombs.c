#include <wchar.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <string.h>

int main() {
  wchar_t a[]=L"Böses Encoding";
  const wchar_t* b=a;
  char buf[100];
  static mbstate_t ps;
  size_t l;
  setlocale(LC_ALL,"de_DE");
  l=wcsrtombs(buf,&b,sizeof(buf),&ps);
  assert(l==14); assert(!strcmp(buf,"B\xf6ses Encoding"));
  memset(buf,0,sizeof(buf));
  setlocale(LC_ALL,"de_DE.UTF8");
  b=a;
  l=wcsrtombs(buf,&b,sizeof(buf),&ps);
  assert(l==15); assert(!strcmp(buf,"Böses Encoding"));
}
