#include <wchar.h>

int wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n) {
  size_t i;
  for (i=0; i<n && s1[i]==s2[i]; ++i) ;
  return s1[i]-s2[i];
}
