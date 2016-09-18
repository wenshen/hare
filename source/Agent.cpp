#include "Agent.h"

Agent::Agent() {
	source = 0;  // start each agent on node 0
	destination = -1;
	progress = 0.0;
	velocity_adjust = 0;
    center = (rand() % 21) / 20.0;
    
    worth = 25.0;//(rand() / (double)(RAND_MAX)) * 50.0;

    r = (float)(rand() % 2) * 0.9;
    g = (float)(rand() % 2) * 0.9;
    b = (float)(rand() % 2) * 0.9;        
}

Agent::Agent(int _start, int _theGoalNode) {
	source = _start;
	destination = -1;
	progress = 0.0;//0.0
	velocity_adjust = ((rand() / (double)RAND_MAX) * 10.0) - 5.0;
	theGoalNode = _theGoalNode;
    center = (rand() % 21) / 20.0;
    
    worth = 25.0;//(rand() / (double)(RAND_MAX)) * 50.0;

    r = (float)(rand() % 2) * 0.9;
    g = (float)(rand() % 2) * 0.9;
    b = (float)(rand() % 2) * 0.9;    
}

Agent::~Agent() {
}

void Agent::copyAgent(Agent *_a) {
	source = _a->source;
	destination = _a->destination;
	progress = _a->progress;
    center = _a->center;
    worth = _a->worth;
    
    r = _a->r;
    g = _a->g;
    b = _a->b;
}

void Agent::choose(Node *_world[100]) {
	// select the next destination randomly
	progress = 0.0;
	int l =  rand() % _world[source]->numLinks;
	destination = _world[source]->links[l];
	printf("destination = %i\n", destination);
}

int Agent::update(double _progress, double t) {
	printf("agent update\n");	
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
