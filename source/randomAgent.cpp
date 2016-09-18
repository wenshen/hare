/*
 *  random.cpp
 *  Transport
 *
 *  Created by Wen on 4/1/13.
 *  Copyright 2013 __wenshen__. All rights reserved.
 *
 */

#include "randomAgent.h"

extern pthread_mutex_t count_mutex;

RandomAgent::RandomAgent() {
	source = 0;  // start each agent on node 0
	destination = -1;
	progress = 0.0;
	velocity_adjust = 0;
    currentDestination = -1;
    center = (rand() % 21) / 20.0;
    
    worth = (rand() / (double)(RAND_MAX)) * 50.0;
}

RandomAgent::RandomAgent(int _start, int _numNodes, double distances[100][100], int vel[100][100], int _theGoalNode, double _lambda, double given[4]) {
	source = _start;
	destination = -1;
	progress = 0.0;//0.0
	velocity_adjust = ((rand() / (double)RAND_MAX) * 10.0) - 5.0;
	
	theGoalNode = _theGoalNode;
	numNodes = _numNodes;
	
	int i, j;
	for (i = 0; i < numNodes; i++) {
		toGo[i] = 99999;
		for (j = 0; j < numNodes; j++) {
			kappa[i][j] = 0;
			cost[i][j] = distances[i][j] / ((vel[i][j]+velocity_adjust) / 75.0);
		}
	}
    
    lambda = _lambda;
    
    //getNewDestinations();
    center = (rand() % 21) / 20.0;
    
    worth = (rand() / (double)(RAND_MAX)) * 50.0;
    
    double mu, sigma;
    for (i = 0; i < 4; i++) {
        mu = given[i] + 1.0 * (rand() / (double)RAND_MAX); // 0.6
        sigma = 0.1 + 0.3 * (rand() / (double)RAND_MAX);
        s[i] = new myStatistics(NORMAL, mu, sigma);
        curUtilities[i] = s[i]->getSample();
        noGoCount = 0;
    }
}

RandomAgent::~RandomAgent() {
}

void RandomAgent::copyAgent(RandomAgent *_a) {
	source = _a->source;
	destination = _a->destination;
	progress = _a->progress;
    center = _a->center;
    currentDestination = _a->currentDestination;
    
    worth = _a->worth;
}

void RandomAgent::choose(Node *_world[100]) {
    pickDestination(_world);
    
	progress = 0.0;
	int l = -1, j;
	
    if (currentDestination >= 0) {
		progress = 0.0;
		l =  rand() % _world[source]->numLinks;
    }
    
    if (l == -1) {
        destination = -1;
        clink = -1;
    }
    else {
		destination = _world[source]->links[l];
        clink = l;
    }
	
	gettimeofday(&previous, NULL);

}

int RandomAgent::update(double _progress, double t) {
	progress = _progress;
	int rval = 0;
	if (progress >= 1.0) {
		if (destination == theGoalNode)
			rval = 1; 	
		
		source = destination;
		progress = 0.0;
	}
	
	return rval;
}

void RandomAgent::runRandom(Node *_world[100], int dest) {
	
}

void RandomAgent::pickDestination(Node *_world[100]) {

    currentDestination = rand() % 4;
	runRandom(_world, currentDestination);
}

