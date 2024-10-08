CFLAGS:= -m32
CFLAGS+= -Qn # 去掉gcc版本信息
# CFLAGS+= -fno-builtin # 不需要gcc内置函数
# CFLAGS+= -nostdinc # 不需要标准头文件
# CFLAGS+= -fno-pic # 不需要位置无关代码
# CFLAGS+= -fno-pie # 不需要位置无关可执行程序
# CFLAGS+= -nostdlib # 不需要标准库
# CFLAGS+= -fno-stack-protector # 不需要桟保护
CFLAGS+= -fno-asynchronous-unwind-tables  # 不需要调用桟信息
CFLAGS+= -fno-pic # 不需要位置无关代码
CFLAGS+= -mpreferred-stack-boundary=2 # 不要桟对齐
CFLAGS+=-fomit-frame-pointer
CFLAGS:= $(strip ${CFLAGS})

.PHONY:hello.out
hello.out:hello.c
	gcc $(CFLAGS) $< -o $@

.PHONY:param.s
param.s:param.c
	gcc $(CFLAGS) -S $< $@

.PHONY:hello.s
hello.s:hello.c
	gcc $(CFLAGS) -S $< $@
