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

//func to write to csv
void Write_to_csv(int fd,char* name, char* msg){
    char tempArr[150];
    strcpy(tempArr,name);
    char* temp = strcat(tempArr,msg);
    if (write(fd, temp, strlen(temp)) == -1) {}
}


//comparing the output with comp.out
int compareOutput(char comp[],char outputFile[], char path_to_write[]){
    int status;
    pid_t pid;
    pid = fork();
    if(pid == 0) {
        char *tab[] = {comp,outputFile,path_to_write, NULL};
        if (execvp(comp, tab) == -1) {
            if(write(2, "error in running in compare function\n",
                     strlen("error in running in compare function\n")) == -1){}
            exit(-1);
        }
    } else if(pid < 0){
        return -1;
    } else if(pid > 0){
        if(waitpid(pid, &status, 0) >0){
            if(WIFEXITED(status) && !WEXITSTATUS(status)){
             //the returnd value is 0
            } else if(WIFEXITED(status) && WEXITSTATUS(status)) {
                if (WEXITSTATUS(status) == 127) {
                    printf("error in compare function\n");
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
    int flag =0;
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
      waitpid(pid, &status, 0);
        time(&finish);
        if((difftime(finish,start))>5){
            return 6;
        }
        else{
            return status;
        }

      
       

    }

}

int compileCheck(char fileName[]){
    int status;
    pid_t pid;
    pid = fork();
    if(pid == 0) {
        char *tab[] = {"gcc", fileName, "-o", "compiletest.out", NULL};
        if (execvp("gcc", tab) == -1) {
            if(write(2, "error in compile file, execvp failed.\n",
                     strlen("error in compile file, execvp failed.\n")) == -1){}
            exit(-1);
        }
    } else if(pid < 0){
        return -1;
    } else if(pid > 0){
       if(waitpid(pid, &status, 0) >0){
           if(WIFEXITED(status) && !WEXITSTATUS(status)) {
           }
           else if(WIFEXITED(status) && WEXITSTATUS(status)) {
               if (WEXITSTATUS(status) == 127) {
                   printf("error in compile function\n");
               } else {
                   return -3;
               }
           }
       } else{
            return -1;
       }
    }
    remove("compiletest.out");

    return 0;
}



int main(int argc,char *argv[])
{
    int timeout;
    char cwd[150];
    char cwd1[150];
    int newfd;
    int infd;
    int test;
    int wrfd;
    int errfd=2;
    int flag;
    char buf1[SIZE];
	int charsr1;
    int i = 0;

    char paths[1024] = {0};
    char pathDir[1024] = {0};
    char pathInput[1024] = {0};
    char pathOutput [1024] = {0};
    int fdDir = 0;
    if((fdDir = open(argv[1], O_RDONLY))== -1) {
        if(write(2, "error in opening conf file\n",
                strlen("error in opening conf file\n")) == -1){}
        exit(-1);
    }
    if(read(fdDir, paths,1024) == -1){
        if(write(2, "error in reading conf file\n",
                strlen("error in reading conf file\n")) == -1){}
        exit(-1);
    }

    // parse the paths:
    char* token = strtok(paths,"\n");
    strcpy(pathDir,token);
    token = strtok(NULL,"\n");
    strcpy(pathInput,token);
    token = strtok(NULL,"\n");
    strcpy(pathOutput,token);
   

//     level_1_handle(pathDir,pathInput,pathOutput);





    //to get the dirctory for the comp
    getcwd(cwd, sizeof(cwd));
    char main[150];
    strncpy(main,cwd,150);
    

//     // strcat(tempPath,"/comp.out");

//     int fdin1;           /* input file descriptor */ 
//     //compileing EX31 for comp.c 


//     //for the csv file 
     if((wrfd = open("results.csv",O_CREAT|O_TRUNC|O_WRONLY|O_APPEND,0664))<0){
     perror("results.csv");
      exit(-1);
     }
// //for the error file
         if((errfd = open("error.txt",O_CREAT|O_TRUNC|O_WRONLY|O_APPEND,0664))<0){
     perror("error.txt");
      exit(-1);
      }
    char *comp[]={"gcc","-o","./comp.out","comp.c",NULL};
     excute("gcc",comp,errfd);
    char compad[150];
    strncpy(compad,main,150);
    strcat(compad,"/");
    strcat(compad,"comp.out");

//     //for the 3 arguments inside the config
//    char line1[3][150];
//     int j=0;
// do{
//         char buf1[SIZE]={};
// 		charsr1 = read(fdin1, buf1 ,SIZE);
//         if (buf1[0]!=10){
//         line1[i][j] = buf1[0];
//         j++;
//         }
//         else{

//             line1[i][j]='\0';
//             if(i==3){
//                 i=2;
//                 line1[i][j]='\0';
//             }
//             j=0;
//         }

// }while(charsr1 == SIZE);



            
    if((open(pathOutput,O_RDONLY))<0){
    perror("output file not exist\n");
        exit(-1);
        }



// //changing the dirctory 
//     // char level0[150];
//     // char cwd2[150];
//     // getcwd(cwd2, sizeof(cwd2));
//     // strncpy(level0,cwd2,150);
//     // strcat(level0,"/");
//     // strncpy(level0,,150);


 chdir(pathDir);
    struct dirent* dent;
    struct dirent* dent1;
    DIR* srcdir = opendir(".");
    DIR* srcdir1 = opendir(".");
    int counter_for_c=0;

  

    if (srcdir == NULL)
    {
        perror("opendir");
        return -1;
    }
//until here we where in student and no
    while((dent = readdir(srcdir)) != NULL)
         {
        char IN[150];
         strncpy(IN,main,150);
         strcat(IN,"/");
         strcat(IN,pathInput);
                 char OUT[150];
         strncpy(OUT,main,150);
         strcat(OUT,"/");
         strcat(OUT,pathOutput);
        if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0){
        continue;

        }
            if((infd = open(IN,O_RDONLY))<0){
    perror("Input file not exist\n");
    exit(-1);
        }

             //LELVEL 1 IS
        flag =0;
        timeout=0;
          struct stat st;
         getcwd(cwd1, sizeof(cwd1));
         char level1[150];
         strncpy(level1,cwd1,150);
         strcat(level1,"/");
         strcat(level1,dent->d_name);




// // // I need to fix searching in SUb directorys
// //         if (fstatat(dirfd(srcdir), dent->d_name, &st, 0) < 0)
// //         {   
// //             perror(dent->d_name);
// //             continue;
// //         }
// open to the dir of the file 
            if (dent->d_type == DT_DIR){
            if ((srcdir1 = opendir(level1)) == NULL){
                 return -1;
                 perror("here");
            }             char write[150];
                         strncpy(write,level1,150);
                        strcat(write,"/");
                         strcat(write,"write.txt");
        if((newfd = open(write,O_CREAT|O_TRUNC|O_WRONLY,0664))<0){
        perror("write.txt");
        exit(-1);
        }
        
            
//            read the files inside

             while((dent1 = readdir(srcdir1)) != NULL)
                {
                    if(strcmp(dent1->d_name, ".") == 0 || strcmp(dent1->d_name, "..") == 0){
continue;
                    }    
                    
// open in each sub dir the write file I need to make it write into it

                        char level2[150];
                         strncpy(level2,level1,150);

                    char* end = strrchr(dent1->d_name,'.');
                    if(end && !strcmp(end, ".c")) {
                         //level 2 is the file name 'it sepouse to be with the c file
                        counter_for_c++;
                        int ret1=0;
                        int com=0;
                        strcat(level2,"/");
                         strcat(level2,dent1->d_name);
                        dup2(infd, 0); 
                        dup2(newfd,1);
                        dup2(errfd,2);
                        fflush(stdin);
                        char *student[]={"gcc","-o","./a.out",level2,NULL};
                        excute("gcc",student,errfd);
                        char* run[] = {"a.out",NULL};
                        int ret=compileCheck(level2);
                        if (ret !=-3){
                            ret1= excute("./a.out",run,errfd);
                             com= compareOutput(compad,OUT,write);
                        }
                        
                        
                        
                    if(ret==-3){
                    timeout =1;
                    remove(write);
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",10,COMPILATION_ERROR\n");
                    fflush(stdout);
                    flag =1;
                    }
                    if(ret1==6){
                    remove(write);
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",20,TIMEOUT\n");
                    fflush(stdout);
                    flag =1;
                    }
                    if(com == 1&&flag==0){
                    remove(write);
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",100,EXCELLENT\n");
                    fflush(stdout);
                
                    }
                    if(com == 2&&flag==0){
                    remove(write);
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",50,WRONG\n");
                    fflush(stdout);
                    
                    }
                    if(com == 3&&flag==0){
                    remove(write);
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",75,SIMILAR\n");
                    fflush(stdout);
                    }

                    }
     
                    }
                    if(counter_for_c==0){
                   remove(write);
                    dup2(wrfd,1);
                    Write_to_csv(wrfd,dent->d_name,",0,NO_C_FILE\n");
                    fflush(stdout);
               }

               counter_for_c =0;
               closedir(srcdir1);

                }
                 close(infd);
            }
    close(wrfd);
    close(newfd);
   
    close(test);
     closedir(srcdir);
     return 0;
         }
         
