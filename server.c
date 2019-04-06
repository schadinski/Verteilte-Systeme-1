#include "chatLib.h"

int main(int argc, char *argv[])
{
  if ( argv[1] == 0 )
    printf("no port specified\n");

//    printf("The following arguments were passed to main(): ");
//    for(i=1; i<argc; i++) printf("%s ", argv[i]);
//    {printf("\n"); 
//    }
  int readBytes;
  int events;
  fd_set readset;
  char* nickname;
  unsigned int nameLen;
  size_t nameChars = 0;
  unsigned int len;
  unsigned int localPort;
  unsigned int peerPorts[9];
  int i;
  
  nickname = malloc( 13*sizeof(char));
  
  //setup port as server and peer ports
  localPort = atoi(argv[1]);
//    printf("localPort is %d\n", localPort); 
  for (i = 0; i < 10; i++)
  {
    if(ports[i] != localPort)
    {
      peerPorts[i] = ports[i];
//      printf("Port %d added to peerPorts\n", peerPorts[i]);
    }
  }
  
  //server part
  int serverFD = socket( AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in localAddr;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(3000);
  localAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
      
  int rc = bind(serverFD, (struct sockaddr*)&localAddr, sizeof(localAddr));
  if (rc < 0)
  {
    printf("Error: Bind 1 FD\n");
    perror("bind()");
    rc = 0;
  }
  //server part end
  
  
  //peer part
  int peerFD = socket( AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in peerAddr;
  memset(&peerAddr, 0, sizeof(peerAddr));
  peerAddr.sin_family = AF_INET;
  peerAddr.sin_port = htons(2000);
  peerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //peer part end
  
  //get user input
  printf("Enter name:");
  nameChars = getline(&nickname, &nameLen, stdin);
//TODO: Errorhandling input == 0
  
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
       getline(&buf2, &len, stdin);
       if(strstr(buf2, "!Exit"))
       {
	 sendExit(serverFD, nickname, peerAddr);
	 break;
       }
       else
       {
       sendMsg(serverFD, nickname, buf2, peerAddr);
       }
     }
     free(buf2);
   }
 free(nickname);
 return 0;
}
/*
void recvPeerMsg(int fd)
{
  unsigned int peerAddrlen;
  int recvBytes;
    
  struct chatPDU* pCurrMsg = malloc(sizeof(struct chatPDU));
  struct sockaddr* peerAddr = malloc(sizeof(struct sockaddr));

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
  
//   printf("typ is %d, name is %s, msg is %s", pCurrMsg->typ, pCurrMsg->name, pCurrMsg->msg);
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
//  printf("send\n");
  int sendbytes;
  //printf("sizeof buf %d\n", sizeof(*buf2));
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
    perror("sendto:");
  }
  //printf("send %d bytes\n", sendbytes);
  free(pExitMsg);
}
*/