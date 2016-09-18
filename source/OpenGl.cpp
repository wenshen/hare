#include "defs.h"
#include "node.h"
#include "Agent.h"
#include "Dijkstra.h"
#include "randomAgent.h"
#include "static.h"


#define BORDER      90
#define DATA_AREA   200

#define BLOCKADE_COST   0

typedef struct {
    double x, y;
} Point;


void copyWorld(Node *_world[100], int _numWorldNodes);
void Resize(int width, int height);
void SpecialKeys(int key, int x, int y);
void RegularKeys(unsigned char key, int x, int y);
void DrawText(GLint x, GLint y, char* s, GLfloat r, GLfloat g, GLfloat b);
void DrawText12(GLint x, GLint y, char* s, GLfloat r, GLfloat g, GLfloat b);
void DrawText24(GLint x, GLint y, char* s, GLfloat r, GLfloat g, GLfloat b);
void CALLBACK Display();
void CALLBACK DrawWorld();
void DrawAgents();
void DrawNodeLabel(GLfloat x, GLfloat y, char *s, GLfloat r, GLfloat g, GLfloat b);
std::string numToLetter(int n);
void DrawGraph();
void DrawLinkStatus();
void DrawLinkStatus2();
void DrawInfoStatus();
void DrawTolls();
void DrawData();
void FillBox(int x, int y, int width, int height);
void OutlineBox(int x, int y, int width, int height);
void drawCircle(int x, int y, int r);
void drawPartialCircle(int x, int y, double pi_i, double pi_f, int r);
void drawCircleOutline(int x, int y, int r);
void drawArrow(double x, double y, double degree, int size);
void myMouse(int button,int state,int x,int y);

void Controls();
void Controls2();

void initCounts();

int WIDTH, HEIGHT;


// keep copies of the world and agents
Node worldCopy[100];
int numWorldNodesCopy = 0;
int highx = -99999, lowx = 99999, highy = -99999, lowy = 99999;

Agent **agentCopy;
int numAgentCopies = 0;
int throughCountCopy = 0;
int recentCount[30], oldCount;
int oldTime = 0;
int agentCountNums[100][100] = {0};
int throughNumD = 0;


unsigned long long secNum = 0;
double temp_timer1;
unsigned long long secCount = 0;
double temp_timer2;

struct timeval empieza;
double empiezaConv;

static float xx = 0.0f;
static float yy = 0.0f;

static float red = 0.4f;
static float green = 0.4f;
static float blue = 0.4f;

bool onleft;

int winid;

extern double GOrevenue, tCosts, Wealth;
extern int gameLength;

double *throughputHistory;
double *slideaveHistory;
double revenueHistory[30], lastRevenueRead, currentRevAve = 0.0;
double costsHistory[30], currentCostAve = 0.0;
double currentScore;

extern int running_model_type;
extern double highScores[5];
extern char highNames[5][1024];

extern bool done, highScoreFlag;
extern char highScoreMessage[1024], yourPseudoname[1024];
extern pthread_mutex_t count_mutex;
extern int sitters[4];

int totalTollChanges = 0, tollChangesHistory[30];
double *tollAveHistory;

extern FILE *fpCON;

double tollChangeFund = 30.0, tollChangeInc = 0.7;

extern bool tollLimitations;

// ****************************************** //
//											  //
//				OPEN_GL STUFF				  //
//											  //
// ****************************************** //
void initGl(int argc, char *argv[], const char *title, Node *_world[100], int _numWorldNodes) {
    done = false;
    if (!tollLimitations)
        tollChangeFund = 999999;
    
    throughputHistory = new double[gameLength * 12+1];
    slideaveHistory = new double[gameLength * 12+1];
    tollAveHistory = new double[gameLength * 12+1];
    int i;
    //printf("length = %i\n", gameLength * 6 + 1);
    for (i = 0; i < (gameLength * 12+1); i++) {
        throughputHistory[i] = 999999;
        slideaveHistory[i] = 999999;
        tollAveHistory[i] = 999999;
    }
    for (i = 0; i < 30; i++) {
        revenueHistory[i] = 999999;
        tollChangesHistory[i] = 0;
    }
    
	copyWorld(_world, _numWorldNodes);
    
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	WIDTH = 2 * BORDER + (highx - lowx) * 200;//wwidth * SQUARE + 2 * BORDERX;
   	HEIGHT = 2 * BORDER + (highy - lowy) * 120;//wheight * SQUARE + 2 * (BORDERY + 20) + REWARD_SPACE;
	
    
	initCounts();
	
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(WIDTH, HEIGHT+DATA_AREA);
    
	winid = glutCreateWindow(title);
    
	glutReshapeFunc(Resize);
	glutDisplayFunc(DrawWorld);	
	glutIdleFunc(DrawWorld);
	
	glutSpecialFunc(SpecialKeys);
	glutKeyboardFunc(RegularKeys);
	glutMouseFunc(myMouse);
    
	glutMainLoop();
}

void initCounts() {
	gettimeofday(&empieza, NULL);
    
    empiezaConv = empieza.tv_sec + (empieza.tv_usec / 1000000.0);
    lastRevenueRead = empiezaConv;
    
	oldTime = 0;

	int i;
	for (i = 0; i < 30; i++) {
		recentCount[i] = 0;
	}
}

void copyWorld(Node *_world[100], int _numWorldNodes) {
	numWorldNodesCopy = _numWorldNodes;
	int i, j;
	for (i = 0; i < numWorldNodesCopy; i++) {
		worldCopy[i].posx = _world[i]->posx;
		worldCopy[i].posy = _world[i]->posy;
		
		if (worldCopy[i].posx > highx)
			highx = worldCopy[i].posx;
		if (worldCopy[i].posx < lowx)
			lowx = worldCopy[i].posx;
		
		if (worldCopy[i].posy > highy)
			highy = worldCopy[i].posy;
		if (worldCopy[i].posy < lowy)
			lowy = worldCopy[i].posy;
		
		worldCopy[i].numLinks = _world[i]->numLinks;
		for (j = 0; j < 4; j++) {
			worldCopy[i].links[j] = _world[i]->links[j];
			worldCopy[i].linkTypes[j] = _world[i]->linkTypes[j];
            worldCopy[i].linkStatus[j] = _world[i]->linkStatus[j];
            worldCopy[i].infoStatus[j] = _world[i]->infoStatus[j];
            worldCopy[i].congested[j] = _world[i]->congested[j];      
            worldCopy[i].timeCongested[j] = _world[i]->timeCongested[j];
            worldCopy[i].toll[j] = _world[i]->toll[j] / 2;
			worldCopy[i].capacity[j] = _world[i]->capacity[j];
            
            worldCopy[i].quo[j] = _world[i]->quo[j];
            worldCopy[i].lastChange[j] = _world[i]->lastChange[j];
		}
	}
}

void initAgentsCopy(int percentLearning, int _numAgents) {
	int i;
	
    agentCopy = new Agent*[_numAgents];
    for (i = 0; i < _numAgents; i++) {
        if (i < ((percentLearning / 100.0) * _numAgents))
            agentCopy[i] = new Dijkstra();
        else
            agentCopy[i] = new Static();
    }
}

void updateAgents(Agent **_agents, int _numAgents, int _throughCount) {
	int i;
	numAgentCopies = _numAgents;
	throughCountCopy = _throughCount;
		
	struct timeval now;
	double seconds, useconds, elapsed;
	gettimeofday(&now, NULL);
	seconds  = now.tv_sec  - empieza.tv_sec;
	useconds = now.tv_usec - empieza.tv_usec;
	elapsed = seconds + (useconds / 1000000.0);
	
	if ((elapsed - oldTime) >= 1.0) {
		for (i = 0; i < 29; i++) {
			recentCount[i] = recentCount[i+1];
		}
		recentCount[29] = _throughCount - oldCount;
		oldCount = _throughCount;
		oldTime = (int)elapsed;
        pthread_mutex_lock( &count_mutex );
        tollChangeFund += tollChangeInc;
        pthread_mutex_unlock( &count_mutex );
	}
	
	for (i = 0; i < numAgentCopies; i++) {
				agentCopy[i]->copyAgent(_agents[i]);
	}
}

