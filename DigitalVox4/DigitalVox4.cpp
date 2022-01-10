//
//  DigitalVox4.cpp
//  DigitalVox4
//
//  Created by 杨丰 on 2022/1/10.
//

#include <iostream>
#include "DigitalVox4.hpp"
#include "DigitalVox4Priv.hpp"

void DigitalVox4::HelloWorld(const char * s)
{
    DigitalVox4Priv *theObj = new DigitalVox4Priv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void DigitalVox4Priv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

