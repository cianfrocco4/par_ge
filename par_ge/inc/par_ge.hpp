//
//  par_ge.hpp
//  par_ge
//
//  Created by Anthony Cianfrocco on 9/2/20.
//  Copyright © 2020 Anthony Cianfrocco. All rights reserved.
//

#ifndef par_ge_hpp
#define par_ge_hpp

#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <vector>

#include "TaskQueue.hpp"
#include "TaskNode.hpp"

class par_ge
{
public:
   par_ge(const int anN, const int anNumThreads);

   /**
    * @brief This function runs the par_ge. It launches the 
    *        number of threads given, then reads and "processes"
    *        the tasks provided in the input file
    */  
   void run();
 
   int getMinAggTime() const;

   int getMaxAggTime() const;

   int getSerialExecTime() const;

   /**
    * @brief This is the function to be used by the threads. It 
    *        "processes" the threads by sleeping the amount of time
    *        each task has assigned to it. Mutexes and locks are used
    *        to protect the TaskQueue data structure and a 
    *        conditional variable is used so that main can inform
    *        the threads when a new task is added to the queue to be 
    *        processed.
    */ 
   void worker();

private:   
   /* MEMBER VARIABLES */
   
   // The size of the vector X
   // NxN is matrix size
   int mnN;

   // The number of threads to launch
   int mnNumThreads;

   // The matrix to use in computations (A)
   std::vector<std::vector<float>> mcMatrix;

   // The vector X
   std::vector<float> mcX;

   // The vector B
   std::vector<float> mcB;

   // The task queue data structure
   std::unique_ptr<TaskQueue> mpQueue;

   // The aggregated thread time map.
   // This maps Thread ID to aggregated time
   std::unordered_map<std::thread::id, int> mcAggTimeMap;

   // The cond var used by main and worker functions
   std::condition_variable mcCondQueueNotEmpty;

   // The mutex used by main and worker functions
   std::mutex mcTaskQueueMutex;

   // The mutex used by update function
   std::mutex mcAggTimeMutex;
  
   // Flag used for reading the input data
   bool mbDoneReadingInput;

   // Flag used for thread processing
   bool mbDoneProcessingInput;

   // Minimum thread aggregated time
   int mnMinAggTime;

   // Maximum thread aggregated time
   int mnMaxAggTime;

   // Calculated serial execution time
   int mnSerialExecTime;

   /* FUNCTIONS */

   /**
    * @brief Init the matrix to size N and fill with
    *        will random values
    */
   void 
   initializeMatrix();

   /**
    * Print the matrix and x and b vectors.
    */
   void
   printMatrix() const;

   /**
    * @brief Reads the supplied input file either:
    *           - Adds a task to the task queue and 
    *             notifies a thread to process it
    *             (mutex and lock are used to modify the 
    *              task queue)
    *           - Sleeps for the given time if input was 'd'
    *
    * @param acInputFileName Name for the input file
    */
   void readInputFile(const std::string acInputFileName);

   /**
    * @brief Calculates the aggregated time for the current thread and
    *        the overall min and max aggregated thread time values.
    *
    * @param anTime The current threads aggregated time
    * @param acId   The current threads id
    */
   void update(const int anTime, std::thread::id acId);
};

#endif /* par_ge_hpp */