void CALLBACK Display() {
	DrawWorld();
}

void CALLBACK DrawWorld() {
	glPushMatrix();

	glClearColor(0.3f, 0.7f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
	DrawGraph();
	DrawAgents();
	DrawLinkStatus2();
	
    DrawTolls();
    
    DrawData();

	glPopMatrix();
	glFlush();
	glutSwapBuffers();
    
}


void DrawNodeLabel(GLfloat x, GLfloat y, char *s, GLfloat r, GLfloat g, GLfloat b) {
    char *cha;
    glColor3f(r,g,b);
	glRasterPos2f(x+1, y);
	glDisable(GL_LIGHTING);
	
    for (cha=s; *cha; cha++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *cha);
    }
}

std::string numToLetter(int n){
    if(n >= 1 && n <= 26){
		std::string str[26] = {"A","B", "C", "D", "E", "F","G","H","I","J","K","L","M","N","O",
			"P","Q","R","S","T","U","V","W","X","Y","Z"};
        return str[n-1];
    }else{
        return "";
    }
}

void DrawGraph() {
	int i, j;
	
	double pi = 3.141593;
	double cx, cy, cx2, cy2, slope, degree, ax, ay;
	double scaley = (HEIGHT - (2 * BORDER)) / (double)(highy - lowy);
	double scalex = (WIDTH - (2 * BORDER)) / (double)(highx - lowx);
		double radius = 25 * (scalex / 200.0);
	if (radius > 40)
		radius = 40;
	for (i = 0; i < numWorldNodesCopy; i++) {
		cx = BORDER + scalex * worldCopy[i].posx;
		cy = BORDER + DATA_AREA + scaley * worldCopy[i].posy;
        
		for (j = 0; j < worldCopy[i].numLinks; j++) {
			cx2 = BORDER + scalex * worldCopy[worldCopy[i].links[j]].posx;
			cy2 = BORDER + DATA_AREA + scaley * worldCopy[worldCopy[i].links[j]].posy;
			
			if ((worldCopy[i].linkTypes[j] == STRAIGHT) || (worldCopy[i].linkTypes[j] == TWO_WAY)) {
				if ((worldCopy[i].linkTypes[j] == TWO_WAY)) {// && (cx == cx2)) {
					if (cy < cy2) {
						cx = cx + 10 * (scalex / 200.0);
						cx2 = cx2 + 10 * (scalex / 200.0);
					}
					else {
						cx = cx - 10 * (scalex / 200.0);
						cx2 = cx2 - 10 * (scalex / 200.0);
					}
				}
            
				
                float width = worldCopy[i].capacity[j] / 3.0;
                glLineWidth(width);	
                if (worldCopy[i].linkStatus[j])
                    glColor3f(0.75, 0.75, 0.6);
                else
                    glColor3f(0.505, 0.725, 0.45);
				glBegin(GL_LINES);
					glVertex2i(cx, cy);
					glVertex2i(cx2, cy2);
				glEnd();
                
                glLineWidth(2.0);
                    glColor3f(0.0, 0.0, 0.0);
                    glLineStipple(1, 0xAA00);
                    glEnable(GL_LINE_STIPPLE);
                    glBegin(GL_LINES);
                        glVertex2i(cx, cy);
                        glVertex2i(cx2, cy2);
                    glEnd();
                    glDisable(GL_LINE_STIPPLE);
                
                
				if (cx2 == cx) {
					if (cy2 > cy) {
						degree = pi / 2.0;
						ax = cx;
						ay = cy2 - (radius);
					}
					else {
						degree = 3 * pi / 2.0;
						ax = cx;
						ay = cy2 + (radius);
					}
				}
				else {
					degree = atan2(cy2 - cy, cx2 - cx);
					ax = -(radius) * cos(degree) + cx2;
					ay = -(radius) * sin(degree) + cy2;
				}
				
			}
			else {
                glLineWidth(30);	
                float width = 15;
				glColor3f(0.75, 0.75, 0.6);
				glBegin(GL_LINES);
					glVertex2i(cx, cy);
					glVertex2i(cx, (BORDER / 2) - width + DATA_AREA);
					glVertex2i(cx, (BORDER / 2) + DATA_AREA);
					glVertex2i(cx2-width, BORDER / 2 + DATA_AREA);
					glVertex2i(cx2, (BORDER / 2) - width + DATA_AREA);				
					glVertex2i(cx2, cy2);
				glEnd();
				
                glLineWidth(1.0);
                if (worldCopy[i].linkStatus[j]) {
                    glColor3f(0.0, 0.0, 0.0);
                    glLineStipple(1, 0xAA00);
                    glEnable(GL_LINE_STIPPLE);
                    glBegin(GL_LINES);
					glVertex2i(cx, cy);
					glVertex2i(cx, (BORDER / 2) + DATA_AREA);
					glVertex2i(cx, (BORDER / 2) + DATA_AREA);
					glVertex2i(cx2, (BORDER / 2) + DATA_AREA);
					glVertex2i(cx2, (BORDER / 2) + DATA_AREA);				
					glVertex2i(cx2, cy2);
                    glEnd();
                    glDisable(GL_LINE_STIPPLE);
                }
			}
		}
        glLineWidth(1.0);
	}
	
	for (i = 0; i < numWorldNodesCopy; i++) {
		cx = BORDER + scalex * worldCopy[i].posx;
		cy = BORDER + DATA_AREA + scaley * worldCopy[i].posy;
		
        glColor3f(0.75, 0.75, 0.6);
		drawCircle(cx, cy, radius);
        if (i == 3)
            glColor3f(0.75, 0.5, 0.2);
        else
            glColor3f(0.3, 0.7, 0.3);
		
		drawCircle(cx, cy, 13 * (scalex / 200.0));
		
		char buf0[4];
		
		char *node_id =  new char[(numToLetter(i+1)).length() +1];
		strcpy(node_id, (numToLetter(i+1)).c_str());
		
        if (node_id!="") {
            sprintf(buf0, "%s",node_id);
            DrawNodeLabel(cx-radius/4.0, cy-radius/4.0, node_id,0.0, 0.0, 0.0); // add a label for each node.
        }else{
            perror ("error occurred!!!");
        }
		delete [] node_id;
		
	}
}

