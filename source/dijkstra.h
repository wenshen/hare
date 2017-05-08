#ifndef	DIJKSTRA_H
#define DIJKSTRA_H

#include "defs.h"
#include "Agent.h"
#include "myStatistics.h"

#define valuePrimary      1.0
#define valueSecondary    0.5
#define GAS_COST        0.079

class Dijkstra : public Agent {
public:
	Dijkstra();
	Dijkstra(int _start, int _numNodes, double distances[100][100], int vel[100][100], int _theGoalNode, double _lambda, double given[4], int gameLength);
	~Dijkstra();
	
	void copyAgent(Dijkstra *_a);
	void choose(Node *_world[100]);
	int update(double _progress, double t);
	
	void runDijkstra(Node *_world[100], int dest);

	
    void pickDestination(Node *_world[100]);
    int currentDestination;//primaryDestination, secondaryDestination;
	
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
