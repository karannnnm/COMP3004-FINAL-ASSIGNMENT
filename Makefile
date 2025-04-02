# all:	test

# test:	test.o Profile.o ProfileManager.o
# 	g++ -o test test.o Profile.o ProfileManager.o

# test.o:	test.cpp Profile.h ProfileManager.h
# 	g++ -c test.cpp

# Profile.o: Profile.cpp Profile.h
# 	g++ -c Profile.cpp

# ProfileManager.o: ProfileManager.cpp ProfileManager.h Profile.h
# 	g++ -c ProfileManager.cpp

# clean:
# 	rm -f test *.o

all: test

test: test.o Profile.o ProfileManager.o BolusCalculator.o
	g++ -o test test.o Profile.o ProfileManager.o BolusCalculator.o

test.o: test.cpp Profile.h ProfileManager.h BolusCalculator.h
	g++ -c test.cpp

Profile.o: Profile.cpp Profile.h
	g++ -c Profile.cpp

ProfileManager.o: ProfileManager.cpp ProfileManager.h Profile.h
	g++ -c ProfileManager.cpp

BolusCalculator.o: BolusCalculator.cpp BolusCalculator.h ProfileManager.h
	g++ -c BolusCalculator.cpp

clean:
	rm -f test *.o
