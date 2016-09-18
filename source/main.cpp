#include "defs.h"
#include "node.h"
#include "Agent.h"
#include "Dijkstra.h"
#include "static.h"
#include "randomAgent.h"
//#include "utility.h"

//#include <string>
//#include <vector>
//#include <cstdlib>
//#include <sstream>
//#include<iomanip>

void *runGame(void *lparam);
void *logingData(void *lparam);
void readGame(char *fname);
void readParameters(int argc, char *argv[]);
void log_data();

pthread_t runGameThread;
pthread_t logDataThread;

extern void initGl(int argc, char *argv[], const char *title, Node *_world[100], int _numWorldNodes);
extern void initAgentsCopy(int percentLearning, int _numAgents);
extern void updateAgents(Agent **_agents, int _numAgents, int _throughCount);



extern Node worldCopy[100];
extern double empiezaConv;

Node *world[100];
int numWorldNodes = 0;
int theGoalNode;
double distances[100][100];
int vel[100][100], cap[100][100];

//int running_model_type;
unsigned long long callLogDataCount = 0;
double temp_timer;

int throughCount = 0;

Agent **agents;
int numAgents = 0;

double Wealth = 25.0;
double GOrevenue = 0.0;
double tCosts = 0.0;

int gameLength=25;


double highScores[5];
char highNames[5][1024];

bool done = false, highScoreFlag = false;
extern double currentScore;

char yourPseudoname[1024], highScoreMessage[1024];

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

int sitters[4];

FILE *fpAGT, *fpSYS, *fpCON;

bool tollLimitations = false;

int percentLearning = 0;


/* ****************************************
 
./JiaoTong 4nodes [numVehicles] [gameLength] [percentLearning] [tollLimitation?] [psuedoName]
 
******************************************* */
int main(int argc, char *argv[]) {
	srand(time(NULL));

	readParameters(argc, argv);
    
    //printf("read the parameters\n"); fflush(stdout);
	
	pthread_create(&runGameThread, NULL, runGame, NULL);
	pthread_create(&logDataThread, NULL, logingData, NULL);
	initGl(argc, argv, "Jiao Tong", world, numWorldNodes);
	
	return 0;
}

void log_data(){
	//printf("in log_data2\n");
	callLogDataCount ++;
	//printf("callLogDataCount: %i\n", callLogDataCount);
	struct timeval now_s;
	gettimeofday(&now_s, NULL);
	double elapsed_s = now_s.tv_sec + (now_s.tv_usec / 1000000.0);
	double interval_s = elapsed_s - empiezaConv;
	//Time sequence
	int agentsNumber, linkNumber;
	
	
	if (callLogDataCount == 1){
		temp_timer =-1.0;
	}
	//printf("temp_timer: %lf\n", temp_timer);
	double time_s = (int(interval_s*10))/10.0;
	//printf("time_s: %lf\n", time_s);
	
	if (((int)(time_s*10))%5==0 && (((int)(temp_timer*10))/5 != ((int)(time_s*10))/5 )) {
		//printf("in if!!!\n");
		temp_timer = time_s;
		
		for (int i = 0; i < numWorldNodes-1; i++) {
			for (int j = 0; j < world[i]->numLinks; j++) {
				//if(secCount == 41){
				linkNumber = world[i]->links[j];
				agentsNumber = 0;
				for (int k = 0; k < numAgents; k++) {
					if ((agents[k]->source == i) && (agents[k]->destination == linkNumber))
						agentsNumber ++;
				}
            
				if (world[i]->links[j] >-1 && gameLength * 60.0+0.1 > time_s) {
                    fprintf(fpAGT, "%.1lf %i %i %i\n", time_s, i, linkNumber, agentsNumber);
                    fflush(fpAGT);
				}
								
			}
			
		}
		
		
		
		if (gameLength * 60.0+0.1 > time_s) {
			// Time                     Total-agents-that-have-passed-through-node-D
            fprintf(fpSYS, "%.1lf %i\n", time_s, throughCount);
            fflush(fpSYS);
			
		}
        
	}
}

void *logingData(void *lparam){
    char fname[1024];
    sprintf(fname, "../data/data_agt_%s.txt", yourPseudoname);
    fpAGT = fopen(fname, "w");
    sprintf(fname, "../data/data_sys_%s.txt", yourPseudoname);
    fpSYS = fopen(fname, "w");
    sprintf(fname, "../data/data_con_%s.txt", yourPseudoname);
    fpCON = fopen(fname, "w");

	//struct timeval  now;   
    double elapsedTime = 0.0;
	//printf("in logingData!!\n");
	//bool run_label = true;
	while (!done) {
		
		log_data();
		usleep(500);
	}
    
    fclose(fpAGT);
    fclose(fpSYS);
    fclose(fpCON);    
}

