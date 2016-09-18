#include "myStatistics.h"

myStatistics::myStatistics() {
	printf("incomplete consumer constructor\n");
	exit(1);
}

myStatistics::myStatistics(int _type, double _param1, double _param2) {
    type = _type;
    numCDFelements = 100;
    if (_type == NORMAL)
        generateNormalCDF(_param1, _param2);
    else
        generateGammaCDF((int)_param1, _param2);
}

myStatistics::~myStatistics() {
    if (type != NORMAL)
        delete gammaCDF;
    else
        delete normalCDF;
}

void myStatistics::generateNormalCDF(double _mu, double _sigma) {
	munormal = _mu;
	sigmanormal = _sigma;
	
	numCDFelements = 100;
	normalCDF = new double[numCDFelements];
	
	int i;
	double x;
	double inc = (6.0 * _sigma) / (numCDFelements - 1);
	double sum = 0.0;
	for (x = _mu - 3.0*_sigma, i = 0; i < numCDFelements; x += inc, i++) {
		normalCDF[i] = (1.0 / (_sigma * sqrt(2.0 * 3.1415926))) * exp(-((x - _mu) * (x - _mu))/(2.0 * _sigma * _sigma));
		sum += normalCDF[i];
	}
	
	for (i = 0; i < numCDFelements; i++) {
		normalCDF[i] /= sum;
		if (i > 0)
			normalCDF[i] += normalCDF[i-1];
	}
}

void myStatistics::generateGammaCDF(int _k, double _theta) {
	kgamma = _k;
	thetagamma = _theta;
	
	numCDFelements = 100;
	gammaCDF = new double[numCDFelements];

	
	int i;
	double x;
	double inc = 20.0 / (numCDFelements - 1);
	double sum = 0.0;

	int f = 1.0;
	for (i = _k-1; i >= 2; i--)
		f *= i;
	
	for (x = 0.0, i = 0; i < numCDFelements; x += inc, i++) {
		gammaCDF[i] = (1.0 / (pow(_theta, _k) * f)) * pow(x, _k - 1) * exp(-1.0 * x / _theta);
		sum += gammaCDF[i];
	}
	
	for (i = 0; i < numCDFelements; i++) {
		gammaCDF[i] /= sum;
		if (i > 0)
			gammaCDF[i] += gammaCDF[i-1];
	}	
}

double myStatistics::getSample() {
    if (type == NORMAL)
        return sampleNormal();
    else
        return sampleGamma();
}

double myStatistics::sampleNormal(){
	int i;
	double num = rand() / (double)RAND_MAX;
	double inc = (6.0 * sigmanormal) / (numCDFelements - 1);
	for (i = 0; i < numCDFelements; i++) {
		if (num <= normalCDF[i]) {
			return (munormal - (3.0 * sigmanormal) + (i * inc));
		}
	}
	
	printf("normal sample not found\n");
	
	return munormal + (3.0 * sigmanormal);
}

double myStatistics::sampleGamma() {
	int i;
	double num = rand() / (double)RAND_MAX;
	double inc = 20.0 / (numCDFelements - 1);
	for (i = 0; i < numCDFelements; i++) {
		if (num <= gammaCDF[i]) {
			return (i * inc);
		}
	}
	
	printf("gamma sample not found\n");
	
	return 20.0;
}

