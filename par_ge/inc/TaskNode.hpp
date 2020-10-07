//
//  TaskNode.hpp
//
//  Created by Anthony Cianfrocco on 9/7/20.
//  Copyright © 2020 Anthony Cianfrocco. All rights reserved.
//

#ifndef TaskNode_hpp
#define TaskNode_hpp

#include <stdio.h>
#include <memory>

class TaskNode
{

public:
   TaskNode(TaskNode *apNext, const int anTime);

   /**
    * @brief Set this nodes next node
    *
    * @param apNext the pointer the the next node
    */    
   void setNext(TaskNode *apNext);

   /**
    * @brief Set this nodes time
    * 
    * @param anTime the new time value
    */
   void setTime(const int anTime);

   std::shared_ptr<TaskNode> getNext() const;

   int getTime() const;

private:
   // Shared pointer to this nodes next node
   std::shared_ptr<TaskNode> mpNext;

   // This nodes time 
   int mnTime;

};

#endif /* TaskNode_hpp */
