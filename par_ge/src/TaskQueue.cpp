//
//  TaskQueue.cpp
//
//  Created by Anthony Cianfrocco on 9/7/20.
//  Copyright © 2020 Anthony Cianfrocco. All rights reserved.
//

#include <iostream>

#include "TaskQueue.hpp"

TaskQueue::TaskQueue()
{
   mnSize = 0;
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void TaskQueue::enqueue(TaskNode *apNode)
{
   if(mnSize == 0)
   {
      mpHead = std::shared_ptr<TaskNode>(apNode);
   }
   else if(mnSize == 1)
   {
      mpHead->setNext(apNode);
      mpTail = mpHead->getNext();
   }
   else
   {
      mpTail->setNext(apNode);
      mpTail = mpTail->getNext();
   }
   mnSize ++;
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
std::shared_ptr<TaskNode> TaskQueue::dequeue()
{
   std::shared_ptr<TaskNode> lpNode;
 
   if (mnSize == 0)
   {
      std::cout << "Queue is empty!\n";
   }
   else if (mnSize == 1)
   {
      lpNode = mpHead;
      mpHead = std::shared_ptr<TaskNode>(nullptr);
   }
   else
   {
      lpNode = mpHead;
      mpHead = mpHead->getNext();
   }
   
   mnSize --;

   return lpNode;
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
int TaskQueue::getSize() const
{
   return mnSize;
}

/////////////////////////////////////////////////////////////////
// See Header File
/////////////////////////////////////////////////////////////////
void TaskQueue::printQueue() const
{
   std::cout << "Printing the queue...\n\n";
   int lnI = 1;
   std::shared_ptr<TaskNode> lpCurrNode = mpHead;
   while(lpCurrNode != nullptr)
   {
      std::cout << "Node " << lnI << ": " << lpCurrNode->getTime() << "\n";
      
      lpCurrNode = lpCurrNode->getNext();
      lnI ++;  
   }
   std::cout << "\n";
}
