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
    float* normals;
    float* tex_coords;
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

void objee_parse_object(objee_info* info, objee_object* obj)
{
    char line[OBJEE_BUFFER_SIZE];
    int v = 0;
    int vn = 0;
    int vt = 0;
    // int f = 0;

    /* float* verts = malloc(info.v_count * 3 * sizeof(float)); */
    /* float* tex_coords = malloc(info.vt_count * 2 * sizeof(float)); */
    /* float* normals = malloc(info.vn_count * 3 * sizeof(float)); */

    while (fgets(line, OBJEE_BUFFER_SIZE, info->file)) {
        /* printf(RED "DEBUG INFO:\n" NORMAL); */
        /* printf("\tline: %s\n", line); */

        switch (line[0]) {
            case '#':
                break;
            case 'v':
                switch (line[1]) {
                    case ' ':
                        printf("v: %s", line);
                        sscanf(line, "v %f %f %f",
                                &obj->verts[v * 3 + 0],
                                &obj->verts[v * 3 + 1],
                                &obj->verts[v * 3 + 2]);
                        v++;
                        break;
                    case 'n':
                        printf("vn: %s", line);
                        sscanf(line, "vn %f %f %f",
                                &obj->normals[vn * 3 + 0],
                                &obj->normals[vn * 3 + 1],
                                &obj->normals[vn * 3 + 2]);
                        vn++;
                        break;
                    case 't':
                        printf("vt: %s", line);
                        sscanf(line, "vt %f %f",
                                &obj->tex_coords[vt * 2 + 0],
                                &obj->tex_coords[vt * 2 + 1]);
                        vt++;
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
    /**/
    /* puts(""); */
    /* for (int i = 0; i < info.v_count; i++) { */
    /*     printf("v %f %f %f\n", verts[i * 3 + 0], verts[i * 3 + 1], verts[i * 3 + 2]); */
    /* } */
    /* puts(""); */
    /* for (int i = 0; i < info.vt_count; i++) { */
    /*     printf("vt %f %f\n", tex_coords[i * 2 + 0], tex_coords[i * 2 + 1]); */
    /* } */
    /* puts(""); */
    /* for (int i = 0; i < info.vn_count; i++) { */
    /*     printf("vn %f %f %f\n", normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]); */
    /* } */
}

int main(void)
{
    objee_info info = {
        .path = "example.obj",
        .file = fopen(info.path, "r"),
        .v_count = 0,
        .vn_count = 0,
        .vt_count = 0,
        .f_count = 0,
    };
    if (info.file == NULL) {
        printf(RED);
        perror(info.path);
        printf(NORMAL);
        exit(1);
    }

    objee_object object;
    object.verts = (float*)malloc(info.v_count * 3 * sizeof(float));
    object.tex_coords = (float*)malloc(info.vt_count * 2 * sizeof(float));
    object.normals = (float*)malloc(info.vn_count * 3 * sizeof(float));

    char buffer[2048];

    objee_get_object_info(&info, buffer, 2048);
    objee_parse_object(&info, &object);

    puts("");
    for (int i = 0; i < info.v_count; i++) {
        printf("v %f %f %f\n", object.verts[i * 3 + 0], object.verts[i * 3 + 1], object.verts[i * 3 + 2]);
    }
    puts("");
    for (int i = 0; i < info.vt_count; i++) {
        printf("vt %f %f\n", object.tex_coords[i * 2 + 0], object.tex_coords[i * 2 + 1]);
    }
    puts("");
    for (int i = 0; i < info.vn_count; i++) {
        printf("vn %f %f %f\n", object.normals[i * 3 + 0], object.normals[i * 3 + 1], object.normals[i * 3 + 2]);
    }
    return 0;
}
