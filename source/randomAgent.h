#ifndef	RANDOMAGENT_H
#define RANDOMAGENT_H

#include "defs.h"
#include "Agent.h"
#include "myStatistics.h"

#define valuePrimary      1.0
#define valueSecondary    0.5
#define GAS_COST        0.079

class RandomAgent : public Agent {
public:
	RandomAgent();
	RandomAgent(int _start, int _numNodes, double distances[100][100], int vel[100][100], int _theGoalNode, double _lambda, double given[4]);
	~RandomAgent();
	
	void copyAgent(RandomAgent *_a);
	void choose(Node *_world[100]);
	int update(double _progress, double t);
	
	void runRandom(Node *_world[100], int dest);
	
    void pickDestination(Node *_world[100]);
    int currentDestination;
	
	double cost[100][100];
	int kappa[100][100];
	double toGo[100];
	struct timeval previous;
	
	int numNodes;
    
    double lambda;
    
    double curUtilities[4];
    myStatistics *s[4];
    
    int noGoCount;
};


#endif