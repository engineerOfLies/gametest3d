#include "model.h"

#define __model_max 1024

static Model ModelList[__model_max];

static void model_close();

void model_init()
{
    memset(ModelList,0,sizeof(Model)*__model_max);
    atexit(model_close);
}

static void model_delete(Model *model)
{
    if (!model)return;
    
    FreeSprite(model->texture);
    if (model->vertex_array)
    {
        free(model->vertex_array);
    }
    if (model->texel_array)
    {
        free(model->texel_array);
    }
    if (model->normal_array)
    {
        free(model->normal_array);
    }
    if (model->triangle_array)
    {
        free(model->triangle_array);
    }
    memset(model,0,sizeof(Model));
}

void model_free(Model *model)
{
    if (!model)return;
    model->used--;
    if (model->used > 0)return;
    model_delete(model);
}

static void model_close()
{
    int i;
    for (i = 0; i < __model_max; i++)
    {
        if (ModelList[i].used)
        {
            model_delete(&ModelList[i]);
        }
    }
}

Model *model_new()
{
    int i;
    for (i = 0; i < __model_max; i++)
    {
        if (ModelList[i].used == 0)
        {
            memset(&ModelList[i],0,sizeof(Model));
            ModelList[i].used = 1;
            return &ModelList[i];
        }
    }
    return NULL;
}

Model *model_get_by_filename(char *filename)
{
    int i;
    for (i = 0; i < __model_max; i++)
    {
        if ((ModelList[i].used != 0) &&
            (strcmp(ModelList[i].filename,filename) == 0))
        {
            memset(&ModelList[i],0,sizeof(Model));
            ModelList[i].used = 1;
            return &ModelList[i];
        }
    }
    return NULL;
}

void model_assign_texture(Model *model,char *texture)
{
    Sprite *sprite;
    if (!model)return;
    sprite = LoadSprite(texture,-1,-1);
    if (!sprite)return;
    model->texture = sprite;
}

size_t model_get_triangle_buffer_size(Model *model)
{
    if (!model)return 0;
    return (sizeof(Triangle)*model->num_tris);
}

size_t model_get_vertex_buffer_size(Model *model)
{
    if (!model)return 0;
    return (sizeof(float)*3*model->num_vertices);
}

/*eol@eof*/
