#include "dijkstra.h"

extern pthread_mutex_t count_mutex;

Dijkstra::Dijkstra() {
	source = 0;  // start each agent on node 0
	destination = -1;
	progress = 0.0;
	velocity_adjust = 0;
    currentDestination = -1;
    center = (rand() % 21) / 20.0;
    
    worth = (rand() / (double)(RAND_MAX)) * 50.0;
}

Dijkstra::Dijkstra(int _start, int _numNodes, double distances[100][100], int vel[100][100], int _theGoalNode, double _lambda, double given[4], int gameLength) {
	//printf("%i\n", _start);
	source = _start;
	destination = -1;
	progress = 0.0;//0.0
	velocity_adjust = (((rand() / (double)RAND_MAX) * 10.0) - 5.0) * (25 / gameLength);
	
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
    
    worth = 0.0;//(rand() / (double)(RAND_MAX)) * 50.0;
    
    double mu, sigma;
    for (i = 0; i < 4; i++) {
        mu = given[i] + 1.0 * (rand() / (double)RAND_MAX);
        sigma = 0.1 + 0.3 * (rand() / (double)RAND_MAX);
        //printf("%i: %.2lf  %.2lf\n", i, mu, sigma);
        s[i] = new myStatistics(NORMAL, mu, sigma);
        curUtilities[i] = s[i]->getSample();
        noGoCount = 0;
    }
    
    r = 0.9;//(float)(rand() % 2) * 0.9;
    g = 0.0;//(float)(rand() % 2) * 0.9;
    b = 0.0;//(float)(rand() % 2) * 0.9;    
}

Dijkstra::~Dijkstra() {
    printf("advanced worth = %lf\n", worth);
}

void Dijkstra::copyAgent(Dijkstra *_a) {
	source = _a->source;
	destination = _a->destination;
	progress = _a->progress;
    center = _a->center;
    currentDestination = _a->currentDestination;
    
    worth = _a->worth;
}

void Dijkstra::choose(Node *_world[100]) {
    pickDestination(_world);
    
	progress = 0.0;
	int l = -1, j;
    
    if (currentDestination >= 0) {
        double Q[4], sum = 0.0;
        
        for (j = 0; j < _world[source]->numLinks; j++)
            sum += kappa[source][_world[source]->links[j]];
        
        double temperature = 1.0 / (5.0 + (sum/2.0));
		//double best = 99999, tmp;
        sum = 0.0;
		for (j = 0; j < _world[source]->numLinks; j++) {
            pthread_mutex_lock( &count_mutex );
			Q[j] = -((cost[source][_world[source]->links[j]]*GAS_COST) + (_world[source]->toll[j] / 100.0) + toGo[_world[source]->links[j]]);
            pthread_mutex_unlock( &count_mutex );
            sum += exp(Q[j]/temperature);
		}
        
        double probs[4];
        for (j = 0; j < _world[source]->numLinks; j++) {
            probs[j] = exp(Q[j]/temperature) / sum;
        }
        
        double num = rand() / (double)RAND_MAX;
        sum = 0.0;
        for (j = 0; j < _world[source]->numLinks; j++) {
            sum += probs[j];
            if (num <= sum) {
                l = j;
                break;
            }
        }
        
        if (l == -1) {
            printf("we have a problem: %lf: (%lf, %lf)\n", num, probs[0], probs[1]);
            exit(1);
        }
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

int Dijkstra::update(double _progress, double t) {
	progress = _progress;
    worth -= t * GAS_COST;
	int rval = 0;
	if (progress >= 1.0) {
		if (destination == theGoalNode)
			rval = 1;
        
        if (destination == currentDestination) {
            worth += curUtilities[currentDestination];
            
            for (int i = 0; i < 4; i++)
                curUtilities[i] = s[i]->getSample();
            noGoCount = 0;
        }
		
		struct timeval now;
		double mtime, seconds, useconds;    
		gettimeofday(&now, NULL);  	
		seconds  = now.tv_sec  - previous.tv_sec;
		useconds = now.tv_usec - previous.tv_usec;
		double thistime = seconds + (useconds / 1000000.0);
		
		kappa[source][destination] ++;
		double _lambda = lambda / (1.0 + kappa[source][destination] / 20.0);//1.0 / (1.0 + kappa[source][destination]);
		if (lambda < 0.1)
			lambda = 0.1;
        
		cost[source][destination] = cost[source][destination] * (1.0 - _lambda) + thistime * _lambda;
		
		source = destination;
		progress = 0.0;
	}
	
	return rval;
}

void Dijkstra::runDijkstra(Node *_world[100], int dest) {
    pthread_mutex_lock( &count_mutex );
	toGo[dest] = 0;
	double tmp, mintmp;
	bool done;
	int i, j, round = 0;
	while (true) {
		done = true;
		for (i = 0; i < numNodes; i++) {
			if (i == dest)//theGoalNode)
				continue;
			
			mintmp = 99999;
			for (j = 0; j < _world[i]->numLinks; j++) {
				if (_world[i]->links[j] >= 0) {
                    if (_world[i]->linkStatus[j]) {
                        tmp = (cost[i][_world[i]->links[j]]*GAS_COST) + (_world[i]->toll[j] / 100.0) + toGo[_world[i]->links[j]];
                    }
                    else {
                        tmp = 99999.0;
                    }
					if (tmp < mintmp) 
						mintmp = tmp;
				}
			}
            
			
			if (fabs(toGo[i] - mintmp) > 0.000001)
				done = false;
			
			toGo[i] = mintmp;
			round ++;
		}
		if (done) {
			break;
		}
	}
    pthread_mutex_unlock( &count_mutex );
}

void Dijkstra::pickDestination(Node *_world[100]) {
    double expectedVal[4];
    int i, j;
    
    currentDestination = 0;
    for (i = 0; i < 4; i++) {
        runDijkstra(_world, i);
        if (i != source)
            expectedVal[i] = -toGo[source] + curUtilities[i];
        else {
            int j;
            double tmp, tmpmin = 99999;
            for (j = 0; j < 4; j++) {
                if (_world[i]->links[j] >= 0) {
                    if (_world[i]->linkStatus[j]) {
                        pthread_mutex_lock( &count_mutex );
                        tmp = (cost[i][_world[i]->links[j]]*GAS_COST) + (_world[i]->toll[j] / 100.0) + toGo[_world[i]->links[j]];
                        pthread_mutex_unlock( &count_mutex );
                    }
                    else
                        tmp = 99999.0;
                    if (tmp < tmpmin) 
                        tmpmin = tmp;
                }
            }
            expectedVal[i] = -tmpmin + curUtilities[i];
        }
        
        if (expectedVal[i] > expectedVal[currentDestination])
            currentDestination = i;
        
    }
    
    runDijkstra(_world, currentDestination);
}