void DrawData() {
	secNum ++;
    
    glColor3f(0.7f, 0.7f, 0.7f);
    FillBox(WIDTH / 2, DATA_AREA / 2, WIDTH, DATA_AREA);
    
    
    // the clock
    int yval = HEIGHT + DATA_AREA - 24;
    int sum, i;
    int minutes, seconds;
    double elapsed;
    
    if (!done) {
        struct timeval now;
        gettimeofday(&now, NULL);
        elapsed = now.tv_sec + (now.tv_usec / 1000000.0);
        
        sum = 0;
        for (i = 0; i < 30; i++)
            sum += recentCount[i];
        
        if ((((int)(elapsed - empiezaConv)) % 5) == 0) {
            int index = (int)(elapsed - empiezaConv) / 5;
            if (throughputHistory[index] > 999998) {
                throughputHistory[index] = throughCountCopy / (elapsed - empiezaConv);
                slideaveHistory[index] = sum / 30;
                tollAveHistory[index] = (totalTollChanges - tollChangesHistory[0]) / 30.0;
                if (tollAveHistory[index] > 5.0)
                    tollAveHistory[index] = 5.0;
            }
        }
    }
    else
        elapsed = empiezaConv + (gameLength*60.0);
    
    minutes = ((int)(elapsed - empiezaConv)) / 60;
    seconds = ((int)(elapsed - empiezaConv)) % 60;

    char buf[1024];
    if ((minutes < 10) && (seconds < 10))
        sprintf(buf, "0%i:0%i", minutes, seconds);
    else if ((minutes >= 10) && (seconds < 10))
        sprintf(buf, "%i:0%i", minutes, seconds);
    else if ((minutes < 10) && (seconds >= 10))
        sprintf(buf, "0%i:%i", minutes, seconds);
    else if ((minutes >= 10) && (seconds >= 10))
        sprintf(buf, "%i:%i", minutes, seconds);
    
    
    glColor3f(0.7f, 0.7f, 0.7f);
    FillBox(WIDTH - 43, yval, 70, 32);
    glColor3f(0.2f, 0.2f, 0.2f);
    OutlineBox(WIDTH - 43, yval, 70, 32);    
	DrawText24(WIDTH - 70, yval-9, buf, 0.0f, 0.0f, 0.0f);
    
    // Score list
    glColor3f(0.9f, 0.9f, 0.9f);
    FillBox((WIDTH / 4) / 2 + (3 * WIDTH / 4), DATA_AREA / 2, (WIDTH / 4) - 10, DATA_AREA - 10);    
    glColor3f(0.0f, 0.0f, 0.0f);
    OutlineBox((WIDTH / 4) / 2 + (3 * WIDTH / 4), DATA_AREA / 2, (WIDTH / 4) - 10, DATA_AREA - 10);
    
    int lx = (3 * WIDTH / 4) + (((WIDTH - 1380) / 4.0) / 2.0);
    
    for (i = 0; i < 5; i++) {
        if (!strcmp(highNames[i], "Unknown"))
            continue;
        
        sprintf(buf, "%i.  %s\n", i+1, highNames[i]);
        DrawText(lx + 70, DATA_AREA - (57 + i * 22), buf, 0.0f, 0.0f, 0.0f);
        
        sprintf(buf, "%.2lf\n", highScores[i]);
        if (highScores[i] >= 9.995)
            DrawText(lx + 230, DATA_AREA - (57 + i * 22), buf, 0.0f, 0.0f, 0.0f);
        else
            DrawText(lx + 240, DATA_AREA - (57 + i * 22), buf, 0.0f, 0.0f, 0.0f);        
    }
    
    sprintf(buf, "Your Score:");
    DrawText24(lx + 30, 15, buf, 0.5f, 0.0f, 0.0f);
    
    double val = (GOrevenue - tCosts) / (elapsed - empiezaConv);
    currentScore = val;
    if ((val >= 9.99) || (val < 0)) {
        sprintf(buf, "$ %.2lf", val);
        DrawText24(lx + 188, 15, buf, 0.5f, 0.0f, 0.0f);
    }
    else {
        sprintf(buf, "$   %.2lf", val);
        DrawText24(lx + 188, 15, buf, 0.5f, 0.0f, 0.0f);  //200
    }
    sprintf(buf, "/ sec");
    DrawText24(lx + 270, 15, buf, 0.0f, 0.0f, 0.0f);
    
    FillBox((3 * WIDTH / 4) + (WIDTH/8), 40, WIDTH / 4 - 30, 2);
    FillBox((3 * WIDTH / 4) + (WIDTH/8), DATA_AREA - 35, WIDTH / 4 - 30, 2);
    
    sprintf(buf, "High Scores");
    DrawText(lx + 120, DATA_AREA - 30, buf, 0.0f, 0.0f, 0.7f);
    
    
    // STATUS: (1) revenue (2) costs (3) Ave. agent wealth
    glColor3f(0.9f, 0.9f, 0.9f);
    FillBox((WIDTH / 4) / 2 + (WIDTH / 2), DATA_AREA / 2, (WIDTH / 4) - 10, DATA_AREA - 10);    
    glColor3f(0.0f, 0.0f, 0.0f);
    OutlineBox((WIDTH / 4) / 2 + (WIDTH / 2), DATA_AREA / 2, (WIDTH / 4) - 10, DATA_AREA - 10);

    double d = elapsed - empiezaConv;
    if (((d - (int)d) < 0.05) && ((elapsed - lastRevenueRead) > 0.8)) {
        lastRevenueRead = elapsed;
        
        if (revenueHistory[0] > 999998) {
            currentRevAve = GOrevenue / 30.0;
            currentCostAve = tCosts / 30.0;
        }
        else {
            currentRevAve = (GOrevenue - revenueHistory[0]) / 30.0;
            currentCostAve = (tCosts - costsHistory[0]) / 30.0;
        }
        
            for (i = 0; i < 29; i++) {
            revenueHistory[i] = revenueHistory[i+1];
            costsHistory[i] = costsHistory[i+1];
            tollChangesHistory[i] = tollChangesHistory[i+1];
        }
        revenueHistory[29] = GOrevenue;
        costsHistory[29] = tCosts;
        tollChangesHistory[29] = totalTollChanges;
    }
    
    double incx, inc;
    
    if (tollLimitations) {
        lx = WIDTH / 2;
        sprintf(buf, "Toll Change Funds");
        DrawText(lx + WIDTH/8 - 75, DATA_AREA - 30, buf, 0.0f, 0.5f, 0.0f);
        
        glColor3f(0.8f, 0.8f, 0.8f);
        FillBox(lx + (WIDTH / 8) - 20, (DATA_AREA / 2) - 14, 14, DATA_AREA - 70);
        
        glColor3f(0.0f, 0.0f, 0.7f);
        int vr;
        if (tollChangeFund > 200)
            vr = (DATA_AREA - 70);
        else
            vr = (tollChangeFund / 200.0) * (DATA_AREA - 70);
        FillBox(lx + (WIDTH / 8) - 20, 22 + vr / 2, 14, vr);    
        
        glColor3f(0.0f, 0.0f, 0.0f);
        OutlineBox(lx + (WIDTH / 8) - 20, (DATA_AREA / 2) - 14, 14, DATA_AREA - 70);
        
        
        sprintf(buf, "$%.2lf", tollChangeFund / 100.0);
        DrawText24(lx + (WIDTH / 8 - 5), 22 + vr - 9, buf, 0.0f, 0.0f, 0.5f);
    }
    else {
        lx = WIDTH / 2;
        sprintf(buf, "Toll Changes");
        DrawText(lx + 25, DATA_AREA - 30, buf, 0.0f, 0.5f, 0.0f);

        sprintf(buf, "Ave: %.2lf", totalTollChanges / (elapsed - empiezaConv));
        DrawText(lx + 145, DATA_AREA - 30, buf, 0.7f, 0.0f, 0.0f);
        
        sprintf(buf, "(cents per second)");
        DrawText12(lx + 230, DATA_AREA - 30, buf, 0.0f, 0.0f, 0.0f);
        
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex2i(lx+30, 30);
        glVertex2i(lx+30, DATA_AREA - 40);
        glVertex2i(lx+30, 30);
        glVertex2i(lx+(WIDTH / 4) - 30, 30);
        glEnd();
        
        double inc = (DATA_AREA-70.0) / 5.0;
        for (i = 0; i <= 5; i++) {
            sprintf(buf, "%i", i);
            if (i >= 10)
                DrawText12(lx + 13, (int)(25 + inc * i), buf, 0.0f, 0.0f, 0.0f);
            else
                DrawText12(lx + 20, (int)(26 + inc * i), buf, 0.0f, 0.0f, 0.0f);
        }
        
        double incx = ((WIDTH/4 - 60) / gameLength);
        for (i = 0; i <= gameLength; i+=5) {
            sprintf(buf, "%i", i);
            DrawText12(lx + 27 + i * incx, 18, buf, 0.0f, 0.0f, 0.0f);
        }    
        
        glLineWidth(2.0);
        incx = (WIDTH/4 - 60) / (double)(gameLength * 12);
        glColor3f(0.0f, 0.5f, 0.0f);
        glBegin(GL_LINES);
        for (i = 1; i < gameLength * 12 + 1; i++) {
            if (tollAveHistory[i] == 999999) {
                break;
            }
            glVertex2i(lx + 30 + ((i-1)*incx), 30 + tollAveHistory[i-1] * inc);
            glVertex2i(lx + 30 + (i*incx), 30 + tollAveHistory[i] * inc);
        }
        glEnd();
        glLineWidth(1.0);
    }    
    
    lx = 0;
    
    glColor3f(0.9f, 0.9f, 0.9f);
    FillBox((WIDTH / 2) / 2, DATA_AREA / 2, (WIDTH / 2) - 10, DATA_AREA - 10);
    glColor3f(0.0f, 0.0f, 0.0f);
    OutlineBox((WIDTH / 2) / 2, DATA_AREA / 2, (WIDTH / 2) - 10, DATA_AREA - 10);
    
    sprintf(buf, "Throughput");
    DrawText(lx + 35, DATA_AREA - 30, buf, 0.0f, 0.0f, 0.7f);

    sprintf(buf, "Overall average:");
    DrawText(lx + 160, DATA_AREA - 30, buf, 0.7f, 0.0f, 0.0f);
    
	
	throughNumD = throughCountCopy;
	
    sprintf(buf, "%.1lf", throughCountCopy / (elapsed - empiezaConv));
    if ((throughCountCopy / (elapsed - empiezaConv)) >= 9.95)
        DrawText(lx + 295, DATA_AREA - 30, buf, 0.7f, 0.0f, 0.0f);
    else
        DrawText(lx + 305, DATA_AREA - 30, buf, 0.7f, 0.0f, 0.0f);
    
    sprintf(buf, "veh/sec");
    DrawText12(lx + 335, DATA_AREA - 30, buf, 0.0f, 0.0f, 0.0f);
    
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2i(lx+30, 30);
    glVertex2i(lx+30, DATA_AREA - 40);
    glVertex2i(lx+30, 30);
    glVertex2i(lx+(WIDTH / 2) - 30, 30);
    glEnd();
    
    inc = (DATA_AREA-70.0) / 5.0;
    for (i = 0; i <= 25; i+=5) {
        sprintf(buf, "%i", i);
        if (i >= 10)
            DrawText12(lx + 13, (int)(25 + inc * (i/5)), buf, 0.0f, 0.0f, 0.0f);
        else
            DrawText12(lx + 20, (int)(26 + inc * (i/5)), buf, 0.0f, 0.0f, 0.0f);
    }

    incx = ((WIDTH/2 - 60) / gameLength);
    for (i = 0; i <= gameLength; i++) {
        sprintf(buf, "%i", i);
            DrawText12(lx + 27 + i * incx, 18, buf, 0.0f, 0.0f, 0.0f);
    }    
    
    glLineWidth(2.0);
    incx = (WIDTH/2 - 60) / (double)(gameLength * 12);
    glColor3f(0.7f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    for (i = 1; i < gameLength * 12 + 1; i++) {
        if (throughputHistory[i] == 999999)
            break;
        glVertex2i(lx + 30 + ((i-1)*incx), 30 + throughputHistory[i-1] * inc/5.0);
        glVertex2i(lx + 30 + (i*incx), 30 + throughputHistory[i] * inc/5.0);
    }
    glEnd();
    glLineWidth(1.0);
    
    
    // Throughput (running average)
    lx += WIDTH / 4;
    sprintf(buf, "Sliding average:");
    DrawText(lx + 60, DATA_AREA - 30, buf, 0.0f, 0.5f, 0.0f);

    sprintf(buf, "%.1lf", sum / 30.0);
    if ((sum/30) >= 9.95)
        DrawText(lx + 193, DATA_AREA - 30, buf, 0.0f, 0.5f, 0.0f);
    else
        DrawText(lx + 203, DATA_AREA - 30, buf, 0.0f, 0.5f, 0.0f);
    
    sprintf(buf, "veh/sec");
    DrawText12(lx + 233, DATA_AREA - 30, buf, 0.0f, 0.0f, 0.0f);
    

    lx -= WIDTH / 4;
    glLineWidth(2.0);
    incx = (WIDTH/2 - 60) / (double)(gameLength * 12);
    glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_LINES);
    for (i = 1; i < gameLength * 12 + 1; i++) {
        if (slideaveHistory[i] == 999999)
            break;
        glVertex2i(lx + 30 + ((i-1)*incx), 30 + slideaveHistory[i-1] * inc/5.0);
        glVertex2i(lx + 30 + (i*incx), 30 + slideaveHistory[i] * inc/5.0);
    }
    glEnd();
    glLineWidth(1.0);
	

    if (highScoreFlag) {
        glColor3f(0.8f, 0.8f, 0.8f);
        int cxm = WIDTH / 2;
        int cym = (HEIGHT+DATA_AREA) / 2;
        int w = 420, l = 160;
        FillBox(cxm, cym, w, l);
        glColor3f(0.7f, 0.0f, 0.0f);
        glLineWidth(2.0);
        OutlineBox(cxm, cym, w - 6, l - 6);
        glLineWidth(1.0);        
        
        DrawText(cxm - w/2 + 10, cym + l / 2 - 28, highScoreMessage, 0.0f, 0.0f, 0.0f);
        
        char fname[1024];
        sprintf(fname, "highscore_%i.txt", gameLength);
        FILE *fp = fopen(fname, "r");
        double val;
        for (i = 0; i < 5; i++) {
            fscanf(fp, "%s", fname);
            fscanf(fp, "%lf", &val);
            
            if (!strcmp(fname, yourPseudoname)) {
                sprintf(buf, "%i.  %s\n", i+1, fname);
                DrawText(cxm - 110, cym + l / 2 - 32 - ((i+1) * 22), buf, 0.8f, 0.0f, 0.0f);
                sprintf(buf, "%.2lf\n", val);
                if (val >= 9.995)
                    DrawText(cxm + 70, cym + l / 2 - 32 - ((i+1) * 22), buf, 0.8f, 0.0f, 0.0f);
                else
                    DrawText(cxm + 70, cym + l / 2 - 32 - ((i+1) * 22), buf, 0.8f, 0.0f, 0.0f);        
            }
            else {
                sprintf(buf, "%i.  %s\n", i+1, fname);
                DrawText(cxm - 110, cym + l / 2 - 32 - ((i+1) * 22), buf, 0.0f, 0.0f, 0.8f);
                sprintf(buf, "%.2lf\n", val);
                if (val >= 9.995)
                    DrawText(cxm + 70, cym + l / 2 - 32 - ((i+1) * 22), buf, 0.0f, 0.0f, 0.8f);
                else
                    DrawText(cxm + 70, cym + l / 2 - 32 - ((i+1) * 22), buf, 0.0f, 0.0f, 0.8f);        
            }
        }
        fclose(fp);
    }
    else if (done) {
        glColor3f(0.8f, 0.8f, 0.8f);
        int cxm = WIDTH / 2;
        int cym = (HEIGHT+DATA_AREA) / 2;
        int w = 200, l = 100;
        FillBox(cxm, cym, w, l);
        glColor3f(0.7f, 0.0f, 0.0f);
        glLineWidth(2.0);
        OutlineBox(cxm, cym, w - 6, l - 6);
        glLineWidth(1.0);        
        
        strcpy(buf, "Game Over");
        DrawText(cxm - 48, cym -5, buf, 0.0f, 0.0f, 0.0f); 
    }
    
}

