/*
 *    adsh: 	this is a small and entirely unsophisticated 
 * 		shell that uses execvp and can execute 
 * 		simple commands (sys programs). If you are to add more features 
 *  		such as pipes, backgrounding, mulitple instructions, 
 * 		wild characters, changing directories, etc.
 * 		more work is needed. Also the interface is clumsy.. 
 * 		
 *
 *   A. Delis alex.delis@nyu.edu
 *
 */

#include 	<stdio.h>
#include	<unistd.h>
#include	<string.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/wait.h>

#define		LEN	40
#define		TK_LN	15
#define 	MAX_LN	80
#define		CMD_MAX	10
#define		OK 	0
#define		NOT_OK	1

/* functions to be used for reading/parsing command lines */

int	mygetline(char *s, int lim);
char	*mygetnexttoken(char *s, int *pos, char *mytoken);
int	extractParams(char *s, char **list_of_args);

/* this is the shell's main body - runs by simple invocation: "adsh" */

int	main(void){
char	line_input[LEN], *new_args[CMD_MAX];
int	i, status, position;
pid_t	cpid;

/* the shell runs for ever.. */
while(1){
	printf("adsh >> ");
	/* obtain the next line from the standard input */
	if (mygetline(line_input, MAX_LN)!= 0){
			/* check if the user wants to exit.. 	*/
		if (strcmp(line_input,"exit ")==0 ) exit(1);
			/* if the input line is just CR then just go get the 
			   next line 				*/
		else if (line_input[0]==' ' && line_input[1]=='\0') continue;
			/* extract the parameters needed for the shell 
			   command invocation 			*/
		else extractParams(line_input,new_args);

		switch(cpid=fork()){
		case 0:
		   /* do run the child with the above extracted parameters */
		   execvp(new_args[0], new_args);
		   perror("exec failure");
		   exit(1);
		case -1:
		   /* take care of possible fork failure */
		   perror("fork failure");
		   break;
		default:
		   /* wait for child/parent to synchronize */
		   cpid=wait(&status);
		} /* switch */
	} /* outer if */
} /* while */
}


int	mygetline(char *s, int lim){
int	c,i;

	for (i=0; i<lim-1 && (c=getchar())!=EOF && c!='\n'; i++)
		s[i]=c;
	if (c=='\n'){
		s[i]=' ';
		++i;
		}
	s[i]='\0';
	return (i);
}

char 	*mygetnexttoken(char *s, int *pos, char *mytoken){
int	i=0;

	for(i=*pos; i<strlen(s) && (s[i]!=' '); i++)
		mytoken[i-(*pos)]=s[i];
	mytoken[i-(*pos)]='\0';
	*pos=i+1;
	return(mytoken);
}

int	extractParams(char *s, char **list_of_args){
int	i=0, position;	
char	nexttoken[TK_LN];

	i=0; position=0;
	strcpy(nexttoken,mygetnexttoken(s,&i,nexttoken));
	do{
		list_of_args[position]=NULL;
		list_of_args[position]=(char *)malloc(2*strlen(nexttoken));
		strcpy(list_of_args[position],nexttoken);
		if (i>strlen(s)) break;
		position++;
		strcpy(nexttoken,mygetnexttoken(s,&i,nexttoken));
	} while (i <= strlen(s) );
	list_of_args[position]=NULL;
	
	if (list_of_args[0]!=NULL) return(OK);
	else 	return(NOT_OK);
}
