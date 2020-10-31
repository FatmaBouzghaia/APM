BIN=apm_MPI apm_OpenMP_file_sharing apm_OpenMP_pattern_sharing apm_OpenMP_pattern_schDynamic

CC=gcc
MPI=mpicc

CFLAG=-fopenmp

all:$(BIN)

apm_OpenMP_file_sharing: src/apm_openMP_file_sharing.c
	$(CC) src/apm_openMP_file_sharing.c -o apm_OpenMP_file_sharing $(CFLAG)

apm_OpenMP_pattern_sharing: src/apm_openMP_pattern_sharing.c
	$(CC) src/apm_openMP_pattern_sharing.c -o apm_OpenMP_pattern_sharing $(CFLAG)

apm_OpenMP_pattern_schDynamic: src/apm_openMP_scheduling_dynamic.c
	$(CC) src/apm_openMP_scheduling_dynamic.c -o apm_OpenMP_pattern_schDynamic $(CFLAG)

apm_MPI: src/apm_mpi.c
	$(MPI) -o apm_MPI src/apm_mpi.c  

clean:
	rm -f $(BIN) 
