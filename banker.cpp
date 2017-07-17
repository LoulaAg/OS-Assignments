/*
 * banker.cpp
 *
 * Student Name: Spyridoula Angelopoulos
 * Student Number: 10175590
 *
 * Class: CPSC 457 Spring 2017
 * Instructor: Pavol Federl
 *
 * Copyright 2017 University of Calgary. All rights reserved.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include <cstdlib>

using namespace std;

int compare(int *array1, int *array2, int bound){
  for(int i=0; i < bound; i ++){
    //cout<< "Array 1: " << array1[i] << "Array: 2" << array2[i] << endl;
    if(array1[i] > array2[i]){
      return 0;
    }
  }
  return 1;
}

class Banker
{
private:
    int numProc;      // the number of processes
    int numResources; // the number of resources
    int * available;  // number of available instances of each resource
    int ** max;       // the max demand of each process, e.g., max[i][j] = k
                      // means process i needs at most k instances of resource j
    int ** allocation;// the number of resource instances already allocated
    int ** need;      // the number of resource isntances needed by each process

public:

    /* Initializing the vectors and matrixes for the Banker's Algorithm. Takes ownership of
     * all arrays.
     * @param avail  The available vector
     * @param m      The max demand matrix
     * @param alloc  The allocation matrix
     * @param p      The number of processes
     * @param r      The number of resources
     */
    Banker (int * avail, int ** m, int ** alloc, int p, int r) {
        numProc = p;
        numResources = r;

        // Setup the available vector, the max matrix, and the allocation matrix
        available = avail;  //available resources
        max = m;
        allocation = alloc;

        need = new int*[numProc];
        for (int i = 0; i < numProc; i++){
            need[i] = new int[numResources];
          }


    }

    /* Destroy the vectors and matrixes
     */
     ~Banker() {
         numProc = 0;
         numResources = 0;
         // Free all allocated memory space
         delete[] available;
         for (int i = 0; i < numProc; i++)
         {
             delete[] need[i];
             delete[] max[i];
             delete[] allocation[i];
         }
         delete[] need;
         delete[] max;
         delete[] allocation;
     }

    /* Check whether it is safe to grant the request
     * @param pid    The process that is making the request
     * @param req    The request
     * @param sequenceOrReason  The safe execution sequence returned by the algorithm
     * @return Whether granting the request will lead to a safe state.
     */
    bool isSafe (int pid, int * req, string & sequenceOrReason) {
        int **oldalloc= allocation;
        for(int i=0; i< numResources; i++){
          allocation[pid][i]= allocation[pid][i] + req[i];       //add the request to the allocation and check if
        }                                   //the system will be in a safe state

        //STEP 1: finish = false for all processes
        int finish[numProc];
        int sequence[numProc];
        for(int i=0; i< numProc; i++){
          finish[i]= 0; //finish[i]= false
          sequence[numProc]= 0;
        }

        // Initialize the need matrix
        need = new int*[numProc];
        for (int i = 0; i < numProc; i++){    //loop through the processes
            need[i] = new int[numResources];  //intitialize a resource array for each process
            for(int j= 0; j< numResources; j++){  //loop through the resources
              need[i][j]= max[i][j] - allocation[i][j]; //claculate resources needed
            }
        }
        //STEP 2:
        //this needs to never stop
        int s=0;
        int i=0;
        int run =1;
        while(run){   //run until all the processes are done, or non can run
            if(finish[i]== 0 && compare(need[i], available, numResources)){ //check if the process has not finished and can run
                //*(available + (j*sizeof(int)))= *(available + (j*sizeof(int))) + *(allocation + (j*sizeof(int)))
                for(int j=0; j< numResources; j++){
                  available[j]= available[j] + allocation[i][j];    //add the previously allocated resources to the available resources
                  finish[i]= 1;
                }
                if(s < numProc){                //add the process to the sequence
                  sequence[s]= i;
                  cout << "I = " << i << endl;
                  s++;
                }
            }
          //check if all are done or none can run
          int done =0;
          int notenough=0;
          for(int i=0; i < numProc; i++){
            if(finish[i]==0 && !compare(need[i], available, numResources)){
              notenough++;
            }
            if(finish[i] == 1){
              done++;
            }
          }
          if(done == numProc || notenough == numProc){
            run =0;
          }
          //indrecent the index of the proceeses
          i++;
          if (i == numProc){
            i=0;
          }
        }

        for(int i =0; i < numProc; i++){
          //check if all processes has finished
          if(finish[i] ==0){  //if ay of them has not, the state is not safe
            sequenceOrReason= "State not Safe";
            return false; //at least one is false, so return false
          }
        }

        //generate sequence of processes as a string
        std::string str= " ";
        for(int i=0; i< numProc; i++){
          str= str + " P" + std::to_string(sequence[i]);
        }
        sequenceOrReason= str;
      //  sequenceOrReason = "State is Safe";
        return true; //all were true, so return true


      //  sequenceOrReason = "Not implemented yet.";
        //return false;
    }
};

