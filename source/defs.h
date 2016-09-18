#ifndef DEFS_H
#define DEFS_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

#include "pthread.h"

#define STRAIGHT	0
#define TWO_WAY		1
#define BELOW		2

//#define RANDOM_TYPE		0
//#define DIJKSTRA_TYPE	1
//#define STATIC_TYPE     2


// Time                     Total-agents-that-have-passed-through-node-D
#define DATA_SYS "..//data//data_sys"

// Time   node-num  Link_num  old-Toll     new-Toll
#define DATA_CON "..//data//data_con"

// Time      node-num    link-num        agents_num_on_each_link     
#define DATA_AGT  "..//data//data_agt"

#endif