#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    const long filesize = 256000;
    uint8_t fc[1] = {0xE5};
    FILE *fp = NULL;
    long off;
    long filler;

    if (argc != 2) {
        printf("Usage: makedisk <filename>\n");
        exit(1);
    }

	printf("Filename: %s\n", argv[1]);

    fp=fopen(argv[1], "rb");
    if (fp==NULL) {
        printf("Error: could find file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (fseek(fp, 0, SEEK_END) == -1) {
		printf("failed to fseek %s\n", argv[1]);
        exit(EXIT_FAILURE);
 	}
    off = ftell(fp);
    if (off == -1) {
		printf("failed to ftell %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("[*] size of file %s is %ld\n", argv[1], off);
    filler = filesize - off;
	printf("[*] need to add %ld characters\n", filler);
    fclose(fp);

    fp=fopen(argv[1], "ab");
    fseek(fp, 0, SEEK_END);
    for (long i = 0; i < filler; i++) {
        fwrite(fc, sizeof(uint8_t), 1, fp);
	}
    fclose(fp);
}
