#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

/*-------Function Declaration--------------------*/
int Menu();              // function to ask user which operation to perform.
void write_in_file();    // function to create a new file and write in it.
void readFile();         // function to read a file with a given file pointer and display contents in standard terminal.
void editFile();         // function to open a file, edit its contents and save it.
void saveAs();           // function to open a file and save it with different name, keeping original file intact.
void Ctrl_F();           // function to search for a given word/pattern in a file.
void compareFiles();     // function to compare two file whether they are exactly similar or not.
void del();           // function to delete a file.
void replaceLine();      // helper function for editFile() to replace a given line in given file
void appendLine();       // helper function for editFile() to append a line in given file
void deleteLine();       // helper function for editFile() to delete a given line in given file
/*-------------------------------------------------*/

int main(void){
	bool running = 1;
	while(running) {
		int n;
		n = Menu();
		switch(n) {
			case 1:
				write_in_file();
				break;
			case 2:
				editFile();
				break;
	    	case 3:
	    		saveAs();
    			break;
	    	case 4:
		    	Ctrl_F();
    			break;
	    	case 5:
		    	compareFiles();
    			break;
	    	case 6:
		    	del();
    			break;
    		case 7:
    			readFile();
    			break;
		    case 8:
			    running = 0;
	    		break;

		    default:
		    	printf("please Enter a valid choice\n");
		}
	}

	return 0;
}



int Menu() {
	printf("\n-----------------------------------------------------------------------------------------------------\n");
	char n, extra;
	printf("Which operation you would like to perform?\n");
	printf("1. Create a new file.\n");
	printf("2. Edit a file.\n");
	printf("3. Save As\n");
	printf("4. Search for a word.\n");
	printf("5. Compare two files.\n");
	printf("6. Delete a file.\n");
	printf("7. Read a file.\n");
	printf("8.Exit()\n");

	scanf("%c", &n);
	scanf("%c", &extra);
	printf("-------------------------------------n-------------------------------------------------------------------\n");
	return (int)(n - '0');
}


void write_in_file() {
	char fname[20], buf[80], c, extra[5];
	int n;
	printf("Enter file name:- ");
	scanf("%s", fname);
	fgets(extra, 4, stdin);
	FILE *fp;

	fp = fopen(fname, "r");
	while(fp != NULL && fgetc(fp) != EOF) {    // check if file with fname already exists AND IF IT IS NOT EMPTY
		printf("A non-empty file with that name already exists. Please choose a different name\n");
		printf("Enter file name:- ");
		scanf("%s", fname);
		fp = fopen(fname, "r");
	}
	fclose(fp);

	fp = fopen(fname, "w");    //open file in write mode.
	if(fp == NULL) {
	    printf("File NOT created!\n");
	    remove(fname);
	    fclose(fp);
	    return ;    //exit from the function if file is not created.
	}

	printf("enter no of line to write: ");
	scanf("%d", &n);
	fgets(extra, 4, stdin);    // FIXES NEWLINE BUG
	printf("<Start writing below: >\n");
	for(int i=0; i<n; i++){
		fgets(buf, 79, stdin);

		if(i == n-1) {    // DONT ADD NEWLINE AT THE END OF LAST LINE
			int pos = 0;
			char letter = buf[0];
			while(letter != '\n') {    // IF WRITING LAST LINE, WRITE EVERYTHING EXCEPT NEWLINE AT THE END
				fputc(letter, fp);
				pos++;
				letter = buf[pos];
			} break;
		}

		fprintf(fp, "%s", buf);
	}
	fclose(fp);
}


void editFile() {
	int ans;
	char extra;
	fprintf(stdout, "1. Replace\n2. Append\n Choose(1/2): ");
	fscanf(stdin, "%d", &ans);
	extra = fgetc(stdin);
	if(ans == 1)
		replaceLine();
	else
		appendLine();
}


void replaceLine() {
	FILE *fp1, *fp2;
	char fname[20], content[35], temp[] = "temp.txt";
	char extra[5];    //FIXES NEWLINE BUG. DO NOT REMOVE
	/*WHEN CHOICE IS SCANNED IN MENU THERE IS ALSO A '\n' AT THE END WHICH AUTOMATICALLY
	GOES INTO CONTENT AND SKIPS ITS INPUT. SO WE STORE THAT '\n' IN A SEPERATE VARIABLE*/
	int line_num;

	fprintf(stdout, "Enter file name: ");
	/*fscanf and fgets DONT WORK HERE FOR SOME REASON*/
	scanf("%s", fname);

	fgets(extra, 4, stdin);
	fprintf(stdout, "New content: ");
	fgets(content, 34, stdin);

	fp1 = fopen(fname, "r");
	if (fp1 == NULL) {
		fprintf(stdout, "ERROR! Cannot open file\nPlease try again\n");
		fclose(fp1);
		return ;
	}
	// Ask for line number when file is valid
	fprintf(stdout, "Enter the number of the line to replace: ");
	fscanf(stdin, "%d", &line_num);
	fgets(extra, 4, stdin);

	fp2 = fopen(temp, "w");    // WRITE INTO TEMP FROM GIVEN FILE THEN RENAME

	int current_line = 1;
	char c = fgetc(fp1);
	while(c != EOF) {
		// WRITE INTO FILE IF CURRENT LINE IS NOT USER GIVEN LINE
		if(current_line != line_num) {
			fputc(c, fp2);
		}
		else {
			// when user given line is encountered SKIP THE WHOLE LINE
			while(c != '\n' && c != EOF) {    // IF WE ARE EDITING THE LAST LINE, IT WILL HAVE EOF AT THE END, NOT \n
				c = fgetc(fp1);
			}

			if(c == EOF)    // IF WE ARE EDITING LAST LINE, WE DONT WANT TO PUT AN EXTRA \n AT THE END
				content[strlen(content) - 1] = '\0';    // CHOP OFF NEWLINE AT END
			// now write 'content' into temp file
			fputs(content, fp2);
		}
		if(c == '\n')
			current_line++;
		if(c != EOF)
			c = fgetc(fp1);
	}
	fclose(fp1);
	fclose(fp2);
	remove(fname);    // remove original file
	rename(temp, fname);    // rename temp to original file
	fprintf(stdout, "Changes Saved!\n");
}


