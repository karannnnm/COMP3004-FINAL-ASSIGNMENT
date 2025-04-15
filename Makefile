
all: test

test: test.o Profile.o ProfileManager.o BolusCalculator.o ControlIQ.o CGM.o
	g++ -o test test.o Profile.o ProfileManager.o BolusCalculator.o ControlIQ.o CGM.o

test.o: test.cpp Profile.h ProfileManager.h BolusCalculator.h
	g++ -c test.cpp

Profile.o: Profile.cpp Profile.h
	g++ -c Profile.cpp

ProfileManager.o: ProfileManager.cpp ProfileManager.h Profile.h
	g++ -c ProfileManager.cpp

BolusCalculator.o: BolusCalculator.cpp BolusCalculator.h ProfileManager.h
	g++ -c BolusCalculator.cpp

ControlIQ.o: ControlIQ.cpp ControlIQ.h BolusCalculator.h
	g++ -c ControlIQ.cpp

CGM.o: CGM.cpp CGM.h ControlIQ.h
	g++ -c CGM.cpp
clean:
	rm -f test *.o