void DrawAgents() {
	int i, j;
	
	double scaley = (HEIGHT - (2 * BORDER)) / (double)(highy - lowy);
	double scalex = (WIDTH - (2 * BORDER)) / (double)(highx - lowx);
	double cx, cy, cx2, cy2, ax, ay; 
	double radius = 5 * (scalex / 200.0);	
	for (i = 0; i < numAgentCopies; i++) {
		j = -1;		
		for (j = 0; j < 4; j++) {
			if (worldCopy[agentCopy[i]->source].links[j] == agentCopy[i]->destination)
				break;
		}
		
		if ((j < 0) || (agentCopy[i]->destination < 0)) {
			ax = BORDER + scalex * worldCopy[agentCopy[i]->source].posx;
			ay = BORDER + DATA_AREA + scaley * worldCopy[agentCopy[i]->source].posy;			
		}
		else if ((worldCopy[agentCopy[i]->source].linkTypes[j] == STRAIGHT) || (worldCopy[agentCopy[i]->source].linkTypes[j] == TWO_WAY)) {
			cx = BORDER + scalex * worldCopy[agentCopy[i]->source].posx;
			cy = BORDER + DATA_AREA + scaley * worldCopy[agentCopy[i]->source].posy;
			if (agentCopy[i]->destination >= 0) {
				cx2 = BORDER + scalex * worldCopy[agentCopy[i]->destination].posx;
				cy2 = BORDER + DATA_AREA + scaley * worldCopy[agentCopy[i]->destination].posy;
				if ((worldCopy[agentCopy[i]->source].linkTypes[j] == TWO_WAY)) {// && (cx == cx2)) {
					if (cy < cy2) {
						cx = cx + 10 * (scalex / 200.0);
						cx2 = cx2 + 10 * (scalex / 200.0);
					}
					else {
						cx = cx - 10 * (scalex / 200.0);
						cx2 = cx2 - 10 * (scalex / 200.0);
					}
				}
				
				ax = cx * (1.0 - agentCopy[i]->progress) + cx2 * agentCopy[i]->progress;
				ay = cy * (1.0 - agentCopy[i]->progress) + cy2 * agentCopy[i]->progress;
                

                double width = worldCopy[agentCopy[i]->source].capacity[j]/5.0;                
                double mag = sqrt((cx2 - cx) * (cx2 - cx) + (cy2 - cy) * (cy2 - cy));
                
                ax = ax + (agentCopy[i]->center - 0.5) * width * (fabs(cy2 - cy) / mag);
                ay = ay + (agentCopy[i]->center - 0.5) * width * (fabs(cx2 - cx) / mag);
			}
			else {
				ax = cx;
				ay = cy;
			}
		}
		else {
			cx = BORDER + scalex * worldCopy[agentCopy[i]->source].posx;
			cy = BORDER + DATA_AREA + scaley * worldCopy[agentCopy[i]->source].posy;
			cx2 = BORDER + scalex * worldCopy[agentCopy[i]->destination].posx;
			cy2 = BORDER + DATA_AREA + scaley * worldCopy[agentCopy[i]->destination].posy;
			double len1 = (cy - ((BORDER / 2) + DATA_AREA));
			double len2 = fabs(cx - cx2);
			double len3 = cy2 - ((BORDER / 2) + DATA_AREA);
			double len = len1 + len2 + len3;
			double spot = len * agentCopy[i]->progress;
			
			if (spot < len1) {
				ax = cx;
				ay = cy * (1.0 - (spot / len1)) + (((BORDER / 2) + DATA_AREA) * (spot / len1));
			}
			else if (spot < (len1 + len2)) {
				ax = cx * (1.0 - ((spot - len1) / len2)) + cx2 * ((spot - len1) / len2);
				ay = ((BORDER / 2) + DATA_AREA);
			}
			else {
				ax = cx2;
				ay = ((BORDER / 2) + DATA_AREA) * (1.0 - ((spot - len1 - len2) / len3)) + cy2 * ((spot - len1 - len2) / len3);
			}
		}
        
        glColor3f(agentCopy[i]->r, agentCopy[i]->g, agentCopy[i]->b);
		drawCircle(ax, ay, radius);
        glColor3f(0.5f, 0.5f, 0.5f);
		drawCircleOutline(ax, ay, radius);
	}
}

