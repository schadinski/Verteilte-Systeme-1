#ifndef MYHEADER_H
#define MYHEADER_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*
static unsigned short ports[10] = { 1050,
				    1100,
				    1150,
				    1200,
				    1250,
				    1300,
				    1350,
				    1400,
				    1450,
				    1500 };
				    */

# define MAXPEERS 2

//[0] = suseVM
//[1] = ubuntuVM HP Laptop
static char* ips[2] = { "192.168.178.71",
  "192.168.216.162"
};


typedef enum{
  ENTRY,
  MSG,
  EXIT
}Typ;

struct chatPDU {
  Typ typ;
  char name[13];
  char msg[2096];
};

void recvPeerMsg(int);
void sendMsg(int, char[13], char*, struct sockaddr_in);
void sendEntry(int, char[13], struct sockaddr_in);
void sendExit(int, char [13], struct sockaddr_in);

#endif /* MYHEADER_H */