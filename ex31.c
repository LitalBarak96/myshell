#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <errno.h>
#include <string.h>
#define SIZE 1

// take care of open the right file
int main( int argc, char* argv[] )
{

	int charsr1=0;
	int charsr2=0;
    if( argc != 3) {
        fprintf( stderr, "Use: cd <directory>\n" );
        return EXIT_FAILURE;
    }

    int fdin1;           /* input file descriptor */ 
	int fdin2;          /* out   file descriptor */ 

	fdin1 = open(argv[1],O_RDONLY);
	if (fdin1 < 0) /* means file open did not take place */ 
	{                
		perror("after open ");   /* text explaining why */ 
		exit(-1); 
	}
	/* create the file with read-write premissions */
	fdin2 = open(argv[2],O_RDONLY);	
	if (fdin2 < 0)	/* means file open did not take place */ 
	{ 
		perror("after create ");   /* text explaining why */ 
		exit(-1); 
	}
    int count =0;
	int flag = 0;



    do 
	{
        char buf2[SIZE]={};
        char buf1[SIZE]={};
		charsr1 = read(fdin1, buf1 ,SIZE);
        charsr2 = read(fdin2, buf2 ,SIZE);
        if (buf1[0] == buf2[0]) {
        }
        else{
			
            count++;
			flag=1;

// this is if the first char is space and the other is tab,they ara similar and this is okay
			if(buf1[0] == 32) {
				if (buf2[0] == 9){
				flag=2;
				charsr2 = read(fdin2, buf2 ,SIZE);
				}
				else if (buf2[0] == 10){
				flag =2;
				charsr2 = read(fdin2, buf2 ,SIZE);
				}
				else{
				charsr2 = read(fdin2, buf2 ,SIZE);
				flag =2;
				}
			
			}
			if(buf2[0] == 32){
				if (buf1[0] == 9){
					charsr1 = read(fdin1, buf1 ,SIZE);
					flag=2;
				}
				else if (buf1[0] == 10){
					charsr1 = read(fdin1, buf1 ,SIZE);
					flag=2;
				}
				else{
					charsr1 = read(fdin1, buf1 ,SIZE);
					flag =2;
				}
				}

// this is the oppersit cuz we dont know whice file is the broken one
			if (buf1[0] == 9){
				charsr2 = read(fdin2, buf2 ,SIZE);
				
			}
			 if (buf2[0] == 9){
				charsr1 = read(fdin1, buf1 ,SIZE);
			}
			
			if(buf1[0] == 10){
				charsr2 = read(fdin2, buf2,SIZE);
				

				flag =2;
				

			}
			
			if(buf2[0] == 10){
				
				charsr1 = read(fdin1, buf1 ,SIZE);
				
				flag =2;
				
			}

			if (buf2[0] == buf1[0]-32){
				flag =2;
				

			}
			 if (buf1[0] == buf2[0]-32){
				flag =2;
		


			}
	}
			


			
        
	
        

    }while ( (charsr1 == SIZE) && (charsr2 == SIZE));
	if (flag == 0){
		//everyone was the same
		exit (1);
	}
	//diffrent
	if(flag == 1){
		exit (2);
	}
	//similar
	if (flag == 2){
		exit (3);
	}
	

		
	

	close(fdin1);		/* free allocated structures */
	close(fdin2);		/* free allocated structures */

}