void drawBlockade(int start, int end) {
    int cx, cy;
	double scaley = (HEIGHT - (2 * BORDER)) / (double)(highy - lowy);
	double scalex = (WIDTH - (2 * BORDER)) / (double)(highx - lowx);
    
    glColor3f(0.7f, 0.0f, 0.0f);

    double rise = worldCopy[end].posy - worldCopy[start].posy;
    double run = worldCopy[end].posx - worldCopy[start].posx;
    
    double invslope = -run / rise;
        
    cx = BORDER + scalex * ((0.9 * worldCopy[start].posx + 0.1 * worldCopy[end].posx));
    cy = BORDER + DATA_AREA + scaley * ((0.9 * worldCopy[start].posy + 0.1 * worldCopy[end].posy));
    if ((start == 1) && (end == 2))
        cx -= 10;
    if ((start == 2) && (end == 1))
        cx += 10;
    
    double w = 18.0;
    double x = sqrt((w * w) / ((invslope * invslope) + 1));
    
    double x0 = -x;
    double y0 = x0 * invslope;
    double x1 = x;
    double y1 = x1 * invslope;
    
    glLineWidth(10);
    glBegin(GL_LINES);
    glVertex2i((int)x0+cx,(int)y0+cy);
    glVertex2i((int)x1+cx,(int)y1+cy);
    glEnd();
}

void DrawLinkStatus() {
	int i, j, k, count;
	int cx, cy, linkNum;
	double scaley = (HEIGHT - (2 * BORDER)) / (double)(highy - lowy);
	double scalex = (WIDTH - (2 * BORDER)) / (double)(highx - lowx);

	for (i = 0; i < numWorldNodesCopy; i++) {
		for (j = 0; j < worldCopy[i].numLinks; j++) {
            if (!(worldCopy[i].linkStatus[j])) {
                drawBlockade(i, worldCopy[i].links[j]);
            }
        }
    }
    
    int w = 10, h;
    
    double d, coef;
    
    glLineWidth(1.0);
	for (i = 0; i < numWorldNodesCopy-1; i++) {
		for (j = 0; j < worldCopy[i].numLinks; j++) {
			linkNum = worldCopy[i].links[j];
			cx = BORDER + ((worldCopy[i].posx + worldCopy[linkNum].posx) / 2.0) * scalex;
			if (worldCopy[i].linkTypes[j] == TWO_WAY) {				
				if (worldCopy[i].posy < worldCopy[linkNum].posy) {
                    cx = BORDER + ((0.4 * worldCopy[i].posx + 0.6 * worldCopy[linkNum].posx)) * scalex + 45;
                    cy = BORDER + DATA_AREA + ((0.4 * worldCopy[i].posy + 0.6 * worldCopy[linkNum].posy)) * scaley;
				}
				else {
                    cx = BORDER + ((0.36 * worldCopy[i].posx + 0.64 * worldCopy[linkNum].posx)) * scalex - 45;
                    cy = BORDER + DATA_AREA + ((0.36 * worldCopy[i].posy + 0.64 * worldCopy[linkNum].posy)) * scaley;
				}
			}
			else {
                coef = 0.5;// - (0.05 * (25.0 / (d*d)));
                cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley + 15;
			}
			
			count = 0;
			for (k = 0; k < numAgentCopies; k++) {
				if ((agentCopy[k]->source == i) && (agentCopy[k]->destination == linkNum))
					count ++;
			}
			
            h = worldCopy[i].capacity[j];
            glColor3f(0.9f, 0.9f, 0.9f);
            FillBox(cx,cy+h/2,w,h);
                        
            glColor4f(1.0f, 0.0f, 0.0f, 0.2f);
            FillBox(cx,cy+(13.0*h/16.0),w,(3.0*h/8.0)+2);
            
            glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
            FillBox(cx,cy+((3.0*h/8.0)/2.0),w,(3.0*h/8.0));
            
            glColor4f(1.0f, 1.0f, 0.0f, 0.2f);
            FillBox(cx,cy+h/2,w,h/4);
           
            h = count;
            if (h > (worldCopy[i].capacity[j]*2))
                h = worldCopy[i].capacity[j]*2;
            glColor4f(1.0f, 0.0f, 0.0f, 0.75f);
            FillBox(cx,cy+h/4,w-4,h/2);
            
            glColor3f(0.0f, 0.0f, 0.0f);
            h = worldCopy[i].capacity[j];
            glBegin(GL_LINES);
            glVertex2i(cx-w/2,cy+h/2);
            glVertex2i(cx+w/2,cy+h/2);
            glEnd();
		}
	}
}

