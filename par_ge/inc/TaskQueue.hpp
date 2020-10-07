//
//  TaskQueue.hpp
//
//  Created by Anthony Cianfrocco on 9/7/20.
//  Copyright © 2020 Anthony Cianfrocco. All rights reserved.
//

#ifndef TaskQueue_hpp
#define TaskQueue_hpp

#include <stdio.h>
#include <memory>
#include "TaskNode.hpp"

class TaskQueue
{

public:
   TaskQueue();
   
   /**
    * @brief Insert a new task at the tail of the queue.
    *        Wraps apNode within a shared pointer.
    *
    * @param apNode the node to insert
    */
   void enqueue(TaskNode *apNode);

   /**
    * @brief Removes a task from the head of the queue
    *        and returns it. The head is updated to the
    *        old head's next node.
    */
   std::shared_ptr<TaskNode> dequeue();

   int getSize() const;

   /**
    * @brief Used for debug. Prints the contents of the queue.
    */
   void printQueue() const;

private:

   // The head of the queue
   std::shared_ptr<TaskNode> mpHead;

   // The tail of the queue
   std::shared_ptr<TaskNode> mpTail;

   // The size of the queue
   int mnSize;
};

#endif /* TaskQueue_hpp */

