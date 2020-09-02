int2store函数使用分析
====================
以下内容来自mysql 5.7.26  

# 1. 文件 my_byteorder.h

```
/*
  Functions for reading and storing in machine independent
  format (low byte first). There are 'korr' (assume 'corrector') variants
  for integer types, but 'get' (assume 'getter') for floating point types.
*/
#if defined(__i386__) || defined(_WIN32) || defined(__x86_64__)
#include "byte_order_generic_x86.h"
#else
#include "byte_order_generic.h"
#endif

static inline void int3store(uchar *T, uint A)
{
  *(T)=   (uchar) (A);
  *(T+1)= (uchar) (A >> 8);
  *(T+2)= (uchar) (A >> 16);
}

#ifdef __cplusplus

static inline void int2store(char *pT, uint16 A)
{
  int2store(static_cast<uchar*>(static_cast<void*>(pT)), A);
}

static inline void int3store(char *pT, uint A)
{
  int3store(static_cast<uchar*>(static_cast<void*>(pT)), A);
}

static inline void int4store(char *pT, uint32 A)
{
  int4store(static_cast<uchar*>(static_cast<void*>(pT)), A);
}

#endif  /* __cplusplus */

```
# 2. 文件 byte_order_generic_x86.h 

```
static inline void int2store(uchar *T, uint16 A) SUPPRESS_UBSAN;
static inline void int2store(uchar *T, uint16 A)
{
  *((uint16*) T)= A;
}

static inline void int4store(uchar *T, uint32 A) SUPPRESS_UBSAN;
static inline void int4store(uchar *T, uint32 A)
{
  *((uint32*) T)= A;
}

```
# 3. 文件 byte_order_generic.h 

```
static inline void int2store(uchar *T, uint16 A)
{
  uint def_temp= A ;
  *(T)=   (uchar)(def_temp);
  *(T+1)= (uchar)(def_temp >> 8);
}

static inline void int4store(uchar *T, uint32 A)
{
  *(T)=  (uchar) (A);
  *(T+1)=(uchar) (A >> 8);
  *(T+2)=(uchar) (A >> 16);
  *(T+3)=(uchar) (A >> 24);
}
```
