# printk

%[flags][width][.prec][h|l|L][type]


## flags
flags 控制输出对其方式、数值符号、小数点、尾零、二进制、八进制、十六进制
- `-`:左对齐，默认右对齐
- `+`：输出+号
- `#`：特殊转换
    - 八进制，转换后字符串首位必须是0
    - 十六进制，转换后必须以0x或0X开始
- `0`：使用0代替空格

## width
指定字符宽度，实际字符大于指定宽度则以实际为准，实际字符小于指定宽度，则用flags指定的符号填充