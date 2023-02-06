#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <vector>

void showExample() {
	printf("\
\n \
  Example:\n \
\n \
    1) if use 'filename.exe 1000 0 0 input.cnc' then program reading file\n \
       'input.cnc' and writing file 'input_out.cnc' for output data\n\n \
    2) if use 'filename.exe 1000 0 0 input.cnc output.cnc' then program\n \
       reading file 'input.cnc' and writing data to 'output.cnc' file.\n \
\n \
	");
}

void showUsage() {
	printf("\
\n \
  Usage:\n \
\n \
    'filename.exe' show this help\n \
    'filename.exe deltaX deltaY deltaZ input_filename'\n \
    'filename.exe deltaX deltaY deltaZ input_filename output_filename'\n \
	");
	showExample();
}

int main(int argc, char *args[]) {
	FILE *fi, *fo;

	char fileName[1024]={0};
	char buf[1024];
	char *pch;

	switch (argc - 1) {
	  case 5:
		fi = fopen(args[4], "r");
		pch = args[5];
		break;
	  case 4:
		strcpy(fileName, args[4]);
		pch = strrchr(fileName, '.');
		if (NULL != pch)
			*pch = 0;
		pch = fileName;
		strcat(pch, "_out.cnc");
		fi = fopen(args[4], "r");
		pch = fileName;
		break;
	  default:
		showUsage();
		return 0;
	}

	if (NULL == fi) {
		printf("Error opening input file '%s'!\nprogram end.\n", args[3]);
		showExample();
		return 1;
	}

	int deltaX = atoi(args[1]);
	int deltaY = atoi(args[2]);
	int deltaZ = atoi(args[3]);

	fo = fopen(pch, "w");

	if (NULL == fo) {
		fclose(fi);
		printf("Error opening output file '%s'!\nprogram end.\n", pch);
		showExample();
		return 1;
	}

	printf("delta 'X': %d, delta 'Y': %d, delta 'Z': %d\n", deltaX, deltaY, deltaZ);

	float x = 0, y = 0, z = 0;
	bool isX = false, isY = false, isZ = false;
	int F=0, N, M;
	bool isF = false, isM = false;
	int line = 0, i;

	std::vector<int> G;
	int iG;

	int T = 1, S = 10000;
	bool isT = false, isS = false;

	do {
		if (fgets(buf, sizeof(buf), fi) == NULL)
			break;

		line++;

		if (buf[0] == '(') {
			fprintf(fo, "%s", buf);
			continue;
		}

		pch = buf;

		isX = isY = isZ = isF = isM = isT = isS = false;
		G.clear();

		for(i = 0; i < 1024; i++) {
			// end of line
			if ((buf[i] == 0) || (buf[i] == '\n') || (buf[i] == '\r'))
				break;

			if (buf[i] == 'G') {
				sscanf(pch+i+1, "%d", &iG);
				G.push_back(iG);
			}else if (buf[i] == 'T') {
				sscanf(pch+i+1, "%d", &T);
				isT = true;
			}else if (buf[i] == 'S') {
				sscanf(pch+i+1, "%d", &S);
				isS = true;
			}else if (buf[i] == 'M') {
				sscanf(pch+i+1, "%d", &M);
				isM = true;
			}else if (buf[i] == 'X') {
				sscanf(pch+i+1, "%f", &x);
				x += deltaX;
				isX = true;
			}else if (buf[i] == 'Y') {
				sscanf(pch+i+1, "%f", &y);
				y += deltaY;
				isY = true;
			}else if (buf[i] == 'Z') {
				sscanf(pch+i+1, "%f", &z);
				z += deltaZ;
				isZ = true;
			}else if (buf[i] == 'F') {
				sscanf(pch+i+1, "%d", &F);
				isF = true;
			}else if (buf[i] == 'N') {
				sscanf(pch+i+1, "%d", &N); // ignore for output
			} else if (!isdigit(buf[i]) && (buf[i] != '-') && (buf[i] != '.') && (buf[i] != ' ')) {
				printf("Error in input file! Break. Line No %d pos %d char '%c' - %s", line, i, buf[i], buf);
				fclose(fi);
				fclose(fo);
				exit(1);
			}
		}

		if (G.size() > 1) {
			for (unsigned int g = 0; g < G.size(); g++)
				fprintf(fo, "G%d", G[g]);
			fprintf(fo, "\n");
		} else if (isX || isY || isZ) {
			if (G.size() == 1)
				fprintf(fo, "G%d", G[0]);
			else {
				printf("Too many 'G' command's! Break. Line No %d - %s", line, buf);
				fclose(fi);
				fclose(fo);
				exit(2);
			}
			if (isX)
				fprintf(fo, " X%.3f", x);
			if (isY)
				fprintf(fo, " Y%.3f", y);
			if (isZ)
				fprintf(fo, " Z%.3f", z);
			if (isF)
				fprintf(fo, " F%d", F);
			fprintf(fo, "\n");
		} else if (isT && isS) {
			fprintf(fo, "T%d S%d\n", T, S);
		} else if (isM)
			fprintf(fo, "M%d\n", M);

	} while(!feof(fi));

	fclose(fi);
	fclose(fo);

	return 0;
}
