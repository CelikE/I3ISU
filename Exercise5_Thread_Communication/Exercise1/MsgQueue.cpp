#include "MsgQueue.h"
#include <iostream>


using namespace std;

MsgQueue::MsgQueue (unsigned long maxSize) : maxSize_(maxSize){
  pthread_mutex_init(&m_, NULL);
  pthread_cond_init(&not_full_, NULL);
  pthread_cond_init(&not_empty_, NULL);
}

void MsgQueue::send(unsigned long id, Message* msg){
  pthread_mutex_lock(&m_);
  while(MsgContainer_.size() >= maxSize_){
   pthread_cond_wait(&not_full_, &m_);
  }
  Item tempItem;
  tempItem.id_ = id;
  tempItem.msg_ = msg;
  
  MsgContainer_.push(tempItem);
  cout<<"pushed message onto stack\n";
  pthread_cond_broadcast(&not_empty_);
  pthread_mutex_unlock(&m_);

}

Message* MsgQueue::receive(unsigned long& id){
  pthread_mutex_lock(&m_);
  
  while(MsgContainer_.empty()){
      cout<<"Waiting for Message\n";
      pthread_cond_wait(&not_empty_, &m_);
  }
  cout<<"Received message.\n";
  Item tempItem = MsgContainer_.front();
  
  MsgContainer_.pop();
  pthread_cond_broadcast(&not_full_);
  pthread_mutex_unlock(&m_);
  
  id = tempItem.id_;
  return tempItem.msg_;
}

MsgQueue::~MsgQueue(){
  pthread_mutex_destroy(&m_);
  pthread_cond_destroy(&not_full_);
  pthread_cond_destroy(&not_empty_);
  
}