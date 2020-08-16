#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>

/*-------Function Declaration--------------------*/
int Menu();              // function to ask user which operation to perform and return their choice.
void write_in_file();    // function to create a new file and write in it.
void readFile();         // function to read a file with a given name and display contents it on the terminal if it exists.
void editFile();         // function to open a file, edit its contents and save it.
void saveAs();           // function to open a file and save it with different name, keeping original file intact.
void Ctrl_F();           // function to search for a given word/pattern in a file.
void compareFiles();     // function to compare two file and check whether they are exactly similar or not.
void del();           // function to delete a file.
void replaceLine();      // helper function for editFile() to replace a given line in given file
void appendLine();       // helper function for editFile() to append line(s) at the end of given file
void deleteLine();       // helper function for editFile() to delete a given line in given file
int total_lines(char fname[]);    // helper function to find number of lines in a file
/*-------------------------------------------------*/

int main(void) {
	bool running = 1;
	while(running) {
		int n; // stores choice 
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
	char choice[30], extra;  // user might enter gibberish characters, so choice is a character array
	printf("Which operation you would like to perform?\n");
	printf("1. Create a new file.\n");
	printf("2. Edit a file.\n");
	printf("3. Save As\n");
	printf("4. Search for a word.\n");
	printf("5. Compare two files.\n");
	printf("6. Delete a file.\n");
	printf("7. Read a file.\n");
	printf("8.Exit()\n");

	scanf("%s", choice);
	extra = fgetc(stdin); // for mewline bug (REASON EXPLAINED IN replaceLine() FUNCTION)
	printf("-----------------------------------------------------------------------------------------------------\n");
	if(strlen(choice) != 1)    // this is here as a precaution if user enters a bunch of characters instead of 1-8
		return 10;    // then this will trigger the default case, so program won't stop
	else
		return (int)(choice[0] - '0');    // if only one character is entered, convert it to integer and return
}


void write_in_file() {
	char fname[20], buff[80], c, extra[5];
	int n;
	printf("Enter file name:- ");
	scanf("%s", fname);
	fgets(extra, 4, stdin);    // for newline bug
	FILE *fp;

	fp = fopen(fname, "r");
	if(fp != NULL && fgetc(fp) != EOF) {    // check if file with fname already exists AND IF IT IS NOT EMPTY
		printf("**A non empty file with same name already exists. Please try again**\n");
		fclose(fp);
		return ;
	}

	fp = fopen(fname, "w");
	if(fp == NULL) {
	    printf("File NOT created!\n");
	    remove(fname);
	    fclose(fp);
	    return ;    //exit from the function if file is not created.
	}

	printf("enter number of lines to write: ");
	scanf("%d", &n);
	fgets(extra, 4, stdin);    // FIXES NEWLINE BUG DO NOT REMOVE
	printf("<Start writing below: >\n");
	for(int i=0; i<n; i++){
		fgets(buff, 79, stdin);

		if(i == n-1) {    // IF WE ARE AT THE LAST LINE, WE DONT WANT TO HAVE AN EXTRA NEWLINE AT END
			int pos = 0;
			char letter = buff[0];
			while(letter != '\n') {    // SO WHEN WE ARE ON LAST LINE, WRITE EVERYTHING EXCEPT NEWLINE AT THE END
				fputc(letter, fp);
				pos++;    // this is the position of letter in buff
				letter = buff[pos];
			} break;    // after writing last line there is nothing more to write, so break
		}

		fprintf(fp, "%s", buff);    // write buff in file
	}
	fclose(fp);
}


void editFile() {
	int ans;
	char extra, choice;
	fprintf(stdout, "1. Replace a line in a file\n2. Append lines to the end of a file\n3. Delete a line in a file\nChoose(1/2/3): ");
	choice = fgetc(stdin);
	extra = fgetc(stdin);    // for newline bug
	if(choice == '1')
		replaceLine();
	else if(choice == '2')
		appendLine();
	else if(choice == '3')
		deleteLine();
	else
		fprintf(stdout, "That is not a choice. Please try again.\n");
}


void replaceLine() {
	FILE *fp1, *fp2;
	char fname[20], content[35], temp[] = "temp.txt";
	char extra;    //FIXES NEWLINE BUG. DO NOT REMOVE
	/*WHEN fscanf IS USED AND USER PRESSES ENTER, A NEWLINE AUTOMATICALLY GOES INTO THE NEXT INPUT.
	TO AVOID THIS, I PUT AN EXTRA INPUT WHICH TAKES THAT \n.
	THIS IS USED IN SOME OTHER PLACES AS WELL. DO NOT REMOVE*/

	/* this usually happens when fscanf and scanf are used togethor */

	int line_num;

	fprintf(stdout, "Enter file name: ");
	scanf("%s", fname);
	extra = fgetc(stdin);    // for newline bug

	fp1 = fopen(fname, "r");    // check if file exists or can be opened
	if (fp1 == NULL) {
		fprintf(stdout, "\nERROR! Cannot open file\n**Please try again**");
		// fclose(fp1);    // DONT DO THIS HERE => segmentation fault when fclose on file that doesn't exist
		return ;
	}

	// Ask for line number when file name is valid
	fprintf(stdout, "Which line do you want to replace(ENTER LINE NUMBER): ");
	fscanf(stdin, "%d", &line_num);
	extra = fgetc(stdin);    // for newline bug

	if(line_num < 1 || line_num > total_lines(fname)) {
		printf("\nThis line does not exist in %s", fname);
		printf("\n**Please enter a valid number**");
		return ;
	}

	// Then ask for new content
	fprintf(stdout, "New content: ");
	fgets(content, 34, stdin);

	fp2 = fopen(temp, "w");    // WRITE INTO TEMP FROM GIVEN FILE THEN RENAME LATER

	int current_line = 1;
	char c = fgetc(fp1);
	while(c != EOF) {
		// WRITE INTO FILE IF CURRENT LINE IS NOT USER GIVEN LINE
		if(current_line != line_num)
			fputc(c, fp2);

		else {
			// when user given line is encountered SKIP THE WHOLE LINE
			while(c != '\n' && c != EOF) {    // IF WE ARE EDITING THE LAST LINE, IT WILL HAVE EOF AT THE END, NOT \n, all other lines end with \n
				c = fgetc(fp1);
			}

			if(c == EOF)    // IF WE ARE EDITING LAST LINE, WE DONT WANT TO PUT AN EXTRA \n AT THE END
				content[strlen(content) - 1] = '\0';    // CHOP OFF NEWLINE AT END
			// now write 'content' into temp file
			fputs(content, fp2);
		}
		if(c == '\n')    // this means we reached the end of a line
			current_line++;    // so increment current_line
		if(c != EOF)
			c = fgetc(fp1);
	}
	fclose(fp1);
	fclose(fp2);
	remove(fname);    // remove original file
	rename(temp, fname);    // rename temp to original file
	fprintf(stdout, "\n**changes saved**");
}


void appendLine() {
	FILE *fp;
	char fname[20], content[35], temp[] = "temp.txt";
	char extra;
	int lines;
	fprintf(stdout, "Enter file name: ");
	scanf("%s", fname);
	extra = fgetc(stdin);    // fixes newline bug

	fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stdout, "\nERROR! Cannot open file\n**Please try again**");
		return ;
	}

	// Ask for line number when file is valid
	fprintf(stdout, "How many lines do you want to append?: ");
	fscanf(stdin, "%d", &lines);
	extra = fgetc(stdin);    // for newline bug

	if(lines <= 0) {
		printf("\n**Please Enter a positive number**\n**Try again**");
		return ;
	}

	fp = fopen(fname, "a");
	fputc('\n', fp);
	for(int i=0; i<lines; i++) {
		fprintf(stdout, "Content for line %d: ", i+1);
		fgets(content, 79, stdin);
		if(i == lines - 1)    // FOR THE LAST LINE WE DONT WANT A NEWLINE AT END
			content[strlen(content) - 1] = '\0';    // so chop off newline
		fputs(content, fp);
	}
	fclose(fp);
	fprintf(stdout, "\n**changes saved**");
}


