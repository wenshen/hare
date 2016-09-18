/*
 *  utility.cpp
 *  Transport
 *
 *  Created by Wen on 4/1/13.
 *  Copyright 2013 __wenshen__. All rights reserved.
 *
 */

#include "utility.h"

extern char yourPseudoname[1024];

void writeData(std::string app_data, std::string file_name ){
	std::ofstream myfile;
    //string filename = "/Users/admin/Desktop/research/games/data_sys.txt";
    //myfile.open(filename);
    //string cur_path = "./";
    //cur_path.append(file_name);
    //char * c_path = (char*)(cur_path.c_str());
	//std::cout << "current directory is : " << c_path << std::endl;
	file_name.append("_");
	std::string pnames(yourPseudoname);
	file_name.append(pnames);
	file_name.append(".txt");
	
	
	
    myfile.open(file_name.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    myfile << app_data<<std::endl;
    myfile.close();
}

void createFile(std::string fname3){
	FILE *file;
    int file_exists;

	std::string pnames(yourPseudoname);
	fname3.append("_");
	fname3.append(pnames);
	fname3.append(".txt");

	
	
    const char * filename=fname3.c_str();
	
	//printf("filename: %s \n", filename);
    /*first check if the file exists...*/
    file=fopen(filename,"r");
    if (file==NULL){ 
		file_exists=0;
	}
    else {
		file_exists=1; 
		fclose(file);
	}
	
    /*...then open it in the appropriate way*/
    if (file_exists==1)
    {
		//printf("file: %s exists!\n", fname);
		file=fopen(filename,"w");
    }else {
        //printf("file: %s does not exist!\n", fname);
        file=fopen(filename,"w+b");
    }
	
    if (file!=NULL)
	{
		//printf ("file: %s opened succesfully!\n", fname);
		fclose(file);
    }
	
}

double gaussrand(double V, double E){
    static double V1, V2, S;
    static int phase = 0;
    double X;
	
    if ( phase == 0 ) {
        do {
            double U1 = (double)rand() / RAND_MAX;
            double U2 = (double)rand() / RAND_MAX;
			
            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);
		
        X = V1 * sqrt(-2 * log(S) / S);
    } else
        X = V2 * sqrt(-2 * log(S) / S);
	
    phase = 1 - phase;
    X = X * V + E;
    return X;
}


std::string getCurrentWorkPath(){
	
    char buffer[1001], buffer1[1001];
    getcwd(buffer1, 1000);
    getcwd( buffer, 1000);
    //std::cout << "Current directory is: " << getcwd( buffer, 1000) << "\n";
    char *pch, *pch1;
    pch = strtok(buffer, "/");
    int count_pch = 0;
    while(pch != NULL){
        //printf("%s\n", pch);
        count_pch ++;
        pch = strtok(NULL, "/");
    }
    //printf("%i\n", count_pch);
	
    char buffer2[1001]="/";
	
    pch1 = strtok(buffer1, "/");
    int count_pch2 = 0;
    while(pch1 != NULL){
        //printf("%s\n", pch1);
        if(count_pch2 < count_pch-4){
            strcat(buffer2, pch1);
            strcat(buffer2, "/");
			
        }
        count_pch2 ++;
        pch1 = strtok(NULL, "/");
    }
	
    //strcat(buffer2, "games/4nodes.txt\0");
    //printf("%s\n", buffer2);
	std::string value = buffer2;
    return value;
}
