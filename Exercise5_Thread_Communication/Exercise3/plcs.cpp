#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include "MsgQueue.h"
using namespace std;

MsgQueue* entryPostQueue = new MsgQueue(100);
MsgQueue* exitPostQueue = new MsgQueue(100);
MsgQueue* carQueue = new MsgQueue(100);
unsigned int numberOfWaitingCarsEntry = 0;
unsigned int numberOfWaitingCarsExit = 0;
bool entryGateOpen = false;
bool exitGateOpen = false;
pthread_mutex_t entryMutex;
pthread_mutex_t exitMutex;

enum{
  CAR_WAITING_TO_ENTER,
  ENTRY_REQUEST,
  ENTRY_OPEN,
  CAR_ENTER,
  ENTRY_CLOSED,
  EXIT_REQUEST,
  EXIT_OPEN,
  CAR_LEAVE,
  EXIT_CLOSED
}; 

bool carHandler(Message* msg, unsigned long id){
  bool running = true;
  switch(id){
    case CAR_WAITING_TO_ENTER:{
      cout << "car waiting to enter" << endl;
      numberOfWaitingCarsEntry++;
      entryPostQueue->send(ENTRY_REQUEST);
      break;
    }
    case ENTRY_OPEN:{
      cout << "car entering" << endl;
      numberOfWaitingCarsEntry--;
      entryPostQueue->send(CAR_ENTER);
      cout << "car inside" << endl;
      numberOfWaitingCarsExit++;
      //wait for some time
      sleep(5);
      exitPostQueue->send(EXIT_REQUEST);
      break;
    }
    case EXIT_OPEN:{
      cout << "car leaving" << endl;
      numberOfWaitingCarsExit--;
      exitPostQueue->send(CAR_LEAVE);
      cout << "car outside" << endl;
      running = false;
      break;
    }
  };
  return running;
}

void entryHandler(Message* msg, unsigned long id){
  switch(id){
    case ENTRY_REQUEST:{
      if(entryGateOpen == true){
	carQueue->send(ENTRY_OPEN);
      }
      else{
	pthread_mutex_lock(&entryMutex);
	entryGateOpen = true;
	pthread_mutex_unlock(&entryMutex);
	carQueue->send(ENTRY_OPEN);
      }
      break;
    }
    case CAR_ENTER:{
      if(numberOfWaitingCarsEntry == 0 && entryGateOpen == true){
	pthread_mutex_lock(&entryMutex);
	entryGateOpen = false;
	pthread_mutex_unlock(&entryMutex);
      }
      else{
	//just let the next car in
      }
      break;
    }
  };  
}

void exitHandler(Message* msg, unsigned long id){
  switch(id){
    case EXIT_REQUEST:{
      if(exitGateOpen == true){
	carQueue->send(EXIT_OPEN);
      }
      else{
	pthread_mutex_lock(&exitMutex);
	exitGateOpen = true;
	pthread_mutex_unlock(&exitMutex);
	carQueue->send(EXIT_OPEN);
      }
      break;
    }
    case CAR_LEAVE:{
      if(numberOfWaitingCarsExit == 0 && exitGateOpen == true){
	pthread_mutex_lock(&exitMutex);
	exitGateOpen = false;
	pthread_mutex_unlock(&exitMutex);
      }
      else{
	//just let the next car leave
      }
      break;
    }
  };
}

void* entryPost(void* notinuse){
  while(true){
    unsigned long id;
    Message* msg = entryPostQueue->receive(id);
    entryHandler(msg, id);
    delete msg;
  }
}

void* exitPost(void* notinuse){
    while(true){
    unsigned long id;
    Message* msg = exitPostQueue->receive(id);
    exitHandler(msg, id);
    delete msg;
  }
}

void* driving(void* notinuse){
  carQueue->send(CAR_WAITING_TO_ENTER);
  bool running = true;
  while(running){
    unsigned long id;
    Message* msg = carQueue->receive(id);
    running = carHandler(msg, id);
    delete msg;
  }
}

int main(int argc, char* argv[]){
  pthread_mutex_init(&entryMutex, NULL);
  pthread_mutex_init(&exitMutex, NULL);
  unsigned int numberOfCars = atoi(argv[1]);
  
  pthread_t entryGuard;
  pthread_t exitGuard;
  pthread_t car[numberOfCars];
  
  pthread_create(&entryGuard, NULL, entryPost, NULL);
  pthread_create(&exitGuard, NULL, exitPost, NULL);
  for(int i = 0; i < numberOfCars; i++){
    pthread_create(&car[i], NULL, driving,NULL);
  }
  void* res;
  for(int i = 0; i < numberOfCars; i++){
    pthread_join(car[i], &res);
  }
  pthread_mutex_destroy(&entryMutex);
  pthread_mutex_destroy(&exitMutex);
  
  return 0;
}