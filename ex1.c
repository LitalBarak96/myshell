#include <unistd.h>
#include <sys/types.h>
#include<stdio.h>
#include <string.h>


int main(int argc, char* argv[])
{
    char command[300];
    
    printf("$ ");
    scanf("%s",&command);
    
    while(strcmp(command,"exit") != 0){
        printf("DOING IMPORTANT COMMAND YES");
        printf("\n$ ");
        scanf("%s",&command);
    }
}