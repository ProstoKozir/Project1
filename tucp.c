#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

static int copy_file(const char *src, const char *dst) {
    FILE *in = fopen(src, "rb");
    if (!in) { 
        perror("open src"); 
        return -1; 
    }

    FILE *out = fopen(dst, "wb");
    if (!out) { 
        perror("open dest"); fclose(in); 
        return -1; 
    }

    char buf[2048]; // 32 KiB buffer
    size_t n;
    while ((n = fread(buf, 1, sizeof buf, in)) > 0) {
        if (fwrite(buf, 1, n, out) != n) {
            perror("write"); 
            fclose(in); 
            fclose(out);
            return -1;
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        puts("Wrong input");
        return 1;
    }

    const char *dst_arg = argv[argc - 1];
    struct stat st;
    int dst_is_dir = (stat(dst_arg, &st) == 0) && S_ISDIR(st.st_mode);

    if (argc == 3) {
        const char *src = argv[1];
        char dst_path[PATH_MAX];

        if (dst_is_dir) {
            const char *base = strrchr(src, '/');
            base = base ? base + 1 : src;

            snprintf(dst_path, sizeof dst_path, "%s/%s", dst_arg, base);
            return copy_file(src, dst_path);
        } else {
            return copy_file(src, dst_arg);
        }
    } else {
        if (!dst_is_dir) {
            puts("Wrong input");
            return 1;
        }


        for (int i = 1; i < argc - 1; ++i) {
            const char *src = argv[i];
            char dst_path[PATH_MAX];

            const char *base = strrchr(src, '/');
            base = base ? base + 1 : src;

            snprintf(dst_path, sizeof dst_path, "%s/%s", dst_arg, base);
            copy_file(src, dst_path);
        }
        return 0;
    }
}