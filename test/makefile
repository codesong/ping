
CSRCS = ../src/Exception.cpp ../src/Util.cpp ../src/TimeStamp.cpp ../src/Ostream.cpp ../src/Log.cpp ../src/Thread.cpp test_thread.cpp 

CSOBJS=${CSRCS:.cpp=.o}
OBJS=$(CSOBJS)

CPP = g++ -g
TARGET = a.out

all: ${TARGET} 

clean:
	-rm -rf ../src/*.o
	-rm -rf *.o
	-rm -rf a.out

FLAGS = -pthread

$(TARGET):$(CSOBJS)
	$(CPP) $(FLAGS) -o $@ $(CSOBJS) 
	@echo ".................Done building $@................."
							        
%.o:%.cpp
	$(CPP) $(FLAGS) -c -o $@ $<