void *runGame(void *lparam) {
	int i, j;
	double inc = 0.0;
	int x[100][100];
	
    
    usleep(200000);
    
	double speed, factor;
	
	struct timeval previous, now;
	double mtime, seconds, useconds;    
	gettimeofday(&previous, NULL);

    double elapsedTime = 0.0;
    
	while ((gameLength * 60.0) > elapsedTime) {
		
		for (i = 0; i < numWorldNodes; i++) {
			for (j = 0; j < numWorldNodes; j++) {
				x[i][j] = 0;
			}
            sitters[i] = 0;
		}
		for (i = 0; i < numAgents; i++) {
			if (agents[i]->destination != -1) {
				x[agents[i]->source][agents[i]->destination] ++;
			}
            else {
                sitters[agents[i]->source] ++;
            }
		}		
		
		// get the time
		gettimeofday(&now, NULL);
        
        
        elapsedTime = (now.tv_sec + (now.tv_usec / 1000000.0)) - empiezaConv;
        //printf("elapsedTime = %lf\n", elapsedTime);
        
		seconds  = now.tv_sec  - previous.tv_sec;
		useconds = now.tv_usec - previous.tv_usec;
		mtime = seconds + (useconds / 1000000.0);
		previous = now;
        
        pthread_mutex_lock( &count_mutex );
        for (i = 0; i < numWorldNodes; i++) {
            for (j = 0; j < world[i]->numLinks; j++) {
                world[i]->linkStatus[j] = worldCopy[i].linkStatus[j];
                world[i]->toll[j] = 2.0 * worldCopy[i].toll[j];
            }
        } 
        //printf("out main\n"); fflush(stdout);        
        pthread_mutex_unlock( &count_mutex );
        
        //double w = 0.0;
		for (i = 0; i < numAgents; i++) {			
			//printf("%i(%i; %.3lf) ", i, agents[i]->destination, agents[i]->progress);
			if (agents[i]->destination == -1) {
				//printf("no destination: %i\n", agents[i]->destination);
				inc = 0.0;
			}
			else {
				//factor = 1.0 - ((double)x[agents[i]->source][agents[i]->destination] / (double)cap[agents[i]->source][agents[i]->destination]);
				//if (factor < 0.1)
				//	factor = 0.1;
                factor = 0.9 * (1.0 / (1.0 + pow (2.718281828, (0.25 *((double)x[agents[i]->source][agents[i]->destination] - (double)cap[agents[i]->source][agents[i]->destination] )))))+0.1;
                
				speed = (vel[agents[i]->source][agents[i]->destination] + agents[i]->velocity_adjust) * factor;
				//printf("speed = %lf; factor = %lf\n", speed, factor); 
				inc = (mtime / distances[agents[i]->source][agents[i]->destination]) * (speed / 75.0);
			}
			
			int passedThru = agents[i]->update(agents[i]->progress + inc, mtime);
			throughCount += passedThru;
            
            GOrevenue += /*(agents[i]->worth / 25.0) * */passedThru;

			
			if (agents[i]->progress <= 0.00000001) {
				agents[i]->choose(world);
                
			}
            
		}
				
		updateAgents(agents, numAgents, throughCount);
		
	}
    
    usleep(50000);
    done = true;
    
    // update the high score list
    for (i = 0; i < 5; i++) {
        if (currentScore > highScores[i]) {
            if (i == 0) 
                strcpy(highScoreMessage, "Congratulations! You got the highest score");
            else if (i == 1)
                sprintf(highScoreMessage, "Congratulations!  You got the %ind highest score\n", i+1);
            else if (i == 2)
                sprintf(highScoreMessage, "Congratulations!  You got the %ird highest score\n", i+1);
            else
                sprintf(highScoreMessage, "Congratulations!  You got the %ith highest score\n", i+1);
            
            char fname[1024];
            sprintf(fname, "highscore_%i.txt", gameLength);
            FILE *fp = fopen(fname, "w");
            int count = 0;
            for (i = 0; i < 5; i++) {
                if ((currentScore > highScores[count]) && (count == i))
                    fprintf(fp, "%s\t%lf\n", yourPseudoname, currentScore);
                else {
                    fprintf(fp, "%s\t%lf\n", highNames[count], highScores[count]);
                    count ++;
                }
            }
            fclose(fp);
            
            highScoreFlag = true;
            
            break;
        }
    }
    
    double simpleWorth = 0.0;
    int simpleCount = 0;
    double advancedWorth = 0.0;
    int advancedCount = 0;
    
    for (i = 0; i < numAgents; i++) {
        if (i < ((percentLearning / 100.0) * numAgents)) {
            advancedWorth += agents[i]->worth;
            advancedCount ++;
        }
        else  {
            simpleWorth += agents[i]->worth;
            simpleCount ++;
        }

        delete agents[i];
    }
    
    printf("average advanced: %lf (%i)\n", advancedWorth / advancedCount, advancedCount);
    printf("average simple: %lf (%i)\n", simpleWorth / simpleCount, simpleCount);    
}

