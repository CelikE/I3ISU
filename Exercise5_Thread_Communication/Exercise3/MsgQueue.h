#pragma once
#include<queue>
#include<cstdlib>
#include<pthread.h>
#include "Message.h"
 
class MsgQueue{
public:
  MsgQueue(unsigned long maxSize);
  void send(unsigned long id, Message* msg = NULL);
  Message* receive(unsigned long& id);
  ~MsgQueue();
private:
  
  struct Item{
   unsigned long id_;
   Message* msg_;
  };
  
  std::queue<Item> MsgContainer_;
  unsigned long maxSize_;
  
  pthread_mutex_t m_;
  pthread_cond_t not_full_;
  pthread_cond_t not_empty_;
};