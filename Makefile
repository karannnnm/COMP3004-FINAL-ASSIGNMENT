all:	test

test:	test.o Profile.o ProfileManager.o
	g++ -o test test.o Profile.o ProfileManager.o

test.o:	test.cpp Profile.h ProfileManager.h
	g++ -c test.cpp

Profile.o: Profile.cpp Profile.h
	g++ -c Profile.cpp

ProfileManager.o: ProfileManager.cpp ProfileManager.h Profile.h
	g++ -c ProfileManager.cpp

clean:
	rm -f test *.o