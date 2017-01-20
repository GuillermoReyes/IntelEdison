CC=gcc
CCFLAGS=-lmraa -lm

TARGET=GetTemperature
TARGET2=EdisonClient

all: $(TARGET) $(TARGET2)

$(TARGET): $(TARGET).c 
	$(CC) $(TARGET).c $(CCFLAGS) -o  $(TARGET)
      	
$(TARGET2): $(TARGET2).c
	$(CC) $(TARGET2).c $(CCFLAGS) -o $(TARGET2)

dist: 
	tar -czvf EdisonProject.tar.gz Makefile README GetTemperature.c EdisonClient.c

clean:
	rm $(TARGET) $(TARGET2)
