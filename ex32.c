#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define SIZE 1

int excute(char* filename ,char* args[]){
    pid_t pid;
    int status;
    time_t start;
    time(&start);
    time_t finish;
    // char* args[] = {"./a.exe", "a.exe", NULL };
    if((pid = fork()) < 0){
        printf("error fork child procces failed\n");
        exit(1);
    }
    else if(pid == 0){
        if (execvp(filename, args) < 0 ){
            printf("exec fail\n");
            // if exec fails cant compile the shit 
            exit(1);
            
         }
    }
    else{
        waitpid(pid,&status,0);
        time(&finish);
        if((difftime(finish,start))>5){
            return 5;
        }
        else{
            return status;
        }
    }

}




int main(int argc,char *argv[])
{
    
    char buf1[SIZE];
	int charsr1;
    int i = 0;
    
    int fdin1;           /* input file descriptor */ 

	fdin1 = open(argv[1],O_RDONLY);
	if (fdin1 < 0) /* means file open did not take place */ 
	{                
		perror("after open ");   /* text explaining why */ 
		exit(-1); 
	}

    //is it should be 2 or 3
   char line1[3][255];

    int j=0;
do{
        char buf1[SIZE]={};
		charsr1 = read(fdin1, buf1 ,SIZE);
        if (buf1[0]!=10){
        line1[i][j] = buf1[0];
        j++;
        }
        else{
            i++;
            j++;
            line1[i][j]='\0';
            j=0;
        }

}while(charsr1 == SIZE);



pid_t pid;
int status;
    if (chdir(line1[0])!= 0) {
    perror("chdir()  failed");
    }
    else{
    struct dirent* dent;
    struct dirent* dent1;
    DIR* srcdir = opendir(".");
    DIR* srcdir1 = opendir(".");
    int newfd;
    int infd;
    int wrfd;
    int errfd;

    if((wrfd = open("result.csv",O_CREAT|O_TRUNC|O_WRONLY|O_APPEND,0664))<0){
    perror("result.csv");
     exit(1);
     }

        if((errfd = open("error.txt",O_CREAT|O_TRUNC|O_WRONLY|O_APPEND,0664))<0){
    perror("result.csv");
     exit(1);
     }

    if (srcdir == NULL)
    {
        perror("opendir");
        return -1;
    }

    while((dent = readdir(srcdir)) != NULL)
    {
        struct stat st;

        if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
            continue;

        if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0)
        {   
            perror(dent->d_name);
            continue;
        }
            if (S_ISDIR(st.st_mode)){
            if(chdir(dent->d_name)!=0){
            perror("chdir() to name  failed");
            }
            else{
            DIR* srcdir1 = opendir(".");
            infd = open(line1[1],O_RDONLY);
            if((newfd = open("d.txt",O_CREAT|O_TRUNC|O_WRONLY,0664))<0){
            perror("d.txt");
            exit(1);
            }
            while((dent1 = readdir(srcdir1)) != NULL)
               {
                   if(dent1->d_type == DT_REG){
                    char * end = strrchr(dent1->d_name, '.');
                        if(strcmp(end, ".c") == 0){
                        char *student[]={"gcc","-o","./a.out",dent1->d_name,NULL};
                        excute("gcc",student);
                        char* run[] = {"a.out",NULL};
                        dup2(infd, 0); 
                        dup2(newfd,1);
                        dup2(errfd,2);
                    if(excute("./a.out",run)==5){
                        


                    dup2(wrfd,1);
                    printf("20 timeout error\n");
                    fflush(stdout);

                    
                       } 
                    }
                    //if nothing is written inside there is no C file
                   }
                   
               }
               
                 chdir("..");

                closedir(srcdir1);
            }
        }
    
    
    }
    close(wrfd);
    close(newfd);
    close(infd);
    
    closedir(srcdir);
    return 0;
    }



}