void DrawLinkStatus2() {
	secCount++;
	int i, j, k, count;

	int cx, cy, linkNum;
	double scaley = (HEIGHT - (2 * BORDER)) / (double)(highy - lowy);
	double scalex = (WIDTH - (2 * BORDER)) / (double)(highx - lowx);
    char buf[1024];
    
    int w = 10, h;
    
    double coef;
    double destx, desty, theta;
    
    double diffx, diffy, slope, d;
    
    glLineWidth(1.0);	
	
	for (i = 0; i < numWorldNodesCopy-1; i++) {
		for (j = 0; j < worldCopy[i].numLinks; j++) {			
			agentCountNums[i][j] = 0;
		}
	}

		
	for (i = 0; i < numWorldNodesCopy-1; i++) {
		for (j = 0; j < worldCopy[i].numLinks; j++) {
			linkNum = worldCopy[i].links[j];
			cx = BORDER + ((worldCopy[i].posx + worldCopy[linkNum].posx) / 2.0) * scalex;
			if (worldCopy[i].linkTypes[j] == TWO_WAY) {				
				if (worldCopy[i].posy < worldCopy[linkNum].posy) {
                    coef = 0.4;
                    d = WIDTH / 25.0;//45.0;
                    cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                    cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                    diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                    diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                    slope = diffy / diffx;
                    coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                    cy += -diffx * coef;
                    cx += diffy * coef;
				}
				else {
                    coef = 0.4;
                    d = WIDTH / 25.0;//45.0;
                    cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                    cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                    diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                    diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                    slope = diffy / diffx;
                    coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                    cy += -diffx * coef;
                    cx += diffy * coef;
				}
			}
			else {
                coef = 0.55;
                d = WIDTH / 42.0;//45.0;
                cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                slope = diffy / diffx;
                coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                cy -= -diffx * coef;
                cx -= diffy * coef;
			}
			
			count = 0;
			for (k = 0; k < numAgentCopies; k++) {
				if ((agentCopy[k]->source == i) && (agentCopy[k]->destination == linkNum))
					count ++;
			}
			agentCountNums[i][j] = count;

            glColor3f(0.0f, 0.0f, 0.0f);
            drawPartialCircle(cx, cy, 0, 3.1415926, 33);
            glColor3f(0.7f, 0.0f, 0.0f);
            drawPartialCircle(cx, cy, 0, 3.0 * 3.1415926 / 8.0, 32);
            glColor3f(0.7f, 0.7f, 0.0f);
            drawPartialCircle(cx, cy, 3.0 * 3.1415926 / 8.0, 5.0 * 3.1415926 / 8.0, 32);
            glColor3f(0.0f, 0.5f, 0.0f);
            drawPartialCircle(cx, cy, 5.0 * 3.1415926 / 8.0, 3.1415926, 32);
            glColor3f(0.4f, 0.4f, 0.4f);
            drawPartialCircle(cx, cy, 0, 3.1415926, 20);
            glColor3f(0.7f, 0.7f, 0.7f);
            drawPartialCircle(cx, cy, 0, 3.1415926, 19);
			
			

            sprintf(buf, "%i", count);
            if (count < 10)
                DrawText12(cx-3, cy+3, buf, 0.0, 0.0, 0.9);
            else if (count < 100)
                DrawText12(cx-7, cy+3, buf, 0.0, 0.0, 0.9);
            else
                DrawText12(cx-11, cy+3, buf, 0.0, 0.0, 0.9);
            
            sprintf(buf, "%i", worldCopy[i].capacity[j]);
            DrawText12(cx - 7, cy + 21, buf, 0.0, 0.0, 0.0);
            
            if (count > (2.0 * worldCopy[i].capacity[j]))
                count = (2.0 * worldCopy[i].capacity[j]);
            theta = 3.1415926 * (((2.0 * worldCopy[i].capacity[j]) - count) / (2.0 * worldCopy[i].capacity[j]));
            
            double destx = 25.0 * cos(theta) + cx;
            double desty = 25.0 * sin(theta) + cy;
            
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(2.0);
            glBegin(GL_LINES);
            glVertex2i(cx, cy);
            glVertex2i(destx, desty);
            glEnd();         
            drawArrow(destx, desty, theta, 8.0);
            glLineWidth(1.0);            
		}
	}
	

		
}

void DrawTolls() {
	int i, j;
	int cx, cy, linkNum;
    char buf[1024];
	double scaley = (HEIGHT - (2 * BORDER)) / (double)(highy - lowy);
	double scalex = (WIDTH - (2 * BORDER)) / (double)(highx - lowx);
        
    int w = 10, h;
    
    double coef;
    double diffx, diffy, slope, d;
    glLineWidth(1.0);
	for (i = 0; i < numWorldNodesCopy-1; i++) {
		for (j = 0; j < worldCopy[i].numLinks; j++) {
			linkNum = worldCopy[i].links[j];
			if (worldCopy[i].linkTypes[j] == TWO_WAY) {
				if (worldCopy[i].posy < worldCopy[linkNum].posy) {
                    coef = 0.6;
                    d = WIDTH / 25.0;//45.0;
                    cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                    cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                    diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                    diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                    slope = diffy / diffx;
                    coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                    cy += -diffx * coef;
                    cx += diffy * coef;
                    //cx = BORDER + ((0.60 * worldCopy[i].posx + 0.4 * worldCopy[linkNum].posx)) * scalex + 60;
                    //cy = BORDER + DATA_AREA + ((0.60 * worldCopy[i].posy + 0.4 * worldCopy[linkNum].posy)) * scaley;
				}
				else {
                    coef = 0.6;
                    d = WIDTH / 25.0;//45.0;
                    cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                    cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                    diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                    diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                    slope = diffy / diffx;
                    coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                    cy += -diffx * coef;
                    cx += diffy * coef;
                    //cx = BORDER + ((0.60 * worldCopy[i].posx + 0.40 * worldCopy[linkNum].posx)) * scalex - 58;
                    //cy = BORDER + DATA_AREA + ((0.60 * worldCopy[i].posy + 0.40 * worldCopy[linkNum].posy)) * scaley;
				}
			}
			else {
                coef = 0.55;
                d = WIDTH / 27.0;//45.0;
                cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                slope = diffy / diffx;
                coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                cy += -diffx * coef;
                cx += diffy * coef;
			}
			
            glColor3f(0.9f, 0.9f, 0.9f);
            FillBox(cx, cy, 53, 25);

            if (abs(worldCopy[i].toll[j]) > 9) {
                sprintf(buf, "0.%i", abs(worldCopy[i].toll[j]));
                DrawText(cx-24, cy-7, buf, 0.0, 0.0, 0.9);
            }
            else {
                sprintf(buf, "0.0%i", abs(worldCopy[i].toll[j]));
                DrawText(cx-24, cy-7, buf, 0.0, 0.0, 0.9);
            }
            
            glColor3f(0.0f, 0.0f, 0.0f);
            FillBox(cx, cy + 19, 53, 15);
            
            sprintf(buf, "TOLL");
            DrawText12(cx-17, cy+14, buf, 0.9f, 0.9f, 0.9f);
            
            glColor3f(0.8f, 0.8f, 0.8f);
            FillBox(cx+19, cy, 14, 25);

            glColor3f(0.5f, 0.5f, 0.5f);
            OutlineBox(cx+19, cy+5, 13, 12);
            OutlineBox(cx+19, cy-6, 13, 12);

            glBegin(GL_LINES);
            glVertex2i(cx+16,cy+4);
            glVertex2i(cx+20,cy+8);
            glVertex2i(cx+20,cy+7);
            glVertex2i(cx+23,cy+4);
            glVertex2i(cx+16,cy-4);
            glVertex2i(cx+20,cy-8);
            glVertex2i(cx+20,cy-7);
            glVertex2i(cx+23,cy-4);
            glEnd();
        }
	}
}

