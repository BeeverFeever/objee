#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef OBJEE_BUFFER_SIZE
#define OBJEE_BUFFER_SIZE 1024
#endif

#define RED "\033[31m"
#define NORMAL "\033[0m"

typedef struct {
    float* verts;
    float* tex_coords;
    float* normals;
} objee_object;

typedef struct {
    char* path;
    FILE* file;

    int v_count;
    int vt_count;
    int vn_count;
    int f_count;
} objee_info;

void objee_get_object_info(objee_info* info, char* buffer, int length) {
    info->v_count = 0;
    info->vn_count = 0;
    info->vt_count = 0;
    info->f_count = 0;
    char line[OBJEE_BUFFER_SIZE];

    while (fgets(line, OBJEE_BUFFER_SIZE, info->file)) {
        /* printf(RED "DEBUG INFO:\n" NORMAL); */
        /* printf("\tline: %s\n", line); */

        switch (line[0]) {
            case '#':
                break;
            case 'v':
                switch (line[1]) {
                    case ' ':
                        info->v_count++;
                        break;
                    case 'n':
                        info->vn_count++;
                        break;
                    case 't':
                        info->vt_count++;
                        break;
                    default:
                        break;
                }
                break;
            case 'f':
                info->f_count++;
                break;
            default:
                printf(RED"ERROR: unknown symbol: %s"NORMAL, line);
                break;
        }
    }

    rewind(info->file);
}

/*
 * You are required to create your own fucking memory, this library doesn't fuck
 * around with memory shit so suck your own dick.
 * Oh also, call objee_get_object_info first to get the sizes for your memory :)
 */
void objee_parse_object(objee_info info, objee_object* obj)
{
    char line[OBJEE_BUFFER_SIZE];

    while (fgets(line, OBJEE_BUFFER_SIZE, info.file)) {
        /* printf(RED "DEBUG INFO:\n" NORMAL); */
        /* printf("\tline: %s\n", line); */

        int v = 0;
        int vn = 0;
        int vt = 0;
        int f = 0;

        switch (line[0]) {
            case '#':
                break;
            case 'v':
                switch (line[1]) {
                    case ' ':
                        sscanf(line, "v %f %f %f", obj->verts[v].x, obj->verts[v].y, obj->verts[v].z);
                        break;
                    case 'n':
                        break;
                    case 't':
                        break;
                    default:
                        break;
                }
                break;
            case 'f':
                break;
            default:
                printf(RED"ERROR: unknown symbol: %s"NORMAL, line);
                break;
        }
    }
}

int main(void)
{
    objee_info info = {
        .path = "teapot.obj",
        .file = fopen(info.path, "r"),
        .v_count = 0,
        .vn_count = 0,
        .vt_count = 0,
        .f_count = 0,
    };
    if (info.file == NULL) {
        printf(RED);
        perror(info.path);
        exit(1);
    }

    char buffer[2048];

    objee_get_object_info(&info, buffer, 2048);

    puts("");
    printf(RED"DEBUG INFO:\n"NORMAL);
    printf("\tv_count: %i\n", info.v_count);
    printf("\tvn_count: %i\n", info.vn_count);
    printf("\tvt_count: %i\n", info.vt_count);
    printf("\tf_count: %i\n", info.f_count);

    return 0;
}