void deleteLine() { // we will copy all lines, except the one we want to delete, in a new file: temp.txt, then delete original file and rename the temp file
	FILE *fp1, *fp2;
	char fname[20], c, temp[]="temp.txt";
	char extra;
	int line, current_line;
	fprintf(stdout, "Enter file name: ");
	scanf("%s", fname);
	extra = fgetc(stdin);    // for newline bug

	fp1 = fopen(fname, "r");
	if (fp1 == NULL) {    // checking if file exists
		fprintf(stdout, "\nERROR! Cannot open file\n**Please try again**");
		return ;
	}

	fprintf(stdout, "Which line do you want to delete?: ");
	fscanf(stdin, "%d", &line);
	extra = fgetc(stdin);    // for nelwine bug

	if(line < 1 || line > total_lines(fname)) {
		printf("\nThis line does not exist in %s", fname);
		printf("\n**Please enter a valid number**");
		return ;
	}

	fp2 = fopen(temp, "w");
	current_line = 1;
	c = fgetc(fp1);
	while(c != EOF) {
		if(c == '\n' && current_line == line-1 && line == total_lines(fname)) { // this part if only used if we are deleting last line
			// if we are deleting the last line we have to remove the \n at the end of second last line so we dont end up with an extra line at the end
			c = fgetc(fp1);
			break;
		}

		// WRITE INTO FILE IF CURRENT LINE IS NOT USER GIVEN LINE
		if(current_line != line) {
			fputc(c, fp2);
		}
		else {
			// when user given line is encountered SKIP THE WHOLE LINE
			while(c != '\n')
				c = fgetc(fp1);
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
	fprintf(stdout, "\n**changes saved**");
}


void saveAs() {
	FILE *fp1, *fp2;
	char fname[20], new_fname[20];
	char extra;
	printf("Enter file name: ");
	scanf("%s", fname);
	extra = fgetc(stdin);    // for newline bug
	printf("Enter new file name: ");
	scanf("%s", new_fname);
	extra = fgetc(stdin);    // for newline bug

	fp1 = fopen(fname, "r");
	fp2 = fopen(new_fname, "w");    // create new file
	if(fp1 == NULL) {
	    printf("**\nError 404! Input File not Found**");
	    remove(fname);
	    remove(new_fname);
	    return ;
	}
	char c;
	c = fgetc(fp1);
	while(c != EOF) {
		fputc(c, fp2);    // copy contents in new file
		c = fgetc(fp1);
	}

	fclose(fp1);
	fclose(fp2);
	printf("\n**changes saved**");
}


void Ctrl_F() {
	char search[20], fname[20];
	char extra;
	printf("Enter file name to be searched: ");
	scanf("%s", fname);
	extra = fgetc(stdin);    // for newline bug

	FILE *fp;
	fp = fopen(fname, "r");
	if(fp == NULL) {
	    printf("\n**File NOT found!**");
	    return ;
	}

	// ask for search query when file opened successfully
	printf("Enter word/pattern to be searched: ");
	scanf("%s", search);
	extra = fgetc(stdin);

	char line_content[80];
	int count = 0, current_line = 0;
	printf("<Search results:>\n");
    while(fgets(line_content, 79, fp) != NULL) {
    	for(int i=0; i<strlen(line_content); i++) {
			bool found = 0;
			if(line_content[i] == search[0]) {    // if current letter in file matches first letter of search then start looking whether next few letters also match or not
				found = 1;
				for(int j=1; j<strlen(search); j++) {    // now we loop through the remaining letters in search word
					if(search[j] != line_content[i+j]){
						found=0;
						break;    // if current letter does not match with corresponding letter in search, break the loop and found=0
					}
				}
				if(found) {
					printf("{{%s}}", search);    // if word was found then print it in curly braces
					i += strlen(search) - 1;    // then increment i by length of search word because we dont need to print the found word again
					count++;
				}
			}
			if(!found)
				printf("%c", line_content[i]);    // print the current letter ONLY IF WORD WAS NOT FOUND
									  /* if word was found, we already printed it and now c is the last letter of that word, 
										so no need to print c*/
		}
    }
    printf("\n\n**The pattern '%s' occured in the file %d times.**", search, count);

	fclose(fp);
}


void compareFiles() {
	FILE *fp1, *fp2;
	char fname1[20], fname2[20];
	char extra;
	bool flag = 1;

	printf("Enter name of file 1: ");
	scanf("%s", fname1);
	extra = fgetc(stdin);
	printf("Enter name of file 2: ");
	scanf("%s", fname2);
	extra = fgetc(stdin);
	fp1 = fopen(fname1, "r");
	fp2 = fopen(fname2, "r");

	if(fp1 == NULL || fp2 == NULL) {
	    printf("\n**Input file(s) not found!**");
	    return ;
	}

	char c1, c2;
	do {
	    c1 = fgetc(fp1);
	    c2 = fgetc(fp2);    // getting characters from both files and checking if they match
	    if(c1 != c2) {
			flag = 0;
			break;    // if any character does not match break the loop
	    }
	} while(c1 != EOF && c2 != EOF);    // getting characters from files until we reach EOF in one or both

	if(flag == 1)
		printf("\n**Files are exactly similar**");
	else
		printf("\n**Files are NOT exactly similar**");
	fclose(fp1);
	fclose(fp2);
}


void del() {
	char fname[20], extra;
	printf("Enter file name to be deleted: ");
	scanf("%s",fname);
	extra = fgetc(stdin);    // handles newline bug
	if(!remove(fname))    //remove function returns 0 when it deletes file successfully.
		printf("\n**%s is DELETED SUCCESSFULLY**", fname);
	else {
		printf("\n**FILE NOT FOUND**");
		printf("\n**%s CANNOT be DELETED.**", fname);
	}
}


void readFile() {
    FILE *fp;
    char c, fname[20], extra;

    printf("Enter file name: ");
    scanf("%s", fname);
    extra = fgetc(stdin);    // for newline bug
    fp = fopen(fname, "r");    // open file in read mode
    if(fp == NULL) {
        printf("\n**File NOT found!**");
        remove(fname);
        // fclose(fp);
        return ;
    }

    printf("<FILE CONTENTS BELOW: >\n***********************\n");
	c = fgetc(fp);
	while(c != EOF) {
		printf("%c", c);
		c = fgetc(fp);
	}
	fclose(fp);
}


int total_lines(char fname[]) {    // returns total number of lines in a file
	FILE *fp = fopen(fname, "r");
	int tot_lines = 1;
	char c = fgetc(fp);
	while(c != EOF) {
		if(c == '\n')
			tot_lines++;
		c = fgetc(fp);
	}
	return tot_lines;
}

