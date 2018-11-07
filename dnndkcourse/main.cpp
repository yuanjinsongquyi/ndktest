//
//  main.cpp
//  dnndkcourse
//
//  Created by 袁劲松 on 18/8/13.
//  Copyright © 2018年 袁劲松. All rights reserved.
//

#include <iostream>
#include "Header.h"
#include "lessen2.hpp"
void test (){
    printf("%s","sd");
}
void dayin(char *s){
    printf("%c",*s);
}
int main(int argc, const char * argv[]) {
    
    func(dayin);
    
    return 0;
}
