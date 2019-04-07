#include "chatLib.h"

int main(int argc, char *argv[])
{
  if ( argv[1] == 0 )
    printf("no port specified\n");

//    printf("The following arguments were passed to main(): ");
//    for(i=1; i<argc; i++) printf("%s ", argv[i]);
//    {printf("\n"); 
//    }
  int events;
  fd_set readset;
  char* nickname;
  size_t nameLen;
  size_t nameChars;
  size_t len;
  unsigned int localPort =3000;
//  unsigned int peerPorts[9];
  char* peerIPs[MAXPEERS-1];
  int i;
  int peerFDs[MAXPEERS-1];
  struct sockaddr_in allPeerAddrs[9];
  char* myIP;
  
  nickname = malloc( 13*sizeof(char));
  
  //setup port as server and peer ports
//  localPort = atoi(argv[1]);
  strcpy(myIP, argv[1]);
//    printf("localPort is %d\n", localPort); 
  for (i = 0; i < MAXPEERS; i++)
  {
/*    if(ports[i] != localPort)
    {
      peerPorts[i] = ports[i];
//      printf("Port %d added to peerPorts\n", peerPorts[i]);
    }
    */
  if(strcmp(ips[i],myIP))
  {
    peerIPs[i]= ips[i];
  }
  }
  //server part
  int serverFD = socket( AF_INET, SOCK_DGRAM, 0);
  if(serverFD < 0)
  {
    perror(" socket serverFD:");
  }
  struct sockaddr_in localAddr;
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(localPort);
  localAddr.sin_addr.s_addr = inet_addr(myIP);
      
  int rc = bind(serverFD, (struct sockaddr*)&localAddr, sizeof(localAddr));
  if (rc < 0)
  {
    printf("Error: Bind 1 FD\n");
    perror("bind()");
    rc = 0;
  }
  //server part end
  
  // setup all peer addresses
  for(i = 0; i <(MAXPEERS-1); i++)
  {
  //peer part
  peerFDs[i] = socket( AF_INET, SOCK_DGRAM, 0);
  if(peerFDs[i] < 0)
  {
    perror(" socket serverFD:");
  }
//  allPeerAddrs[i] = (struct sockaddr_in)malloc(sizeof(struct sockaddr_in));
  memset(&allPeerAddrs[i], 0, sizeof(allPeerAddrs[i]));
  allPeerAddrs[i].sin_family = AF_INET;
  allPeerAddrs[i].sin_port = htons(3000);
  allPeerAddrs[i].sin_addr.s_addr = inet_addr(peerIPs[i]);
  //peer part end
  }
  
  // setup all peer addresses
/*  for(i = 0; i <9; i++)
  {
  //peer part
  peerFDs[i] = socket( AF_INET, SOCK_DGRAM, 0);
  if(peerFDs[i] < 0)
  {
    perror(" socket serverFD:");
  }
//  allPeerAddrs[i] = (struct sockaddr_in)malloc(sizeof(struct sockaddr_in));
  memset(&allPeerAddrs[i], 0, sizeof(allPeerAddrs[i]));
  allPeerAddrs[i].sin_family = AF_INET;
  allPeerAddrs[i].sin_port = htons(peerPorts[i]);
  allPeerAddrs[i].sin_addr.s_addr = inet_addr("127.0.0.1");
  //peer part end
  }
  */
  //get user input
  printf("Enter name:");
  nameChars = getline(&nickname, &nameLen, stdin);
//TODO: Errorhandling input == 0
  
  for(i=0; i<(MAXPEERS-1); i++)
  {
  sendEntry(serverFD, nickname, allPeerAddrs[i]);
  }
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
	 for(i=0;i<(MAXPEERS-1);i++)
	 {
	 sendExit(serverFD, nickname, allPeerAddrs[i]);
	 }
	 break;
       }
       else
       {
	  for(i=0;i<(MAXPEERS-1);i++)
	  { 
	    sendMsg(serverFD, nickname, buf2, allPeerAddrs[i]);
	  }
       }
     }
     free(buf2);
   }
 for(i=0; i<(MAXPEERS-1);i++)
 {
   close(peerFDs[i]);
 }
 close(localPort);
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