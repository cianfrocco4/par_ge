//
//  par_ge.cpp
//  par_ge
//
//  Created by Anthony Cianfrocco on 10/7/20.
//  Copyright © 2020 Anthony Cianfrocco. All rights reserved.
//

#include "par_ge.hpp"

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <chrono>
#include <limits>
#include <math.h>

par_ge::par_ge(const int anN, const int anNumThreads)
{
   mnN = anN;
   mnNumThreads = anNumThreads;
   mbDoneReadingInput = false;
   mbDoneProcessingInput = false;
   mnMinAggTime = std::numeric_limits<int>::max();
   mnMaxAggTime = std::numeric_limits<int>::min();
   mnSerialExecTime = 0;

   initializeMatrix();
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void 
par_ge::initializeMatrix() 
{
    std::cout << "\nInitializing ...\n";
    for (int lnCol = 0; lnCol < mnN; lnCol++) 
    {
        for (int lnRow = 0; lnRow < mnN; lnRow++) 
        {
            mcMatrix[lnRow][lnCol] = (float)rand() / 32768.0;
        }
        mcB[lnCol] = (float)rand() / 32768.0;
        mcX[lnCol] = 0.0;
    }
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void 
par_ge::printMatrix() const 
{
    std::cout << "\nA =\n\t";
    for (int lnRow = 0; lnRow < mnN; lnRow++) 
    {
        for (int lnCol = 0; lnCol < mnN; lnCol++) 
        {
            std::cout << "%5.2f%s", mcMatrix[lnRow][lnCol], (lnCol < mnN-1) ? ", " : ";\n\t";
        }
    }
    std::cout << "\nB = [";
    for (int lnCol = 0; lnCol < mnN; lnCol++) 
    {
        std::cout << "%5.2f%s", mcB[lnCol], (lnCol < mnN-1) ? "; " : "]\n";
    }
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void par_ge::run()
{

   // initialzie matrix
/*
   // create and launch threads
   std::thread lacThreads[anNumThreads];

   for(int i = 0; i < anNumThreads; i++)
   { 
      lacThreads[i] = std::thread(&par_ge::worker, this); 
   }
   
   // init task queue
   mpQueue = std::unique_ptr<TaskQueue>(new TaskQueue());
   
   // read input file  
   readInputFile(acInputFileName);
  
   // let the threads know that input is all read in
   mbDoneReadingInput = true;

   // keep notifying threads until the queue is empty  
   while(!mbDoneProcessingInput)
   {
      mcCondQueueNotEmpty.notify_one();
   }  

   // wait for all threads to finish
   for(int i = 0; i < anNumThreads; i++)
   {
      lacThreads[i].join();  
   }
*/
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void par_ge::readInputFile(const std::string acInputFileName)
{
   std::fstream lcFileStream;
   lcFileStream.open(acInputFileName);

   if(!lcFileStream)
   {
      std::cout << "File name is not valid!\n";
      
      // allow any waiting threads to finish
      mbDoneProcessingInput = true;
   }
   else
   {
      std::string lcStr;
      lcFileStream >> lcStr;

      int lnTime;
      lcFileStream >> lnTime;

      while(!lcFileStream.eof())
      {
         // sum up all times to calculate what would be the serial exec time
         mnSerialExecTime += lnTime;        
         
      if(lcStr == "t")
         {
            // lock the mutex for the queue -- will automatically unlock when it goes out of scope
            std::lock_guard<std::mutex> lcGaurd(mcTaskQueueMutex);

            // add this task to the queue
            mpQueue->enqueue(new TaskNode(nullptr, lnTime));
    
            // notify a thread that the queue is not empty
            mcCondQueueNotEmpty.notify_one();
         }
         else if(lcStr == "d")
         {
            // sleep for the time
            std::this_thread::sleep_for(std::chrono::seconds(lnTime));
         }

         lcFileStream >> lcStr;
         lcFileStream >> lnTime;
      }
   }
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void par_ge::worker()
{
   std::thread::id lcId = std::this_thread::get_id();
   int lnAggregatedTime = 0;

   while(!mbDoneProcessingInput)
   {
      // lock the mutex for the queue -- automatically unlocks when it goes out of scope
      std::unique_lock<std::mutex> lcLock(mcTaskQueueMutex);

      // wait for the TaskQueue condition variable to be notified
      mcCondQueueNotEmpty.wait(lcLock);

      if( (mbDoneReadingInput) && (mpQueue->getSize() == 0) )
      {
         mbDoneProcessingInput = true;
         lcLock.unlock();
      }
      else
      {
         // get the time from the next task in the queue
         const int lnTime = mpQueue->dequeue()->getTime();
         
         lnAggregatedTime += lnTime;

         // manually unlock the lock to allow the next thread to process BEFORE this thread sleeps
         lcLock.unlock();

	 // sleep for the time given to the task
         std::this_thread::sleep_for(std::chrono::seconds(lnTime));
        
         // update the aggregated time vars
         update(lnAggregatedTime, lcId);
      }
   }

   // notify any waiting threads so they can finish
   mcCondQueueNotEmpty.notify_all();
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void par_ge::update(const int anTime, std::thread::id acId)
{
   // lock the mutex for the aggreg time vars -- automatically unlocks when it goes out of scope
   std::unique_lock<std::mutex> lcLock(mcAggTimeMutex);
   
   // Check if this thread is in the map yet
   auto iter = mcAggTimeMap.find(acId);
   if(iter == mcAggTimeMap.end())
   { 
      // Add this thread to the map
      mcAggTimeMap.insert({acId, anTime});
   }
   else
   {
      // Update this thread's value to the new time
      iter->second = anTime;
   }

   // reset the min and max to get their new values accurately
   mnMinAggTime = std::numeric_limits<int>::max();
   mnMaxAggTime = std::numeric_limits<int>::min();

   // loop through the map to get the min and max values
   for(const auto& lnTime : mcAggTimeMap)
   {
      if(lnTime.second < mnMinAggTime)
      {
         mnMinAggTime = lnTime.second;
      }
     
      if(lnTime.second > mnMaxAggTime)
      {
         mnMaxAggTime = lnTime.second;
      }
   }
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
int par_ge::getMinAggTime() const
{
   return mnMinAggTime;
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
int par_ge::getMaxAggTime() const
{
   return mnMaxAggTime;
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
int par_ge::getSerialExecTime() const
{
   return mnSerialExecTime;
}

// Main function for this program.
// Creates the par_ge instance and runs the simulation.
// Outputs the following:
//    <serial_time, parallel_time, min_aggregated_thread_time, max_aggregated_thread_time>
int main(int argc, char* argv[])
{
   if(argc != 3)
   {
      std::cout << "usage: par_ge <dimension_of_matrix> <number_of_threads>\n";
   }
   else
   {
      const int lnMatrixDim = atoi(argv[1]);
      const int lnThreads = atoi(argv[2]);
      
      par_ge lcSim(lnMatrixDim, lnThreads);  
      
   }
/*
   par_ge lcSim;
   
   auto lnStart = std::chrono::high_resolution_clock::now();

   lcSim.run(lcInputFileName, lnThreads);

   auto lnStop = std::chrono::high_resolution_clock::now();

   auto lnDuration = std::chrono::duration_cast<std::chrono::seconds>(lnStop - lnStart);

   std::cout << lcSim.getSerialExecTime() << "," << lnDuration.count() << "," << lcSim.getMinAggTime() << "," << lcSim.getMaxAggTime() << "\n";
*/
}