void appendLine() {
	FILE *fp;
	char fname[20], content[35], temp[] = "temp.txt";
	char extra[5];
	int lines;
	fprintf(stdout, "Enter file name: ");
	scanf("%s", fname);

	fgets(extra, 4, stdin);    //FIXES NEWLINE BUG.

	fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stdout, "ERROR! Cannot open file\nPlease try again\n");
		fclose(fp);
		return ;
	}
	// Ask for line number when file is valid
	fprintf(stdout, "How many lines do you want to append?: ");
	fscanf(stdin, "%d", &lines);
	fgets(extra, 4, stdin);

	fp = fopen(fname, "a");
	fputc('\n', fp);
	for(int i=0; i<lines; i++) {
		fprintf(stdout, "Content for line %d: ", i);
		fgets(content, 79, stdin);
		if(i == lines - 1)
			content[strlen(content) - 1] = '\0';
		fputs(content, fp);
	}
	fclose(fp);
	fprintf(stdout, "Changes Saved!\n");
}


void deleteLine() {
	fprintf(stdout, "Enter file name: ");
}


void saveAs() {
	FILE *fp1, *fp2;
	char buff[80], fname[20], new_fname[20];
	printf("Enter file name: ");
	scanf("%s", fname);
	printf("Enter new file name: ");
	scanf("%s", new_fname);

	fp1 = fopen(fname, "r");
	fp2 = fopen(new_fname, "w");
	if(fp1 == NULL) {
	    printf("Error 404! Input File not Found.\n");
	    remove(fname);
	    remove(new_fname);
	    return ;
	}
	char c;
	c = fgetc(fp1);
	while(c != EOF) {
		fputc(c, fp2);
		c = fgetc(fp1);
	}

	fclose(fp1);
	fclose(fp2);
}


void Ctrl_F()
{
	char buff[80];
	char search[20];
	char fname[20];
	printf("Enter file name to be searched: ");
	scanf("%s",fname);

	FILE *fp;
	fp = fopen(fname, "r");
	if(fp == NULL) {
	    printf("File NOT found!\n");
	    fclose(fp);
	    return ;
	}

	printf("Enter string to find: ");
	scanf("%s", search);
	int lino = 0, flag = 0;
	while(fgets(buff, 79, fp)) {    //buff string will take one line of the file.
	    lino++;
		if(strstr(buff, search)) {    //if search is a substring in the buff string.
	        flag = 1;
			printf("Word Found at line no %d\n", lino);
	    }
	}
	fclose(fp);
	if(flag == 0)
		printf("Word NOT found!");
}


void compareFiles() {
	FILE *fp1, *fp2;
	char fname1[20], fname2[20];
	int flag = 1;

	printf("Enter name of file 1: ");
	scanf("%s", fname1);
	printf("Enter name of file 2: ");
	scanf("%s", fname2);
	fp1 = fopen(fname1, "r");
	fp2 = fopen(fname2, "r");

	if(fp1 == NULL || fp2 == NULL) {
	    printf("Input file not found.\n");
	    fclose(fp1);
	    fclose(fp2);
	    remove(fname1);
	    remove(fname2);
	    return ;
	}

	char c1,c2;
	do {
	    c1 = fgetc(fp1);
	    c2 = fgetc(fp2);
	    if(c1 != c2) {
			flag = 0;
			break;
	    }
	} while(c1 != EOF && c2 != EOF);

	if(flag == 1)
		printf("Files are exactly similar\n");
	else
		printf("File are NOT exactly similar.\n");
	fclose(fp1);
	fclose(fp2);
}


void del() {
	char fname[20], extra;
	printf("Enter file name to be deleted: ");
	scanf("%s",fname);
	extra = fgetc(stdin);    // handles newline bug
	if(!remove(fname))    //remove function returns 0 when it deletes file successfully.
		printf("%s is DELETED SUCCESFULLY.\n", fname);
	else
		printf("%s  CANNOT be DELETED.\n", fname);
}


void readFile() {
    FILE *fp;
    char c, fname[20], extra;

    printf("Enter file name: ");
    scanf("%s", fname);
    extra = fgetc(stdin);
    fp = fopen(fname, "r");
    if(fp == NULL) {
        printf("File NOT found!\n");
        remove(fname);
        fclose(fp);
        return ;
    }

	c = fgetc(fp);
	while(c != EOF) {
		printf("%c", c);
		c = fgetc(fp);
	}
	fclose(fp);
}

