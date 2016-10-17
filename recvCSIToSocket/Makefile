OBJS = csi_fun.o socket.o sender.o main.o
CC = gcc
recv_csi: $(OBJS)
	$(CC) $(OBJS) -o recv_csi
csi_fun.o: csi_fun.c csi_fun.h
	$(CC) -c csi_fun.c -o csi_fun.o
socket.o: socket.c socket.h
	$(CC) -c socket.c -o socket.o
sender.o: sender.c sender.h
	$(CC) -c sender.c -o sender.o
main.o: main.c csi_fun.h socket.h sender.h
	$(CC) -c main.c -o main.o

clean: 
	rm -f *.o recv_csi

