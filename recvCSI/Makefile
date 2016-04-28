OBJS = csi_fun.o csi_persistence.o main.o
CC = gcc

recv_csi: $(OBJS)
	$(CC) $(OBJS) -o recv_csi -L/usr/local/pgsql/lib -lpq
csi_fun.o: csi_fun.c csi_fun.h
	$(CC) -c csi_fun.c -o csi_fun.o
csi_persistence.o: csi_persistence.c csi_persistence.h
	$(CC) -c -I/usr/include/postgresql/ csi_persistence.c -o csi_persistence.o
main.o: main.c csi_fun.h
	$(CC) -c -I/usr/include/postgresql/ main.c -o main.o
clean: 
	rm -f *.o recv_csi

