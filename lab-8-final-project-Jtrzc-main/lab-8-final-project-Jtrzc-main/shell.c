// PID: 730273827
// I pledge the COMP211 honor code.

// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included.
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "shell.h"

const char* valid_builtin_commands[] = {"cd", "exit", NULL};


void parse( char* line, command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
	
	char *token;
	int i=0; //used for counting argc
	char* null = "NULL";
	if(line[0] == '\n'){
		p_cmd->argc=0;
	}
	if(line[0] != '\n'){
		token = strtok(line," \n");
		while(token!=NULL){
			p_cmd->argv[i]=malloc(sizeof(char)*100);
			strcpy(p_cmd->argv[i],token);      
			token = strtok(NULL," \n");
			i++;
		}                     
		p_cmd->argc=i; 
		strcpy( p_cmd->path, p_cmd->argv[0]);
		if(is_builtin(p_cmd)){
		}else{
			p_cmd->argv[i]=NULL;
			find_fullpath(p_cmd->path,p_cmd);
		}
	}
    return;
} // end parse function


int find_fullpath( char* command_name, command_t* p_cmd ) {
    // TO-DO: COMPLETE THIS FUNCTION BODY
    char fullpath[200];
    int exists = FALSE;
	int i=0; //used to loop through the paths for the holder in line 57
	struct stat buffer;
	char* file_or_dir[7];
	char* holder = getenv("PATH"); //temp variable to hold the paths
	char *token;
	char path[] = "";
    token = strtok(holder,":");
    while(token!=NULL){
		file_or_dir[i]=malloc(sizeof(char)*100);
        strcpy(file_or_dir[i],token);
        token = strtok(NULL,":\n");
        i++;
    }
	int bool =0;
	for(int j=0;j<7;j++){
		strcpy(path, file_or_dir[j]);
		strcat(path, "/");
		strcat(path, p_cmd->path );
		exists = stat(path, &buffer );
		if ( exists == 0 && ( S_IFREG & buffer.st_mode ) ) {
				  bool=1;
				  strcpy(p_cmd->path, path);
				  break;		 
		}
	}
	if(bool == 0){
		p_cmd->argc=-1;
		p_cmd->argv[0]=NULL;
	}
    return exists;
}


int execute( command_t* p_cmd ) {
    int status = SUCCESSFUL;
    int child_process_status;
    pid_t child_pid = fork();
	if ( child_pid == 0 ) {
      execv(p_cmd->path,p_cmd->argv);
      perror("Execute terminated with an error condition!\n");
      exit( 1 );
}
	wait( NULL);
    return status;
} // end execute function


int is_builtin( command_t* p_cmd ) {

    int cnt = 0;

    while ( valid_builtin_commands[cnt] != NULL ) {

        if ( equals( p_cmd->path, valid_builtin_commands[cnt] ) ) {

            return TRUE;

        }

        cnt++;

    }

    return FALSE;

} // end is_builtin function


int do_builtin( command_t* p_cmd ) {

    // only builtin command is cd

    if ( DEBUG ) printf("[builtin] (%s,%d)\n", p_cmd->path, p_cmd->argc);

    struct stat buff;
    int status = ERROR;

    if ( p_cmd->argc == 1 ) {

        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental 
        // variable

        status = chdir( getenv("HOME") );

    } else if ( ( stat( p_cmd->argv[1], &buff ) == 0 && ( S_IFDIR & buff.st_mode ) ) ) {


        // -----------------------
        // cd with one arg 
        // -----------------------
        // only perform this operation if the requested
        // folder exists
        status = chdir( p_cmd->argv[1] );

    } 
	
	
    return status;

} // end do_builtin function



void cleanup( command_t* p_cmd ) {

    int i=0;

    while ( p_cmd->argv[i] != NULL ) {
        free( p_cmd->argv[i] );
        i++;
    }

    free( p_cmd->argv );
    free( p_cmd->path );	

} // end cleanup function


int equals( char* str1, const char* str2 ) {
    int len[] = {0,0};

    char* b_str1 = str1;
    const char* b_str2 = str2;

    while( (*str1) != '\0' ) { 
        len[0]++;
        str1++;
    }

    while( (*str2) != '\0' ) {
        len[1]++;
        str2++;
    }

    if ( len[0] != len[1] ) {

        return FALSE;

    } else {

        while ( (*b_str1) != '\0' ) {

            if ( tolower( (*b_str1)) != tolower((*b_str2)) ) {

                return FALSE;

            }

            b_str1++;
            b_str2++;

        }

    } 

    return TRUE;


} // end compare function definition