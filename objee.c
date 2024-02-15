#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef int Size;

#ifndef OBJEE_BUFFER_SIZE
#define OBJEE_BUFFER_SIZE 1024
#endif

#define RED "\033[31m"
#define NORMAL "\033[0m"

typedef struct {
    Size length;
    Size capacity;
} ArrayHeader;

#define objee_array(T, a) objee_array_init(sizeof(T), ARRAY_INIT_SIZE, a)
#define objee_arrayT(T) T*

#define objee_array_header(a) ((ArrayHeader*)(a) - 1)
#define objee_array_length(a) (array_header(a)->length)
#define objee_array_capacity(a) (array_header(a)->capacity)

#define objee_array_append(a, v) ( \
        (a) = array_ensure_capacity(a, 1, sizeof(v)), \
        (a)[array_header(a)->length] = (v), \
        &(a)[array_header(a)->length++])

static void* objee_array_init(Size item_size, Size capacity)
{
    Size total_size = item_size * capacity * sizeof(ArrayHeader);
    ArrayHeader* h = malloc(total_size);

    if (h) {
        h->capacity = capacity;
        h->length = 0;
        h++;
    }

    return h;
}

static void* objee_array_ensure_capacity(void* a, Size item_count, Size item_size) {
    ArrayHeader* h = objee_array_header(a);
    Size desired_capacity = h->length + item_count;

    if (h->capacity < desired_capacity) {
        Size new_capacity = h->capacity * 2;
        while (new_capacity < desired_capacity) {
            new_capacity *= 2;
        }
        h->capacity = new_capacity;

        Size new_size = sizeof(ArrayHeader) + new_capacity * item_size;
        h = realloc(h, new_size);
    }

    h++;
    return h;
}

typedef struct {
    char* path;
    FILE* file;

    int v_count;
    int vt_count;
    int vn_count;
    int f_count;

    objee_arrayT(float) verts;
    objee_arrayT(float) normals;
    objee_arrayT(float) tex_coords;
} ObjeeInfo;

void objee_get_object_info(ObjeeInfo* info) {
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

void objee_parse_object(ObjeeInfo* info, ObjeeObject* obj)
{
    char line[OBJEE_BUFFER_SIZE];
    int v = 0;
    int vn = 0;
    int vt = 0;
    // int f = 0;

    /* float* verts = malloc(info.v_count * 3 * sizeof(float)); */
    /* float* tex_coords = malloc(info.vt_count * 2 * sizeof(float)); */
    /* float* normals = malloc(info.vn_count * 3 * sizeof(float)); */

    int ret = 0;

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
                        ret = sscanf(line, "v %f %f %f",
                                &obj->verts[v + 0],
                                &obj->verts[v + 1],
                                &obj->verts[v + 2]);
                        assert(ret == 3);
                        v += 3;
                        break;
                    case 'n':
                        printf("vn: %s", line);
                        ret = sscanf(line, "vn %f %f %f",
                                &obj->normals[vn + 0],
                                &obj->normals[vn + 1],
                                &obj->normals[vn + 2]);
                        assert(ret == 3);
                        vn += 3;
                        break;
                    case 't':
                        printf("vt: %s", line);
                        ret = sscanf(line, "vt %f %f",
                                &obj->tex_coords[vt + 0],
                                &obj->tex_coords[vt + 1]);
                        assert(ret == 2);
                        vt += 2;
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
    ObjeeInfo info = {
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

    ObjeeObject object;
    object.verts = (float*)malloc(info.v_count * 3 * sizeof(float));
    object.tex_coords = (float*)malloc(info.vt_count * 2 * sizeof(float));
    object.normals = (float*)malloc(info.vn_count * 3 * sizeof(float));

    char buffer[2048];

    objee_get_object_info(&info);
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
