#ifndef AGENT_H
#define AGENT_H

#include "defs.h"
#include "node.h"

class Agent {
public:
	Agent();
	Agent(int _start, int _theGoalNode);
	virtual ~Agent();
	
	virtual void copyAgent(Agent *_a);
	virtual void choose(Node *_world[100]);
	virtual int update(double _progress, double t);

	int source, destination, clink;
	double progress;
	double velocity_adjust;
	int theGoalNode;
    double center;
    
    float r, g, b;
    
    double worth;
};

#endif