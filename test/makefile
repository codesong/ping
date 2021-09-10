
CSRCS = ../src/Exception.cpp ../src/Util.cpp ../src/Timestamp.cpp ../src/Ostream.cpp ../src/Thread.cpp ../src/ThreadPool.cpp \
	../src/LogFile.cpp ../src/Log.cpp ../src/net/Socket.cpp ../src/net/InetAddress.cpp \
	test_inetaddress.cpp
	#test_log.cpp
	#test_timestamp.cpp
	#test_thread.cpp 

CSOBJS=${CSRCS:.cpp=.o}
OBJS=$(CSOBJS)

CPP = g++ -g
TARGET = a.out

all: ${TARGET} 

clean:
	-rm -rf ../src/*.o
	-rm -rf *.o
	-rm -rf ${TARGET}

FLAGS = -pthread

$(TARGET):$(CSOBJS)
	$(CPP) $(FLAGS) -o $@ $(CSOBJS) 
	@echo ".................Done building $@................."
							        
%.o:%.cpp
	$(CPP) $(FLAGS) -c -o $@ $<

