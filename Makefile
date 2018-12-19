.PHONY:
	clean 

main:main.cc MemLeaDet.cc
	g++ $^ -o $@ -std=c++11

clean:
	rm main
