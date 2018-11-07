//
//  lessen2.cpp
//  dnndkcourse
//
//  Created by 袁劲松 on 18/8/14.
//  Copyright © 2018年 袁劲松. All rights reserved.
//
#include "lessen2.hpp"
#include <stdarg.h>
typedef void (*Day)(char *s);
void func(void (*Day)(char *s)){
    char l='h';
    Day(&l);
}
void lessen2(){
    int s[]={1,2,3,4};
    int *m=s;
    for (int i = 0; i<4; i++) {
        printf("%d",*m);
        m++;
    }
    
}
void lessen2_1(char *m,...){
    va_list list ;
    va_start(list, m);
    for (int i =0;i<4;i++){
        int m = va_arg(list, int);
        printf("%d\n",m);
    }
}