int main (int argc, char * const argv[])
{
    ifstream config;       // Configuration file
    string conffile;       // The configuration file name
    int numProc;           // The number of processes
    int numResources;      // The number of resources
    string sequenceOrReason;       // The execution sequence returned by the Banker's Algorithm
    int i, j, index;       // Indices for the vectors and matrixes
    int pid;               // The ID of the process that is making the request
    string reqStr;         // The request vector in string format

    // Read in the config file name from the commanda-line arguments
    if (argc < 2)
    {
        cout << "Usage: banker <config file>\n";
        return 0;
    }
    else
    {
        conffile = argv[1];
    }

    // Open the file
    config.open(conffile.c_str());

    // Get the number of process and the number of resources
    string line, var, equal;    // strings for parsing a line in the config file
    getline(config, line);
    istringstream iss(line);
    iss >> var >> equal >> numProc;     // Get the number of processes
    iss.clear();

    getline(config, line);
    iss.str(line);
    iss >> var >> equal >> numResources;    // Get the number of resources
    iss.clear();

    // Create the available vector, the max matrix, and the allocation matrix
    // according to the number of processes and the number of resources
    int * available = new int[numResources];
    int ** max = new int*[numProc];
    int ** allocation = new int*[numProc];
    for (int i = 0; i < numProc; i++)
    {
        max[i] = new int[numResources];
        allocation[i] = new int[numResources];
    }

    // Get the available vector
    getline(config, line);
    replace(line.begin(), line.end(), '<', ' ');  // Remove "<" and ">"
    replace(line.begin(), line.end(), '>', ' ');
    iss.str(line);
    iss >> var >> equal;
    for (j = 0; j < numResources; j++)        // Read in the "available" vector
        iss >> available[j];
    iss.clear();

    // Get the max matrix and the allocation matrix
    for (i = 0; i < numProc; i++)
    {
        getline(config, line);
        replace(line.begin(), line.end(), '<', ' ');
        replace(line.begin(), line.end(), '>', ' ');
        iss.str(line);
        iss >> var;
        index = atoi(&var.at(1));            // Get the process ID
        if (index < 0 || index >= numProc)
        {
            cerr << "Invalid process ID: " << var << endl;
            return 0;
        }

        // Get the number of resources allocated to process "index".
        for (j = 0; j < numResources; j++)
            iss >> allocation[index][j];

        // Get the max allocation to process "index".
        for (j = 0; j < numResources; j++)
            iss >> max[index][j];

        iss.clear();
    }
    //printf("Here");
    // Get the request vector
    int * request = new int[numResources];
    getline(config, line);
    reqStr = line.substr(line.find('<'), line.find('>') - line.find('<') + 1);
    replace(line.begin(), line.end(), '<', ' ');
    replace(line.begin(), line.end(), '>', ' ');
    iss.str(line);
    iss >> var >> pid >> equal;
    for (j = 0; j < numResources; j++)          // Read in the "request" vector
        iss >> request[j];
    iss.clear();

    // Check the request using the Banker's algorithm.
    Banker * banker = new Banker(available, max, allocation, numProc, numResources);
    if (banker -> isSafe(pid, request, sequenceOrReason))
        cout << "Grant request " << reqStr << " from P" << pid << ".\n"
             << "Sequence: " << sequenceOrReason << ".\n";
    else
        cout << "Reject request " << reqStr << " from P" << pid << ".\n"
             << "Reason: " << sequenceOrReason << "\n";
}
