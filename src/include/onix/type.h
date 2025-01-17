#ifndef ONIX_TYPE_H
#define ONIX_TYPE_H

#define EOF -1
#define NULL ((void *)0)
#define EOS '\0'
#define bool _Bool
#define true 1
#define false 0


#define _packed __attribute__((packed))  // 用于定义特殊结构体

// 用于省略函数的桟帧
#define _ofp __attribute__((optimize("omit-frame-pointer")))

typedef unsigned int size_t;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;



#endif