#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE * fp;
	unsigned long filelen;

	if (argc != 2) {
		printf("Usage: %s file2patch.bin\n", argv[0]);
		return -1;
	}

	fp = fopen(argv[1], "r+b");
	if (NULL == fp) {
		printf("Fail to open %s\n", argv[1]);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	filelen = ftell(fp);
	fseek(fp, 16L, SEEK_SET);

	if (fwrite(&filelen, 1, 4, fp) != 4) {
		printf("Fail to write %s\n", argv[1]);
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}