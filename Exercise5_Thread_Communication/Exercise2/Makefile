EXE=MsgQueue
CXX=g++
SOURCES=Main.cpp MsgQueue.cpp
OBJECTS=${SOURCES:.cpp=.o} 
CXXFLAGS=-pthread

all	:	${OBJECTS}
		${CXX} -o ${EXE} ${OBJECTS} ${CXXFLAGS}

%.o	:	%.cpp
		${CXX} -c $^ ${CXXFLAGS}
		
.PHONY	:	clean
clean	:	
		rm -f *.o ${EXE}
.PHONY	:	help
help	:
		@echo all
		@echo clean
		@echo help
