CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
TARGET = dhaka_routing

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe *.kml *.o

.PHONY: all clean
