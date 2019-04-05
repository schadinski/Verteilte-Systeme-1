#include "header.h"

int main()
{
  char* buf2 = malloc(2096* sizeof(char));
  int readBytes;
  int events;
  fd_set readset;
  char nickname[13];
  int serverFD = 0;
  
  //server part
  if ((	serverFD = socket( AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror("socket: ");
  }
  
  struct sockaddr_in localAddr;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(2000);
  localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
      
  int rc = bind(serverFD, (struct sockaddr*)&localAddr, sizeof(localAddr));
  if (rc < 0)
  {
    printf("Error: Bind FD\n");
    perror("bind()");
    rc = 0;
  }
  //server part end
  
  
  //peer part
  int peerFD = socket( AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in peerAddr;
  memset(&peerAddr, 0, sizeof(peerAddr));
  peerAddr.sin_family = AF_INET;
  peerAddr.sin_port = htons(3000);
  peerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //peer part end
  
  printf("Enter name:");
  scanf("%s", nickname);
  
  sendEntry(serverFD, nickname, peerAddr);
  
  FD_ZERO(&readset);

   while(1)
   {
     
//      printf("sizeof buf %d\n", sizeof(buf2));

     FD_SET(serverFD, &readset);
     FD_SET(0, &readset);
     
     events = select(serverFD+1, &readset, 0, 0, 0);
     if(FD_ISSET(serverFD,&readset))
     {    
	recvPeerMsg(serverFD); 
     }
     if(FD_ISSET(STDIN_FILENO,&readset))
     {
//        memset(&buf2, 0, 2096);
	readBytes = read(STDIN_FILENO, buf2, 2096);
	//printf("readBytes %d\n", readBytes);
	//printf("Ich: %s", buf2);
	sendMsg(serverFD, nickname, buf2, peerAddr);
     }
   } 
 return 0;
}

void recvPeerMsg(int fd)
{
  struct chatPDU* pCurrMsg = malloc(sizeof(struct chatPDU));
  //memset(&buf, 0, 2096);
  struct sockaddr peerAddr;
  memset(&peerAddr, 0, sizeof(peerAddr));
  unsigned int peerAddrlen;
  peerAddrlen = sizeof(peerAddr);
  int recvBytes;
  recvBytes = recvfrom(fd, (struct chatPDU*)pCurrMsg, sizeof(*pCurrMsg), 0, &peerAddr, &peerAddrlen);
  if(recvBytes < 0)
  {
    perror("recvfrom:");
  }
  
  if(pCurrMsg->typ == ENTRY)
  {
  printf("%s ist dem Chat beigetreten\n", pCurrMsg->name);
  }
  else
  {
  printf("%s: %s", pCurrMsg->name, pCurrMsg->msg);
  //printf("recv %d bytes\n", recvBytes);
  }
  free(pCurrMsg);
}

void sendMsg(int fd, char nickname[13], char* buf2, struct sockaddr_in peerAddr)
{
  int sendbytes;
  //printf("sizeof buf %d\n", sizeof(buf2));
  struct chatPDU* pCurrMsg = malloc(sizeof(struct chatPDU));
  strncpy(pCurrMsg->msg, buf2, 2096);
  strncpy(pCurrMsg->name, nickname, 13);
  sendbytes = sendto(fd, (const struct chatPDU*)pCurrMsg, sizeof(*pCurrMsg), 0, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
  if(sendbytes < 0)
  {
    perror("sendto:");
  }
 // printf("send %d bytes\n", sendbytes);
   free(pCurrMsg);
}

void sendEntry(int fd, char nickname[13], struct sockaddr_in peerAddr)
{
  int sendbytes;
  struct chatPDU* pEntryMsg = malloc(sizeof(struct chatPDU));
  pEntryMsg->typ = ENTRY;
  strncpy(pEntryMsg->name, nickname, 13);
  sendbytes = sendto(fd, (const struct chatPDU*)pEntryMsg, sizeof(*pEntryMsg), 0, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
  if(sendbytes < 0)
  {
    perror("sendto:");
  }
  //printf("send %d bytes\n", sendbytes);
  free(pEntryMsg);
}