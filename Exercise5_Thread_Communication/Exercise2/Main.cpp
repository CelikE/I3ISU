#include "MsgQueue.h"
#include "Point3D.h"
#include <iostream>
#include "unistd.h" 

using namespace std;

void *Writer(void *data);
void *Reader(void *data);
void handleMsg(Message *msg, unsigned long id);
void Point3DHandler(Point3D* point);

pthread_t Sender, Receiver;

enum IdType {
  IdPoint = 1
};

int main(){
  MsgQueue msgQueue(10);
  
  pthread_create(&Sender, NULL, Writer, (void*) &msgQueue);
  pthread_create(&Receiver, NULL, Reader, (void*) &msgQueue);
 
  pthread_join(Sender, NULL);
  pthread_join(Receiver, NULL);
}

void *Writer(void *data){
  MsgQueue* messageQueue = static_cast<MsgQueue*> (data);
  
  while(true){
   Point3D* point3d=new Point3D;
    point3d->x=(rand() % 1000);
    point3d->y=(rand() % 1000);
    point3d->z=(rand() % 1000);
    messageQueue->send(IdPoint, point3d);
    usleep(1000000);
    }
  return NULL;
}

void *Reader(void *data){
    MsgQueue* messageQueue = static_cast<MsgQueue*> (data);
  
  
  while(true){
    unsigned long id = 0;
    Message* msg = messageQueue->receive(id);
    handleMsg(msg, id);
    delete msg;
    }
  return NULL;
}

void Point3DHandler(Point3D* point){
  cout<<"X:"<< point->x<<" Y:"<< point->y<<" Z:"<< point->z<<"\n";
}

void handleMsg(Message *msg, unsigned long id){
  cout << "Id is: " << id << "\n";
  switch(id) {
    case IdPoint:
    {
      //Static cast
      Point3D* point = static_cast<Point3D*> (msg);
      //Handler
      Point3DHandler(point);
    }
      break;
      
    default:
      cout << "wtf is this?\n";
  }; 
}