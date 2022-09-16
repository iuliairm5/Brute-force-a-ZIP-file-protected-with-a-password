#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//#include <mpi.h>
#include <time.h>
#include <omp.h>

int main(int argc, char* argv[]) {

	char zipfilename[14] = "";
	strcpy(zipfilename, argv[1]);
	//printf("%s\n", zipfilename);

	//my alphabet
	char alphabet[63] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";

	//my text file with all the possible passwords/combinations
	FILE* fp;
	fp = fopen("passwords.txt", "w");

	char mypassword[5] = "";

	for (int i = 0; i < 62; i++)
	{

		mypassword[0] = alphabet[i];

		for (int j = 0; j < 62; j++)
		{
			mypassword[1] = alphabet[j];

			for (int k = 0; k < 62; k++)
			{
				mypassword[2] = alphabet[k];

				for (int m = 0; m < 62; m++)
				{
					mypassword[3] = alphabet[m];
					//printf("%s\n", mypassword);
					fputs(mypassword, fp);
					fputs("\n", fp);
				}
			}
		}

	}
	fclose(fp);

	//read passwords from the text file



	FILE* fp2 = fopen("passwords.txt", "r+");
	if (!fp2) {
		printf("\n Unable to open : %s ", "passwords.txt");
		return -1;
	}


	int ch = 0;
	int no_lines = 1;
	char c;
	/*while ((ch = fgetc(fp)) != EOF)
	{
		if (ch == '\n')
			no_lines++;
	}*/
	for (c = getc(fp); c != EOF; c = getc(fp))
	{
		if (c == '\n') 
			no_lines = no_lines + 1;
	}

	printf("The no of lines in the file is %d\n", no_lines);
	fseek(fp, 0, SEEK_SET);
	//fclose(fp2);


	/*
		long SIZE = no_lines * 4 * sizeof(char);
		fseek(fp, 0L, SEEK_SET);
		char* text = (char*)malloc(sizeof(char) * SIZE);
		*/


	char line0[6];
	char line1[5];


	//a) Parallel
	omp_lock_t mutex;
	omp_init_lock(&mutex);


	double tStart = omp_get_wtime();
	int ok = 0;
#pragma omp parallel firstprivate(line0)
	{
#pragma omp  for	


		//-------------------------------------------------------------------

		for (int i = 0; i < no_lines; i++)
		{
			fgets(line0, sizeof(line0), fp2);

			char line[5];
			strncpy(line, line0, 4);
			line[4] = '\0';
			printf("Thread nr %d gets the password %s\n", omp_get_thread_num(), line);


			//------------------------------------------------------------------------------

			//char command[26] = "7z x protected.zip -p";
			char command[26] = "7z x ";
			strcat(command, zipfilename);
			strcat(command, " -p");
			strcat(command, line);

			if (ok == 0)remove("test.txt");


			int x = system(command);

			if (x == 0)
			{

				ok = 1;
				printf("\n!!!! THE PASSWORD IS : %s !!!!\n", line);

				double tFinal = omp_get_wtime();
				printf("\nRequired time (benchmark time) for parallel is = %f seconds\n", tFinal - tStart);


				FILE* fp3 = fopen("test.txt", "r+");

				//fseek(fp3, 0L, SEEK_END);

				//long int res = ftell(fp3);

				if (!fp3) {
					printf("Unable to open : %s \n", "test.txt");
				}
				else
				{

					char line2[255];
					while (fgets(line2, sizeof(line2), fp3))
					{
						printf("THE UNZIP TEXT SAYS : %s\n", line2);

					}


				}
				fclose(fp3);


			}

			if (ok == 1) break;




		}

	}


	fclose(fp2);

	//b) Sequentially

	fp2 = fopen("passwords.txt", "r+");
	if (!fp2) {
		printf("\n Unable to open : %s ", "passwords.txt");
		return -1;
	}
	double tStart0 = omp_get_wtime();
	double tFinal0;


	while (fgets(line1, sizeof(line1), fp2))
	{
		printf("%s", line1);
		
		//char command[26] = "7z x protected.zip -p";
		char command[26] = "7z x ";
		strcat(command, zipfilename);
		strcat(command, " -p");
		strcat(command, line1);

		remove("test.txt");
			if (system(command) == 0)
				{
					tFinal0 = omp_get_wtime();
					break;
				}

		}
	printf("Required time (benchmark time) for sequential is = %f seconds\n", tFinal0 - tStart0);
	printf("The password found in sequential is %s", line1);
	fclose(fp2);
	



	return 0;
}