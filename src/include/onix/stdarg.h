#ifndef ONIX_STDARG_H
#define ONIX_STDARG_H

typedef char *va_list;
#define va_start(ap,v) (ap = (va_list)&v + sizeof(v))
#define va_arg(ap,t) (*(t*)((ap += sizeof(t))- sizeof(t))) // ap = ap+4; *(ap-4)
#define va_end(ap) (ap = (va_list)0)

#endif
