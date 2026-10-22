#include "types.h"

void *
memset(void *dst, පූර්ණ_සංඛ්‍යා c, uint n)
{
  var_na *cdst = (var_na *)dst;
  පූර්ණ_සංඛ්‍යා i;
  සඳහා (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ආපසු dst;
}

පූර්ණ_සංඛ්‍යා
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  අතරතුර (n-- > 0) {
    නම් (*s1 != *s2)
      ආපසු *s1 - *s2;
    s1++, s2++;
  }

  ආපසු 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  නම් (n == 0)
    ආපසු dst;

  s = src;
  d = dst;
  නම් (s < d && s + n > d) {
    s += n;
    d += n;
    අතරතුර (n-- > 0)
      *--d = *--s;
  } නැතහොත්
    අතරතුර (n-- > 0)
      *d++ = *s++;

  ආපසු dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ආපසු memmove(dst, src, n);
}

පූර්ණ_සංඛ්‍යා
strncmp(const var_na *p, const var_na *q, uint n)
{
  අතරතුර (n > 0 && *p && *p == *q)
    n--, p++, q++;
  නම් (n == 0)
    ආපසු 0;
  ආපසු (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, පූර්ණ_සංඛ්‍යා n)
{
  var_na *os;

  os = s;
  අතරතුර (n-- > 0 && (*s++ = *t++) != 0)
    ;
  අතරතුර (n-- > 0)
    *s++ = 0;
  ආපසු os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, පූර්ණ_සංඛ්‍යා n)
{
  var_na *os;

  os = s;
  නම් (n <= 0)
    ආපසු os;
  අතරතුර (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ආපසු os;
}

පූර්ණ_සංඛ්‍යා
strlen(const var_na *s)
{
  පූර්ණ_සංඛ්‍යා n;

  සඳහා (n = 0; s[n]; n++)
    ;
  ආපසු n;
}
