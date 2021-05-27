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


int Write_to_csv(int fd,char* name, char* msg){
    char tempArr[1024];
    strcpy(tempArr,name);
    char* temp = strcat(tempArr,msg);
    if (write(fd, temp, strlen(temp)) == -1) {}
    return 0;
}


//comparing the output with comp.out
int compareOutput(char outputFile[],char path[]){
    int status;
    pid_t pid;
    pid = fork();
    if(pid == 0) {
        char *comp[] = {path,outputFile, "d.txt", NULL};
        if (execvp(path,comp) == -1) {
            //write the error to error 
            if(write(2, "error in compare\n",
                     strlen("error in compare\n")) == -1){}
            exit(-1);
        }
        //if error
    } else if(pid < 0){
        return -1;
    } else if(pid > 0){
        if(waitpid(pid, &status, 0) >0){
            if(WIFEXITED(status) && !WEXITSTATUS(status)){
             //the returnd value is 0
            } else if(WIFEXITED(status) && WEXITSTATUS(status)) {
                //ERROR: Could not execute 
                if (WEXITSTATUS(status) == 127) {
                    printf("error in compare\n");
                } else {
                   if(WEXITSTATUS(status) == 1){
                       return 1;
                   } else if(WEXITSTATUS(status) == 2){
                       return 2;
                   } else if(WEXITSTATUS(status) == 3){
                       return 3;
                   }
                }
            }
        } else{
            return -1;
        }
    }
    return 0;
}
// function to excute the program with complie
int excute(char* filename ,char* args[],int fder){
    pid_t pid;
    int status;
    time_t start;
    time(&start);
    time_t finish;
    if((pid = fork()) < 0){
        if(write(fder, "error fork child procces failed\n\n",
                     strlen("error fork child procces failed\n\n")) == -1){}
        exit(1);
    }
    else if(pid == 0){
        if (execvp(filename, args) < 0 ){
            if(write(fder, "error in compile file, execvp failed.\n",
                     strlen("error in compile file, execvp failed.\n")) == -1){}
            return -3;
            
         }
    }
    else{
        //if we need to wait more than 5 sec
        waitpid(pid,&status,0);
        time(&finish);
        if((difftime(finish,start))>5){
            return 6;
        }
        else{
            return status;
        }
    }

}




int main(int argc,char *argv[])
{
    char cwd[150];
    int newfd;
    int infd;
    int wrfd;
    int errfd=2;
    int flag=0;
    char buf1[SIZE];
	int charsr1;
    int i = 0;
    getcwd(cwd, sizeof(cwd));
    char tempPath[150];
    strncpy(tempPath,cwd,150);
    strcat(tempPath,"/comp.out");

    int fdin1;           /* input file descriptor */ 
    //compileing EX31 for comp.c 
    char *comp[]={"gcc","-o","./comp.out","comp.c",NULL};
    excute("gcc",comp,errfd);
	fdin1 = open(argv[1],O_RDONLY);
	if (fdin1 < 0) /* means file open did not take place */ 
	{                
		perror("after open ");  
		exit(-1); 
	}


    //for the 3 arguments inside the config
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

//changing the dirctory 
    if (chdir(line1[0])!= 0) {
    perror("Not a Valid Directory\n");
    exit(-1);
    }
    else{
    struct dirent* dent;
    struct dirent* dent1;
    DIR* srcdir = opendir(".");
    DIR* srcdir1 = opendir(".");
    int counter_for_c=0;


//for the csv file 
    if((wrfd = open("results.csv",O_CREAT|O_TRUNC|O_WRONLY|O_APPEND,0664))<0){
    perror("results.csv");
     exit(-1);
     }
//for the error file
        if((errfd = open("error.txt",O_CREAT|O_TRUNC|O_WRONLY|O_APPEND,0664))<0){
    perror("error.txt");
     exit(-1);
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


// I need to fix searching in SUb directorys
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


            if((infd = open(line1[1],O_RDONLY))<0){
                perror("Input file not fxist\n");
                exit(1);
            }
            if(open(line1[2],O_RDONLY)<0){
                perror("Output file not fxist\n");
                exit(1);
            }
            if((newfd = open("d.txt",O_CREAT|O_TRUNC|O_WRONLY,0664))<0){
            perror("d.txt");
            exit(1);
            }
            while((dent1 = readdir(srcdir1)) != NULL)
               {
                   if(dent1->d_type == DT_REG){
                    char * end = strrchr(dent1->d_name, '.');
                        if(strcmp(end, ".c") == 0){
                        counter_for_c++;
                        char *student[]={"gcc","-o","./a.out",dent1->d_name,NULL};
                        dup2(infd, 0); 
                        dup2(newfd,1);
                        dup2(errfd,2);
                        excute("gcc",student,errfd);
                        char* run[] = {"a.out",NULL};
                        int ret = excute("./a.out",run,errfd);
                        int com = compareOutput(line1[2],tempPath);
                    if(ret==6){
                    remove("d.txt");
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",20,TIMEOUT\n");
                    fflush(stdout);
                    flag =1;
                       }
                    if(ret==-3){
                    flag =1;
                    remove("d.txt");
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",10,COMPILATION_ERROR\n");
                    fflush(stdout);
                    
                    }
                    if(com == 1&&flag==0){
                    remove("d.txt");
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",100,EXCELLENT\n");
                    fflush(stdout);
                    }
                    if(com == 2&&flag==0){
                    remove("d.txt");
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",50,WRONG\n");
                    fflush(stdout);
                    }
                    if(com == 3&&flag==0){
                    remove("d.txt");
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",75,SIMILAR\n");
                    fflush(stdout);
                    }
                        }
                    //if nothing is written inside there is no C file
                   }
                   
               }
               if(counter_for_c==0){
                   remove("d.txt");
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",0,NO_C_FILE\n");
                    fflush(stdout);
               }

               counter_for_c =0;
                
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