void readGame(char *fname) {
	char fnombre[1024];
	sprintf(fnombre, "..//games//%s.txt", fname);
	FILE *fp = fopen(fnombre, "r");
	
	fscanf(fp, "%i", &numWorldNodes);
	int i, j;
	int numlinks, links[4], linkTypes[4];
	int velocity[4], capacity[4];
	double posx, posy;
	char l[20];
	for (i = 0; i < numWorldNodes; i++) {
		fscanf(fp, "%lf %lf %i", &posx, &posy, &numlinks);
		for (j = 0; j < numlinks; j++) {
			fscanf(fp, "%s", l);
			if (l[0] == 's')
				linkTypes[j] = STRAIGHT;
			else if (l[0] == 't')
				linkTypes[j] = TWO_WAY;
			else if (l[0] == 'b')
				linkTypes[j] = BELOW;
			else {
				printf("don't recognize link type: %c\n", l[0]);
				exit(1);
			}
			links[j] = atoi(l+1);
			
			int c = 1;
			while (l[c] != '_')
				c++;
			c ++;
			velocity[j] = atoi(l+c);
			
			while (l[c] != '_')
				c++;
			c ++;
			capacity[j] = atoi(l+c);
			
			vel[i][links[j]] = velocity[j];
			cap[i][links[j]] = capacity[j];	
		}
		for (j = numlinks; j < 4; j++)
			links[j] = linkTypes[j] = -1;
		
		world[i] = new Node(posx, posy, links, linkTypes, velocity, capacity);
	}
	fscanf(fp, "%i", &theGoalNode);
	
	double x, y;
	for (i = 0; i < numWorldNodes; i++) {
		for (j = 0; j < numWorldNodes; j++) {
			x = world[i]->posx - world[j]->posx;
			y = world[i]->posy - world[j]->posy;
			distances[i][j] = sqrt(x * x + y * y);
		}
	}
	
	fclose(fp);
}

void readParameters(int argc, char *argv[]) {
    
	if (argc >= 2)
		readGame(argv[1]);
	else {
		printf("error: no data file provided\n");
		exit(1);
	}
	if (argc >= 3)
		numAgents = atoi(argv[2]);
    if (argc >= 4)
        gameLength = atoi(argv[3]);
    if (argc >= 5) {
        percentLearning = atoi(argv[4]);
        printf("percent learning agents = %i\n", percentLearning);
    }
    
    if (argc >= 6) {
        if (!strcmp(argv[5], "yes"))
            tollLimitations = true;
        else
            tollLimitations = false;
    }
	
    
    if (argc >= 7){
        strcpy(yourPseudoname, argv[6]);
	}
    else
        strcpy(yourPseudoname, "Blaw");
		
    double lambda;
	agents = new Agent*[numAgents];
	int i;
    
    double given[4] = {0.6, 0.6, 0.8, 0.6};// check
	for (i = 0; i < numAgents; i++) {
        
        lambda = 0.6 - 0.4 * (rand() / (double)RAND_MAX);
        
        if (i < ((percentLearning / 100.0) * numAgents)) {
            agents[i] = new Dijkstra(rand() % numWorldNodes, numWorldNodes, distances, vel, theGoalNode, lambda, given, gameLength);
        }
        else {
            agents[i] = new Static(rand() % numWorldNodes, numWorldNodes, distances, vel, theGoalNode, lambda, given, gameLength);
        }
        
        	}
    
    initAgentsCopy(percentLearning, numAgents);

	updateAgents(agents, numAgents, 0);
    
    // read the high score file
    char fname[1024];
    sprintf(fname, "highscore_%i.txt", gameLength);
    FILE *fp = fopen(fname, "r");
    if (fp != NULL) {
        for (i = 0; i < 5; i++) {
            fscanf(fp, "%s", highNames[i]);
            fscanf(fp, "%lf", &highScores[i]);
        }
        fclose(fp);
    }
    else {
        for (i = 0; i < 5; i++) {
            strcpy(highNames[i], "Unknown");
            highScores[i] = -99999;
        }
    }
}



