/**
 * ----------------------------------------------------------------------------
 * File       : apci1710Main.cpp
 * Author     : Marty Kraimer
 * Created    : 2000-03-17
 * Last update: 2016-05-06
 * ----------------------------------------------------------------------------
 * Description:
 * Example ioc application.
 * ----------------------------------------------------------------------------
 * This file is part of apci1710-asyn. It is subject to 
 * the license terms in the LICENSE.txt file found in the top-level directory 
 * of this distribution and at: 
 *   https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
 * No part of apci1710-asyn, including this file, may be 
 * copied, modified, propagated, or distributed except according to the terms 
 * contained in the LICENSE.txt file.
 * ----------------------------------------------------------------------------
**/

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsThread.h"
#include "iocsh.h"

int main(int argc,char *argv[])
{
    if(argc>=2) { 
        iocsh(argv[1]);
        epicsThreadSleep(.2);
    }
    iocsh(NULL);
    return(0);
}
