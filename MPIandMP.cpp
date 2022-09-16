#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <omp.h>
#define SIZE 2

int main(int argc, char* argv[]) {

	int numtasks, rank, sendcount, recvcount, source;
	char buff_found_pass[56];
	char buff_recv_pass[112];

	// Initialize the MPI environment
	MPI_Init(&argc, &argv);

	// Get the number of processes
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	// Get the rank of the process
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	// Get the name of the processor
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);
	//--------------------------------------------------

	//printf("Hello world from processor %s, rand %d out of %d processors\n", processor_name, world_rank, world_size);

	//my alphabet
	char alphabet[63] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
	char mypassword[5] = "";

	FILE* fp;
	fp = fopen("passwords.txt", "w+");


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


	FILE* fp2 = fopen("passwords.txt", "r+");
	if (!fp2) {
		printf("\n Unable to open : %s ", "passwords.txt");
		return -1;
	}

	//get number of passwords
	int ch = 0;
	int no_lines = 1;
	while ((ch = fgetc(fp2)) != EOF)
	{
		if (ch == '\n')
			no_lines++;
	}
	//printf("The no of lines in the file is %d\n", no_lines);
	fseek(fp2, 0, SEEK_SET);


	//buffer for all the passwords and a buffer for half of the passwords
	char* buffer_passwords2 = (char*)malloc(sizeof(char) * 5 * no_lines);
	char* recvbuf2 = (char*)malloc(sizeof(char) * 5 * (no_lines / 2));


	char line0[6];
	int start = 0;
	for (int i = 0; i < no_lines; i++)
	{
		fgets(line0, sizeof(line0), fp2);
		//printf("%s\n", line0);
		char line[5];
		strncpy(line, line0, 4);
		line[4] = '\0';

		start += 5;
		int j = 0;
		for (int i = start; i < start + 5; i++)
		{

			buffer_passwords2[i] = line[j];
			//printf("%c", buffer_passwords2[i]);
			j++;
		}
		//printf("\n");
	}
	/////////////////////////////////////////////////////////////////////
	if (world_size == SIZE) {
		source = 1;
		sendcount = 5 * (no_lines / 2);
		recvcount = 5 * (no_lines / 2);

		MPI_Scatter(buffer_passwords2, sendcount, MPI_CHAR, recvbuf2, recvcount, MPI_CHAR, source, MPI_COMM_WORLD);
		//printf("\nScatter result is:");


		/// ///////////////////////////////////////////////////


		FILE* fp8;
		char name[21] = "passwords_node_";
		char number[2];
		sprintf(number, "%d", world_rank);
		strcat(name, number);
		strcat(name, ".txt");
		fp8 = fopen(name, "w+");

		char line00[5] = "";


		for (int i = 0; i < 5 * (no_lines / 2); i++)
		{
			if (i % 5 == 0)
			{
				//printf("%s\n", line00);

				fputs(line00, fp8);
				fputs("\n", fp8);

				memset(line00, '0', 5 * sizeof(char));
			}

			line00[i % 5] = recvbuf2[i];
		}

		fclose(fp8);


		//////////////////////////////////////////////
		if (world_rank == 0) {

			double tStart0 = omp_get_wtime();
			int ok0 = 0;
			char line0[6];
			FILE* fp8;
			fp8 = fopen("passwords_node_0.txt", "r");
#pragma omp parallel firstprivate(line0)
			{
#pragma omp  for	


				//-------------------------------------------------------------------

				for (int i = 0; i < no_lines; i++)
				{
					fgets(line0, sizeof(line0), fp8);

					char line[5];
					strncpy(line, line0, 4);
					line[4] = '\0';
					
						char command[26] = "7z x protected.zip -p";
						strcat(command, line);
						if (ok0 == 0)remove("test.txt");
						int x = system(command);

						//printf("On the process %d , thread nr %d gets the password %s\n", world_rank, omp_get_thread_num(), line);
						

						//--------------------------------------

						if (x == 0)
						{
							strcpy(buff_found_pass, line);
							ok0 = 1;
							//printf("\n!!!! THE PASSWORD IS : %s !!!!\n", line);

							double tFinal0 = omp_get_wtime();
							printf("\nRequired time (benchmark time) for parallel is = %f seconds\n", tFinal0 - tStart0);


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
									strcat(buff_found_pass, " ");
									strcat(buff_found_pass, line2);
									//printf("THE UNZIP TEXT SAYS : %s\n", line2);

								}


							}
							fclose(fp3);


						}

						if (ok0 == 1) {
							break;
							//#pragma omp cancel for
							//#pragma omp cancel parallel
						}


					

				}

			}
		}
		else {
			double tStart1 = omp_get_wtime();
			int ok1 = 0;
			char line1[6];
			FILE* fp8;
			fp8 = fopen("passwords_node_1.txt", "r");

#pragma omp parallel firstprivate(line1)
			{
#pragma omp  for	


				//-------------------------------------------------------------------

				for (int i = 0; i < no_lines; i++)
				{
					fgets(line1, sizeof(line1), fp8);

					char line[5];
					strncpy(line, line1, 4);
					line[4] = '\0';
					
						//printf("On the process %d , thread nr %d gets the password %s\n", world_rank, omp_get_thread_num(), line);


						//---------------------------------------
						char command[26] = "7z x protected.zip -p";
						strcat(command, line);
						if (ok1 == 0)remove("test.txt");
						int x = system(command);
						if (x == 0)
						{
							strcpy(buff_found_pass, line);
							ok1 = 1;
							//printf("\n!!!! THE PASSWORD IS : %s !!!!\n", line);

							double tFinal1 = omp_get_wtime();
							printf("\nRequired time (benchmark time) for parallel is = %f seconds\n", tFinal1 - tStart1);


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
									strcat(buff_found_pass, " ");
									strcat(buff_found_pass, line2);
									//printf("THE UNZIP TEXT SAYS : %s\n", line2);

								}


							}
							fclose(fp3);


						}

						if (ok1 == 1) {
							break;
							//#pragma omp cancel for
							//#pragma omp cancel parallel
						}


					

				}

			}



		}
		//////////////////////////////////////////////////////////////////////

		MPI_Gather(buff_found_pass, 56, MPI_CHAR, buff_recv_pass, 56, MPI_CHAR, 0,
			MPI_COMM_WORLD);

		if (world_rank == 0)
		{
			printf("At gather level we received the pass and the text unziped as it follows:  %s", buff_recv_pass);
		}

		MPI_Finalize();
	}

	//b) Sequentially

	fp2 = fopen("passwords.txt", "r+");
	if (!fp2) {
		printf("\n Unable to open : %s ", "passwords.txt");
		return -1;
	}
	double tStart0 = omp_get_wtime();
	double tFinal0;

	char line1[5];
	while (fgets(line1, sizeof(line1), fp2))
	{
		//printf("%s", line1);

		char command[26] = "7z x protected.zip -p";
		
		strcat(command, line1);

		remove("test.txt");
		if (system(command) == 0)
		{
			tFinal0 = omp_get_wtime();
			break;
		}

	}
	printf("\nRequired time (benchmark time) for sequential is = %f seconds\n", tFinal0 - tStart0);
	printf("The password found in sequential is %s", line1);
	fclose(fp2);

	return 0;

}