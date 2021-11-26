CC = mpic++
EXECS = exec
TOUCH = touch
CLEAN = clean

run90: ${EXECS}
	mpirun -np 90 ./${EXECS} 20

run20: ${EXECS}
	mpirun -np 20 ./${EXECS} 4

exec: main.cpp
	$(CC) -o exec main.cpp
	
clean:
	rm ${EXECS}

