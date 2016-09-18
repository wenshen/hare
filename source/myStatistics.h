#ifndef MYSTATISTICS_H
#define MYSTATISTICS_H

#include "defs.h"

#define NORMAL  0
#define GAMMA   1

class myStatistics {
public:
	myStatistics();
	myStatistics(int _type, double _param1, double _param2);
	~myStatistics();
	
	// computation
	void generateNormalCDF(double _mu, double _sigma);
	void generateGammaCDF(int _k, double _theta);
    double getSample();
	double sampleNormal();
	double sampleGamma();

    int type;
	int numCDFelements;	
	double *normalCDF;
	double munormal, sigmanormal;
	double *gammaCDF;
	double kgamma, thetagamma;
};

#endif
