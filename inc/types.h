//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
#ifndef __TYPES_H__
#define __TYPES_H__


#define NULL  ((void *)0)

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef uint32_t pde_t;
typedef uint32_t pte_t;
typedef uint32_t size_t;

typedef int32_t pid_t;
typedef int32_t off_t;
typedef int32_t time_t;

typedef uint16_t uid_t;
typedef uint16_t gid_t;
typedef uint16_t dev_t;

#endif
