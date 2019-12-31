#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"


//get the numbers from the buffer
void substring( const char srcString[], char destString[], int startPosition, int length)
{
 int k=1;
	for( int i = 0; i < length; ++i )
	{
       if((srcString[startPosition + i]>='0' && srcString[startPosition + i]<='9') ||
        srcString[startPosition + i]==','
        || srcString[startPosition + i]==')'|| srcString[startPosition + i]=='('
        ){
		    destString[i] = srcString[startPosition + i];
            k++;
        }
		if( destString[i] == 0 )
			return;	// reached the end of srcString
	}

}

//get the type of inode from the buffer
void typeofinode( const char srcString[], char destString[], int startPosition, int length)
{
 int k=1;
	for( int i = 0; i < length; ++i )
	{
        if(srcString[startPosition + i]>='A' && srcString[startPosition + i]<='Z')
     {
		    destString[i] = srcString[startPosition + i];
            k++;
        }
		if( destString[i] == 0 )
			return;	// reached the end of srcString
	}
}

int find(char *str, char *substr)
{
	  int pos=0;
      while (*str)
	  {
		    char *Begin = str;
		    char *pattern = substr;
            pos++;

		    // If first character of sub string match, check for whole string
		    while (*str && *pattern && *str == *pattern)
			{
                  str++;
			      pattern++;
		    }
		    // If complete sub string match, return the position of the substring
		    if (!*pattern)
			  return pos;

		    str = Begin + 1;	// Increament main string
	  }
	  return 0;
}



void printinodeinfo(char * buf){
    int pos=0;
    char s[5]={0} ;
    //Device:
    pos= find(buf,":");
    substring(buf,s,pos,5);
	printf(1,"%s ",s);
    buf=buf+pos;
    //Inode number:
    char s2[5]={0} ;
    pos= find(buf,":");
    substring(buf,s2,pos,5);
	printf(1,"%s ",s2);
    buf=buf+pos;
    //is valid:
    char s3[5]={0} ;
    pos= find(buf,":");
    substring(buf,s3,pos,5);
	printf(1,"%s ",s3);
    buf=buf+pos;
    //type:
    char s4[5]={0} ;
    pos= find(buf,":");
    typeofinode(buf,s4,pos,5);
	printf(1,"%s ",s4);
    buf=buf+pos;
    //major minor:
   char s5[6]={0} ;
     pos= find(buf,":");
     substring(buf,s5,pos,6);
	 printf(1,"%s ",s5);
     buf=buf+pos;
    //hard links:
    char s6[5]={0} ;
    pos= find(buf,":");
    substring(buf,s6,pos,5);
	printf(1,"%s ",s6);
    buf=buf+pos;
    //blocks used:
    char s7[5]={0} ;
    pos= find(buf,":");
    substring(buf,s7,pos,5);
	printf(1,"%s\n",s7);
//<#device> <#inode> <is valid> <type> <(major,minor)> <hard links> <blocks used>
}

int isinodedir(char * name){
	if (strcmp(name, "inodeinfo") == 0)
		return 1;
    return 0;
}
int isinodepage(int inum){
  int ninode = (inum % 200) - 50;
  if (ninode>=0 && ninode<50)
    return 1;

  return 0;
}

void openfd(char *path){

 char buf[512];
 int fd;
 int num_of_bits;
 if((fd = open(path, 0)) < 0){
    printf(2, "cannot open %s\n", path);
    return;
}
 if((num_of_bits = read(fd, buf, sizeof(buf))) > 0)
    printinodeinfo(buf);
 if(num_of_bits < 0)
    printf(2, "read error\n");
  close(fd);
}


void lsOfInodeInfoDir(char *path){
   char buf[512], *p;
  int fd;
  struct dirent dir;

  if((fd = open(path, 0)) < 0){
    printf(2, "lsof: cannot open %s\n", path);
    return;
  }

  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';
   printf(1, "ls %s \n",buf);
  while(read(fd, &dir, sizeof(dir)) == sizeof(dir)){
    if(dir.inum == 0)
      continue;
    printf(1, "%s\n", dir.name);

    memmove(p, dir.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (isinodedir(dir.name))
    {
        lsOfInodeInfoDir(buf);
    }
  }
  close(fd);
}


void PrintInodeInfoPages(char *path){
  char buf[512], *p;
  int fd=0;
  struct dirent dir;

//open the path and get file descriptor
  if((fd = open(path, 0)) < 0){
    printf(2, "lsof: cannot open %s\n", path);
    return;
  }

// insert path to buf
  strcpy(buf, path);
  p = buf+strlen(buf);
// add / to path
  *p++ = '/';

//read from our path
  while(read(fd, &dir, sizeof(dir)) == sizeof(dir)){
    if(dir.inum != 0){
        memmove(p, dir.name, DIRSIZ);
        p[DIRSIZ] = 0;
        //check if the path is /proc/inodeinfo- if so we enter recursivelly to the path
        if (isinodedir(dir.name))
            PrintInodeInfoPages(buf);

        //check if the path is /proc/inodeinfo/number - if so we open every page and print it's content
        if(isinodepage(dir.inum))
            openfd(buf);
    }
  }
  close(fd);
}




int main(int argc, char *argv[]){
 if(argc>1){
    lsOfInodeInfoDir("proc");
 }
 else{
    PrintInodeInfoPages("proc");
 }

  exit();
}
