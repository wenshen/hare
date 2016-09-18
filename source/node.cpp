#include "node.h"

Node::Node() {
}

Node::Node(double _posx, double _posy, int _links[4], int _linkTypes[4], int _velocity[4], int _capacity[4]) {
	posx = _posx;
	posy = _posy;
	
	int i;
	numLinks = 0;
	for (i = 0; i < 4; i++) {
		links[i] = _links[i];
		linkTypes[i] = _linkTypes[i];
        linkStatus[i] = true;
        infoStatus[i] = false;
        congested[i] = false;
        timeCongested[i] = 0.0;
        toll[i] = 100;//((rand() % 199) + 1) - 100;
		velocity[i] = _velocity[i];
		capacity[i] = _capacity[i];
		
		if (links[i] >= 0)
			numLinks ++;
        
        quo[i] = 0;
        lastChange[i] = 0.0;
	}
	
	printf("node created at %.1lf, %.1lf\n", posx, posy);
}

Node::~Node() {
}
