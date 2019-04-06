#include "header.h"

int main()
{
  int readBytes;
  int events;
  fd_set readset;
  char* nickname; 
  int serverFD = 0;
  int peerFD = 0;
  unsigned int nameLen;
  size_t nameChars = 0;
  unsigned int len;
  
  nickname = malloc( 13*sizeof(char));
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
  peerFD = socket( AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in peerAddr;
  memset(&peerAddr, 0, sizeof(peerAddr));
  peerAddr.sin_family = AF_INET;
  peerAddr.sin_port = htons(3000);
  peerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //peer part end

  //get user input
  printf("Enter name:");
  nameChars = getline(&nickname, &nameLen, stdin);
  //printf("length of name is %d\n", nameChars);
  
  sendEntry(serverFD, nickname, peerAddr);
  
  FD_ZERO(&readset);

   while(1)
   {
     char* buf2 = malloc(2096* sizeof(char));
     FD_SET(serverFD, &readset);
     FD_SET(0, &readset);
     
     events = select(serverFD+1, &readset, 0, 0, 0);
     if(FD_ISSET(serverFD,&readset))
     {    
	recvPeerMsg(serverFD); 
     }
     if(FD_ISSET(STDIN_FILENO,&readset))
     {
       size_t chars = 0;
       chars = getline(&buf2, &len, stdin);
       //printf("chars no %d\n", chars);
       sendMsg(serverFD, nickname, buf2, peerAddr);
     }
     free(buf2);
   } 
 return 0;
}

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
  if(pCurrMsg->typ == ENTRY)
  {
  printf("%s ist dem Chat beigetreten\n", pCurrMsg->name);
  }
  else
  {
  printf("%s: %s", pCurrMsg->name, pCurrMsg->msg);
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
    perror("sendto:");
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
    perror("sendto:");
  }
  //printf("send %d bytes\n", sendbytes);
  free(pEntryMsg);
}