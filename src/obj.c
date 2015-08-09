#include "obj.h"
#include "simple_logger.h"

void obj_file_get_counts(Model* model, FILE* file)
{
    char buf[1024];
    int  numvertices = 0;
    int  numnormals = 0;
    int  numtexcoords = 0;
    int  numfaces = 0;
    
    if ((file == NULL) ||
        (model == NULL))
    {
        return;
    }
    while(fscanf(file, "%s", buf) != EOF)
    {
        switch(buf[0])
        {
            case 'v':
                switch(buf[1])
                {
                    case '\0':
                        fgets(buf, sizeof(buf), file);
                        numvertices++;
                        break;
                    case 'n':
                        fgets(buf, sizeof(buf), file);
                        numnormals++;
                        break;
                    case 't':
                        fgets(buf, sizeof(buf), file);
                        numtexcoords++;
                        break;
                    default:
                        break;
                }
                break;
            case 'f':
                fgets(buf, sizeof(buf), file);
                numfaces++;
                break;
            default:
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
    model->num_tris = numfaces;
    model->num_vertices = numvertices;
    model->num_texels = numtexcoords;
    model->num_normals = numnormals;
}

void obj_model_allocate(Model *model)
{
    if (!model)return;
    if (model->num_vertices)
    {
        model->vertex_array = malloc(sizeof(float)*3*model->num_vertices);
        if (model->vertex_array)
        {
            memset(model->vertex_array,0,sizeof(float)*3*model->num_vertices);
        }
    }
    if (model->num_normals)
    {
        model->normal_array = malloc(sizeof(float)*3*model->num_normals);
        if (model->normal_array)
        {
            memset(model->normal_array,0,sizeof(float)*3*model->num_normals);
        }
    }
    if (model->num_texels)
    {
        model->texel_array = malloc(sizeof(float)*2*model->num_texels);
        if (model->texel_array)
        {
            memset(model->texel_array,0,sizeof(float)*2*model->num_texels);
        }
    }
    if (model->num_tris)
    {
        model->triangle_array = malloc(sizeof(Triangle)*model->num_tris);
        if (model->triangle_array)
        {
            memset(model->triangle_array,0,sizeof(Triangle)*model->num_tris);
        }
    }
}

void obj_file_parse(Model * model, FILE* file)
{
    int  numvertices = 0;
    int  numnormals = 0;
    int  numtexcoords = 0;
    int  numfaces = 0;
    char buf[128];
    float x,y,z;
    GLuint t1,t2,t3,v1,v2,v3,n1,n2,n3;
        
    if (model == NULL)
    {
        return;
    }
    if (file == NULL)
    {
        return;
    }
    rewind(file);
    while(fscanf(file, "%s", buf) != EOF)
    {
        switch(buf[0])
        {
            case 'v':
                switch(buf[1])
                {
                    case '\0':
                        fscanf(
                            file,
                            "%f %f %f",
                            &x,
                            &y,
                            &z
                        );
                        model->vertex_array[numvertices++] = x;
                        model->vertex_array[numvertices++] = y;
                        model->vertex_array[numvertices++] = z;
                        break;
                    case 'n':
                        fscanf(
                            file,
                            "%f %f %f",
                            &x,
                            &y,
                            &z
                        );
                        model->normal_array[numnormals++] = x;
                        model->normal_array[numnormals++] = y;
                        model->normal_array[numnormals++] = z;
                        break;
                    case 't':
                        fscanf(
                            file,
                            "%f %f",&x,&y
                        );
                        model->texel_array[numtexcoords++] = x;
                        model->texel_array[numtexcoords++] = 1 - y;
                        break;
                    default:
                        break;
                }
                break;
            case 'f':
                fscanf(
                    file,
                    "%ui/%ui/%ui %ui/%ui/%ui %ui/%ui/%ui",
                    &v1,
                    &t1,
                    &n1,
                    &v2,
                    &t2,
                    &n2,
                    &v3,
                    &t3,
                    &n3);
                model->triangle_array[numfaces].p[0].v = v1;
                model->triangle_array[numfaces].p[0].t = t1;
                model->triangle_array[numfaces].p[0].n = n1;
                model->triangle_array[numfaces].p[1].v = v2;
                model->triangle_array[numfaces].p[1].t = t2;
                model->triangle_array[numfaces].p[1].n = n2;
                model->triangle_array[numfaces].p[2].v = v3;
                model->triangle_array[numfaces].p[2].t = t3;
                model->triangle_array[numfaces].p[2].n = n3;
                slog("triangle vertices: %i,%i,%i",model->triangle_array[numfaces].p[0].v,model->triangle_array[numfaces].p[1].v,model->triangle_array[numfaces].p[2].v);
                slog("scraped  vertices: %i,%i,%i",v1,v2,v3);
                numfaces++;
                break;
            default:
                /*chomp the line*/
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
}

void obj_model_sort(Model *model)
{
    float *vertex_array;
    float *texel_array;
    float *normal_array;
    int count;
    int i;
    if (!model)
    {
        return;
    }
    count = model->num_tris * 3;
    if (model->num_vertices)
    {
        vertex_array = malloc(sizeof(float)*3*count);
        if (vertex_array)
        {
            memset(vertex_array,0,sizeof(float)*3*count);
        }
    }
    if (model->num_normals)
    {
        normal_array = malloc(sizeof(float)*3*count);
        if (normal_array)
        {
            memset(normal_array,0,sizeof(float)*3*count);
        }
    }
    if (model->num_texels)
    {
        texel_array = malloc(sizeof(float)*2*count);
        if (texel_array)
        {
            memset(texel_array,0,sizeof(float)*2*count);
        }
    }
    /*sort everything into triangle order*/
    for (i = 0; i < model->num_tris; i++)
    {
        if (model->vertex_array)
        {
            vertex_array[(i*9)]     = model->vertex_array[(model->triangle_array[i].p[0].v * 3)];
            vertex_array[(i*9) + 1] = model->vertex_array[(model->triangle_array[i].p[0].v * 3) + 1];
            vertex_array[(i*9) + 2] = model->vertex_array[(model->triangle_array[i].p[0].v * 3) + 2];
            
            vertex_array[(i*9) + 3] = model->vertex_array[(model->triangle_array[i].p[1].v * 3)];
            vertex_array[(i*9) + 4] = model->vertex_array[(model->triangle_array[i].p[1].v * 3) + 1];
            vertex_array[(i*9) + 5] = model->vertex_array[(model->triangle_array[i].p[1].v * 3) + 2];
            
            vertex_array[(i*9) + 6] = model->vertex_array[(model->triangle_array[i].p[2].v * 3)];
            vertex_array[(i*9) + 7] = model->vertex_array[(model->triangle_array[i].p[2].v * 3) + 1];
            vertex_array[(i*9) + 8] = model->vertex_array[(model->triangle_array[i].p[2].v * 3) + 2];
        }
        if (model->normal_array)
        {
            normal_array[(i*9)]     = model->normal_array[(model->triangle_array[i].p[0].n * 3)];
            normal_array[(i*9) + 1] = model->normal_array[(model->triangle_array[i].p[0].n * 3) + 1];
            normal_array[(i*9) + 2] = model->normal_array[(model->triangle_array[i].p[0].n * 3) + 2];

            normal_array[(i*9) + 3] = model->normal_array[(model->triangle_array[i].p[1].n * 3)];
            normal_array[(i*9) + 4] = model->normal_array[(model->triangle_array[i].p[1].n * 3) + 1];
            normal_array[(i*9) + 5] = model->normal_array[(model->triangle_array[i].p[1].n * 3) + 2];

            normal_array[(i*9) + 6] = model->normal_array[(model->triangle_array[i].p[2].n * 3)];
            normal_array[(i*9) + 7] = model->normal_array[(model->triangle_array[i].p[2].n * 3) + 1];
            normal_array[(i*9) + 8] = model->normal_array[(model->triangle_array[i].p[2].n * 3) + 2];
        }

        if (model->normal_array)
        {
            texel_array[(i*6)]     = model->texel_array[(model->triangle_array[i].p[0].t * 2)];
            texel_array[(i*6) + 1] = model->texel_array[(model->triangle_array[i].p[0].t * 2) + 1];

            texel_array[(i*6) + 2] = model->texel_array[(model->triangle_array[i].p[1].t * 2)];
            texel_array[(i*6) + 3] = model->texel_array[(model->triangle_array[i].p[1].t * 2) + 1];

            texel_array[(i*6) + 4] = model->texel_array[(model->triangle_array[i].p[2].t * 2)];
            texel_array[(i*6) + 5] = model->texel_array[(model->triangle_array[i].p[2].t * 2) + 1];
        }
    }
    if (vertex_array)
    {
        if (model->vertex_array)free(model->vertex_array);
        model->vertex_array = vertex_array;
    }
    if (texel_array)
    {
        if (model->texel_array)free(model->texel_array);
        model->texel_array = texel_array;
    }
    if (normal_array)
    {
        if (model->normal_array)free(model->normal_array);
        model->normal_array = normal_array;
    }
}

void obj_model_setup(Model *model)
{
    if (!model)return;

    glGenBuffers(1, &model->vertex_bo); //create the buffer
    
    glGenVertexArrays(1, &model->vertex_ao);
    glBindVertexArray(model->vertex_ao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, model->vertex_bo); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
    
    
    glBufferData(GL_ARRAY_BUFFER, model_get_vertex_buffer_size(model), &model->vertex_array, GL_STATIC_DRAW);
    
    
    
/*    glBindBuffer(GL_ARRAY_BUFFER, model->triangle_bo); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, model_get_triangle_buffer_size(model), &model->triangle_array, GL_STATIC_DRAW); //formatting the data for the buffer
*/
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Model *obj_load_model(char *filename)
{
    FILE *file;
    Model *model;
    model = model_get_by_filename(filename);
    if (model)
    {
        return model;
    }
    model = model_new();
    if (!model)
    {
        return NULL;
    }
    file = fopen(filename,"r");
    if (file == NULL)
    {
        slog("failed to open file %s",filename);
        return NULL;
    }
        
    obj_file_get_counts(model,file);
    
    slog("vertices: %i",model->num_vertices);
    slog("normals: %i",model->num_normals);
    slog("texels: %i",model->num_texels);
    slog("faces: %i",model->num_tris);
    
    obj_model_allocate(model);
    obj_file_parse(model, file);
    
    fclose(file);
    
    obj_model_sort(model);
    obj_model_setup(model);
    
    return model;
}


/*eol@eof*/