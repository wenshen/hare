/*
 *  utility.h
 *  Transport
 *
 *  Created by Wen on 4/1/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef UTILITY_H
#define UTILITY_H

#include "defs.h"
//#include "fstream.h"
//#include "iostream.h"
//#include "string.h"

void writeData(std::string app_data, std::string file_name);
void createFile(std::string fname3);
double gaussrand(double V, double E);
std::string getCurrentWorkPath();

#endif // UTILITY_H