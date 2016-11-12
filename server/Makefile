CMP = g++
CLASS = server
MAIN = main
EXEC = myfrmd
FLAGS = -std=c++11

$(EXEC): $(CLASS).o $(MAIN).o
	$(CMP) $(CLASS).o $(MAIN).o -o $(EXEC) $(FLAGS)

$(CLASS).o: $(CLASS).cpp $(CLASS).h
	$(CMP) -c $(CLASS).cpp -o $(CLASS).o $(FLAGS)

$(MAIN).o: $(MAIN).cpp $(CLASS).h
	$(CMP) -c $(MAIN).cpp -o $(MAIN).o $(FLAGS)

clean:
	rm *.o
	rm $(EXEC)

