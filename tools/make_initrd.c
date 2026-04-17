/* tools/make_initrd.c — Host tool to create the initial RAM disk image
 *
 * Usage: ./make_initrd <out_file> <in_file1> <name1> <in_file2> <name2> ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned int nfiles;
} initrd_header_t;

typedef struct {
    unsigned int magic;
    char name[64];
    unsigned int offset;
    unsigned int length;
} initrd_file_header_t;

int main(int argc, char **argv) {
    if (argc < 4 || (argc - 2) % 2 != 0) {
        printf("Usage: %s <out_file> <in_file1> <name1> <in_file2> <name2> ...\n", argv[0]);
        return 1;
    }

    int nfiles = (argc - 2) / 2;
    initrd_header_t header = { .nfiles = nfiles };
    initrd_file_header_t *fheaders = malloc(sizeof(initrd_file_header_t) * nfiles);

    FILE *out = fopen(argv[1], "wb");
    if (!out) { printf("Error opening %s for writing\n", argv[1]); return 1; }

    /* Skip headers for now, we'll write them later */
    fseek(out, sizeof(initrd_header_t) + sizeof(initrd_file_header_t) * nfiles, SEEK_SET);

    unsigned int current_offset = sizeof(initrd_header_t) + sizeof(initrd_file_header_t) * nfiles;

    for (int i = 0; i < nfiles; i++) {
        char *in_path = argv[i * 2 + 2];
        char *internal_name = argv[i * 2 + 3];

        FILE *in = fopen(in_path, "rb");
        if (!in) { printf("Error opening %s for reading\n", in_path); return 1; }

        fseek(in, 0, SEEK_END);
        unsigned int length = ftell(in);
        fseek(in, 0, SEEK_SET);

        unsigned char *data = malloc(length);
        fread(data, 1, length, in);
        fclose(in);

        fwrite(data, 1, length, out);
        free(data);

        fheaders[i].magic = 0xBF;
        strncpy(fheaders[i].name, internal_name, 63);
        fheaders[i].offset = current_offset;
        fheaders[i].length = length;

        current_offset += length;
        printf("Adding file [%s] as [%s], size %d, offset %d\n", in_path, internal_name, length, fheaders[i].offset);
    }

    /* Back to beginning and write headers */
    fseek(out, 0, SEEK_SET);
    fwrite(&header, 1, sizeof(initrd_header_t), out);
    fwrite(fheaders, sizeof(initrd_file_header_t), nfiles, out);

    fclose(out);
    free(fheaders);

    printf("Successfully created initrd archive: %s\n", argv[1]);
    return 0;
}
