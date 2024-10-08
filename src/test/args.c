#include <onix/stdarg.h>
//#include <stdio.h>
void show(int cnt,...){
    va_list arg;
    va_start(arg,cnt);

    int ret;
    while (cnt--)
    {
        //printf("%d\n",va_arg(arg,int));
        ret = va_arg(arg,int);
    }

    va_end(arg);

}

int main(){
    show(4,1,2,3,4);
    return 0;
}