void FillBox(int x, int y, int width, int height) {
	glBegin(GL_POLYGON);
		glVertex2i(x - (width / 2), y + (height / 2));
		glVertex2i(x + (width / 2), y + (height / 2));
		glVertex2i(x + (width / 2), y - (height / 2));
		glVertex2i(x - (width / 2), y - (height / 2));
	glEnd();
}

void OutlineBox(int x, int y, int width, int height) {
	glBegin(GL_LINES);
		glVertex2i(x - (width / 2), y + (height / 2));
		glVertex2i(x + (width / 2), y + (height / 2));

		glVertex2i(x + (width / 2), y + (height / 2));
		glVertex2i(x + (width / 2), y - (height / 2));

		glVertex2i(x + (width / 2), y - (height / 2));
		glVertex2i(x - (width / 2), y - (height / 2));

		glVertex2i(x - (width / 2), y - (height / 2));
		glVertex2i(x - (width / 2), y + (height / 2));
	glEnd();
}

void Resize(int width, int height) {
    WIDTH  = width;
    HEIGHT = height-DATA_AREA;
	
    glViewport(0, 0, width, height);
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLfloat)width, 0.0, (GLfloat)height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void DrawText(GLint x, GLint y, char *s, GLfloat r, GLfloat g, GLfloat b) {
    int lines;
    char* p;
	
	glColor3f(r,g,b);
	glRasterPos2i(x, y);
	for(p = s, lines = 0; *p; p++) {
		if (*p == '\n') {
			lines++;
			glRasterPos2i(x, y-(lines*18));
		}
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
	}
}

void DrawText12(GLint x, GLint y, char *s, GLfloat r, GLfloat g, GLfloat b) {
    int lines;
    char* p;
	
	glColor3f(r,g,b);
	glRasterPos2i(x, y);
	for(p = s, lines = 0; *p; p++) {
		if (*p == '\n') {
			lines++;
			glRasterPos2i(x, y-(lines*18));
		}
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *p);
	}
}

void DrawText24(GLint x, GLint y, char *s, GLfloat r, GLfloat g, GLfloat b) {
    int lines;
    char* p;
	
	glColor3f(r,g,b);
	glRasterPos2i(x, y);
	for(p = s, lines = 0; *p; p++) {
		if (*p == '\n') {
			lines++;
			glRasterPos2i(x, y-(lines*18));
		}
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *p);
	}
}

void SpecialKeys(int key, int x, int y) {
	//printf("special key hit: %i\n", key);
	switch (key) {
		case GLUT_KEY_UP: break;
		case GLUT_KEY_DOWN: break;
		case GLUT_KEY_RIGHT: break;
		case GLUT_KEY_LEFT: break;
	}
}

void RegularKeys(unsigned char key, int x, int y) {
	//printf("regular key hit: %c\n", key);
	switch (key) {
		case 'h': 
			printf("Entering human-control mode\n");
			//hmnControl = true;
			break;
		default: 
			//printf("Entering automated-control mode\n");
			//hmnControl = false; 
			break;
	}
}

void drawCircle(int x, int y, int r) {
	double ang, inc = 3.1415926 / 50;
	
	glBegin(GL_POLYGON);
	for (ang = 0; ang < ((2.0 * 3.1425926) + inc); ang += inc) {
		glVertex2f(x + cos(ang) * r, y + sin(ang) * r);
	}
	glEnd();
}

void drawPartialCircle(int x, int y, double pi_i, double pi_f, int r) {
	double ang, inc = (pi_f - pi_i) / 50;
	
	glBegin(GL_POLYGON);
    glVertex2f(x, y);
	for (ang = pi_i; ang < (pi_f); ang += inc) {
		glVertex2f(x + cos(ang) * r, y + sin(ang) * r);
	}
	glEnd();
}



void drawCircleOutline(int x, int y, int r) {
	double ang, inc = 3.1415926 / 50;
	
	glBegin(GL_LINES);
	for (ang = 0; ang < ((2.0 * 3.1425926) + inc); ang += inc) {
		glVertex2f(x + cos(ang) * r, y + sin(ang) * r);
		glVertex2f(x + cos(ang + inc) * r, y + sin(ang + inc) * r);		
	}
	glEnd();
}

void drawArrow(double x, double y, double degree, int size) {
	glColor3f(0.0, 0.0, 0.0);
	
	//drawCircleOutline((int)x, (int)y, 10);
	double pi = 3.141593;
	int ax = (int)(size * cos(degree + (3 * pi / 4.0)) + x);
	int ay = (int)(size * sin(degree + (3 * pi / 4.0)) + y);
	glBegin(GL_LINES);
		glVertex2i(x, y);
		glVertex2i(ax, ay);

	ax = (int)(size * cos(degree - (3 * pi / 4.0)) + x);
	ay = (int)(size * sin(degree - (3 * pi / 4.0)) + y);

        glVertex2i(x, y);
		glVertex2i(ax, ay);
	glEnd();
}


double distanceFromSegment(Point p, Point start, Point end) {
    double slope = (end.y - start.y) / (end.x - start.x);
    double b = start.y - slope * start.x;
    double x0 = (slope * p.y + p.x - slope * b) / (slope * slope + 1);
    double y0 = slope * x0 + b;
    
    if (x0 > p.x) {
        //printf("it's on the left\n");
        onleft = true;
    }
    else {
        //printf("it's on the right\n");
        onleft = false;
    }
    
    return sqrt(((p.x - x0) * (p.x - x0)) + ((p.y - y0) * (p.y - y0)));
}

double onSegment(Point p, Point start, Point end) {
    //printf("Is (%.2lf, %.2lf) between (%.2lf, %.2lf) and (%.2lf, %.2lf)?\n", p.x, p.y, start.x, start.y, end.x, end.y);
    
    double minx = start.x;
    double miny = start.y;
    
    if (minx > end.x)
        minx = end.x;
    if (miny > end.y)
        miny = end.y;

    double maxx = start.x;
    double maxy = start.y;

    if (maxx < end.x)
        maxx = end.x;
    if (maxy < end.y)
        maxy = end.y;
    
    double margin = 0.1;
    if ((p.x > (minx - margin)) && (p.x < (maxx + margin)) && (p.y > (miny - margin)) && (p.y < (maxy + margin)))
        return distanceFromSegment(p, start, end);
    
    return -1.0;
}

