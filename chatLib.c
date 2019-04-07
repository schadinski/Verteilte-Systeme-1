#include "chatLib.h"

// gcc -o chatLib.o -c chatLib.c

void recvPeerMsg(int fd)
{
  struct chatPDU* pCurrMsg = malloc(sizeof(struct chatPDU));
  struct sockaddr* peerAddr = malloc(sizeof(struct sockaddr));
  unsigned int peerAddrlen;
  int recvBytes;
    
  peerAddrlen = sizeof(*peerAddr);
  recvBytes = recvfrom(fd, (struct chatPDU*)pCurrMsg, sizeof(*pCurrMsg), 0, peerAddr, &peerAddrlen);
  if(recvBytes < 0)
  {
    perror("recvfrom:");
  }
  
  // cut of \n from name 
  int strLen = strlen(pCurrMsg->name);
  if (pCurrMsg->name[strLen-1] == '\n')
  {
    pCurrMsg->name[strLen-1] = 0;
  }
  
  //printf("typ is %d, name is %s, msg is %s", pCurrMsg->typ, pCurrMsg->name, pCurrMsg->msg);
  switch(pCurrMsg->typ)
  {
    case ENTRY:  printf("%s ist dem Chat beigetreten\n", pCurrMsg->name);
		 break;
    case MSG: 	 printf("%s: %s", pCurrMsg->name, pCurrMsg->msg);
		 break;
    case EXIT:   printf("%s hat den Chat verlassen\n", pCurrMsg->name);
		 break;
    default: 	printf("Error: got message without typ\n");
  }
  //printf("recv %d bytes\n", recvBytes);
  free(pCurrMsg);
}

void sendMsg(int fd, char nickname[13], char* buf2, struct sockaddr_in peerAddr)
{
//    printf("send\n");
  int sendbytes;
  struct chatPDU* pCurrMsg = malloc(sizeof(struct chatPDU));
  
  strncpy(pCurrMsg->msg, buf2, 2096);
  strncpy(pCurrMsg->name, nickname, 13);
  pCurrMsg->typ = MSG;
  sendbytes = sendto(fd, (const struct chatPDU*)pCurrMsg, sizeof(*pCurrMsg), 0, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
  if(sendbytes < 0)
  {
    perror("sendMsg sendto:");
  }
  //printf("send %d bytes\n", sendbytes);
  free(pCurrMsg);
}

void sendEntry(int fd, char nickname[13], struct sockaddr_in peerAddr)
{
//  printf("entry\n");
  int sendbytes;
  struct chatPDU* pEntryMsg = malloc(sizeof(struct chatPDU));
  
  pEntryMsg->typ = ENTRY;
  strncpy(pEntryMsg->name, nickname, 13);
  sendbytes = sendto(fd, (const struct chatPDU*)pEntryMsg, sizeof(*pEntryMsg), 0, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
  if(sendbytes < 0)
  {
    perror("sendEntry sendto:");
  }
  //printf("send %d bytes\n", sendbytes);
  free(pEntryMsg);
}

void sendExit(int fd, char nickname[13], struct sockaddr_in peerAddr)
{
  //  printf("exit\n");
  int sendbytes;
  struct chatPDU* pExitMsg = malloc(sizeof(struct chatPDU));
  pExitMsg->typ = EXIT;
  strncpy(pExitMsg->name, nickname, 13);
  sendbytes = sendto(fd, (const struct chatPDU*)pExitMsg, sizeof(*pExitMsg), 0, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
  if(sendbytes < 0)
  {
    perror("sendExit sendto:");
  }
  //printf("send %d bytes\n", sendbytes);
  free(pExitMsg);
}