void myMouse(int button, int state,int x, int y) {
	if (state == 1) {
        double scaley = (HEIGHT - (2 * BORDER)) / (double)(highy - lowy);
        double scalex = (WIDTH - (2 * BORDER)) / (double)(highx - lowx);        
        
        Point point;
        point.x = (x - BORDER) / scalex;
        point.y = highy - ((y - BORDER) / scaley);
        
		//printf("left button click (%i): %.2lf, %.2lf\n", state, point.x, point.y);
        
        // see if click is on a link (blockades)
        int i, j;
        Point p1, p2;
        bool found;
        double dist;
		int oldToll;
		int newToll;

        // tolls
        double diffx, diffy, slope, d, coef;
        double cx, cy;
        int linkNum;
        int inc;
        for (i = 0; i < numWorldNodesCopy-1; i++) {
            for (j = 0; j < worldCopy[i].numLinks; j++) {
                linkNum = worldCopy[i].links[j];                
                if (worldCopy[i].linkTypes[j] == TWO_WAY) {
                    if (worldCopy[i].posy < worldCopy[linkNum].posy) {
                        coef = 0.6;
                        d = WIDTH / 25.0;//45.0;
                        cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                        cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                        diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                        diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                        slope = diffy / diffx;
                        coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                        cy += -diffx * coef;
                        cx += diffy * coef;
                        //cx = BORDER + ((0.60 * worldCopy[i].posx + 0.4 * worldCopy[linkNum].posx)) * scalex + 60;
                        //cy = BORDER + DATA_AREA + ((0.60 * worldCopy[i].posy + 0.4 * worldCopy[linkNum].posy)) * scaley;
                    }
                    else {
                        coef = 0.6;
                        d = WIDTH / 25.0;//45.0;
                        cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                        cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                        diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                        diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                        slope = diffy / diffx;
                        coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                        cy += -diffx * coef;
                        cx += diffy * coef;
                        //cx = BORDER + ((0.60 * worldCopy[i].posx + 0.40 * worldCopy[linkNum].posx)) * scalex - 58;
                        //cy = BORDER + DATA_AREA + ((0.60 * worldCopy[i].posy + 0.40 * worldCopy[linkNum].posy)) * scaley;
                    }
                }
                else {
                    coef = 0.55;
                    d = WIDTH / 27.0;//45.0;
                    cx = BORDER + (coef * worldCopy[i].posx + (1.0-coef) * worldCopy[linkNum].posx) * scalex;
                    cy = BORDER + DATA_AREA + (coef * worldCopy[i].posy + (1.0-coef) * worldCopy[linkNum].posy) * scaley;// - 65;
                    diffx = (worldCopy[linkNum].posx - worldCopy[i].posx) * scalex;
                    diffy = (worldCopy[linkNum].posy - worldCopy[i].posy) * scaley;
                    slope = diffy / diffx;
                    coef = sqrt((d * d) / (diffy * diffy + diffx * diffx));
                    cy += -diffx * coef;
                    cx += diffy * coef;
                }
                
                diffy = fabs(cy - ((HEIGHT+DATA_AREA) - y));
                diffx = fabs(cx - x);
                
                //printf("diff = %.0lf, %.0lf\n", diffx, diffy);
                
                int before = worldCopy[i].toll[j];
				oldToll = before;
				int newToll = oldToll;
				
                if ((diffx < 27) && (diffy < 15)) {
                    pthread_mutex_lock( &count_mutex );
                    //printf("in gl\n"); fflush(stdout);
                    if (button == GLUT_LEFT_BUTTON) {
                        if (tollChangeFund > 0)
                            inc = 1;
                        else
                            inc = 0;
                    }
                    else {
                        if (tollChangeFund > 4)
                            inc = 5;
                        else
                            inc = 0;
                    }
                    
                    if (inc != 0) {
                        if ((cy < ((HEIGHT+DATA_AREA)-y)) && (worldCopy[i].toll[j] < 99)){
                            worldCopy[i].toll[j] += inc;
                        }else if ((cy > ((HEIGHT+DATA_AREA)-y)) &&  (worldCopy[i].toll[j] > -99)){
                            worldCopy[i].toll[j] -= inc;
                        }
                        if (worldCopy[i].toll[j] > 99){
                            worldCopy[i].toll[j] = 99;
                        }else if (worldCopy[i].toll[j] < 0){//-99)
                            worldCopy[i].toll[j] = 0;
                        }
                        newToll= worldCopy[i].toll[j];
                        
                        
                        // Log info control data
                        //std::stringstream tollControlStream;
                        struct timeval now_t3;
                        gettimeofday(&now_t3, NULL);
                        double elapsed_t3 = now_t3.tv_sec + (now_t3.tv_usec / 1000000.0);
                        double interval3 = elapsed_t3 - empiezaConv;
                        
                        // Time    Node_num  Link_num  oldToll newToll
                        fprintf(fpCON, "%.2lf %i %i %i %i\n", interval3, i, worldCopy[i].links[j], oldToll, newToll);
                        fflush(fpCON);
                        
                        tollChangeFund -= abs(newToll - oldToll);
                    }
                    pthread_mutex_unlock( &count_mutex );
					
                }
                totalTollChanges += abs(worldCopy[i].toll[j] - before);
            }
        }
        
        
	}
	
	Display();
	
}

void Controls() {
    int i, j, k, nextNode;

    struct timeval now;
    gettimeofday(&now, NULL);
    double elapsed = (now.tv_sec + (now.tv_usec / 1000000.0)) - empiezaConv;

    
    for (i = 0; i < numWorldNodesCopy-1; i++) {
        for (j = 0; j < worldCopy[i].numLinks; j++) {
            //printf("%lf\n", worldCopy[i].lastChange[j]);
            if (agentCountNums[i][j] > worldCopy[i].capacity[j]) {
                if (worldCopy[i].toll[j] < worldCopy[i].quo[j] + 5.0) {
                    worldCopy[i].lastChange[j] = elapsed;
                    worldCopy[i].quo[j] += 2;
                }

                double val = worldCopy[i].quo[j] + 5.0 + (elapsed - worldCopy[i].lastChange[j]) / 3.0;
                if (val > 99)
                    val = 99;
                worldCopy[i].toll[j] = (int)val;
            }
            else
                worldCopy[i].toll[j] = worldCopy[i].quo[j];
        }
    }
}


// totalTimeSpentCongested / 10.0 +
// totalTimeUplinksHaveBeenCongested / 30.0 +
// totalTimeCongestedThisTime / 5.0
void Controls2() {
    int i, j, k, nextNode;
    
    struct timeval now;
    gettimeofday(&now, NULL);
    double elapsed = (now.tv_sec + (now.tv_usec / 1000000.0)) - empiezaConv;
    
    
    for (i = 0; i < numWorldNodesCopy-1; i++) {
        for (j = 0; j < worldCopy[i].numLinks; j++) {
            if ((agentCountNums[i][j] > worldCopy[i].capacity[j]) && (!(worldCopy[i].congested[j]))) {
                printf("link %i-%i congested\n", i, worldCopy[i].links[j]);
                worldCopy[i].lastChange[j] = elapsed;
                worldCopy[i].congested[j] = true;
            }
            else if ((agentCountNums[i][j] <= worldCopy[i].capacity[j]) && worldCopy[i].congested[j]) {
                printf("link %i-%i uncongested\n", i, worldCopy[i].links[j]);                
                worldCopy[i].congested[j] = false;
                worldCopy[i].timeCongested[j] += elapsed - worldCopy[i].lastChange[j];
            }
            else if (!(worldCopy[i].congested[j]))
                worldCopy[i].lastChange[j] = elapsed;
        }
    }
    
    double toll;
    for (i = 0; i < numWorldNodesCopy-1; i++) {
        for (j = 0; j < worldCopy[i].numLinks; j++) {
            toll = worldCopy[i].timeCongested[j] / (10 / (25 / gameLength));
            if (worldCopy[i].congested[j]) {
                toll += (elapsed - worldCopy[i].lastChange[j]) / (10 / (25 / gameLength));
                toll += (elapsed - worldCopy[i].lastChange[j]) / (5 / (25 / gameLength));
            }
        
            
            nextNode = worldCopy[i].links[j];
            for (k = 0; k < worldCopy[nextNode].numLinks; k++) {
                toll += worldCopy[nextNode].timeCongested[k] / (30 / (25 / gameLength));
                if (worldCopy[nextNode].congested[k]) {
                    toll += (elapsed - worldCopy[nextNode].lastChange[k]) / (30 / (25 / gameLength));
                }
            }
            
            worldCopy[i].toll[j] = (int)(toll + 0.5);
        }
    }
}

