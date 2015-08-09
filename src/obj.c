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
        model->triangle_array = malloc(sizeof(GLuint)*9*model->num_tris);
        if (model->triangle_array)
        {
            memset(model->triangle_array,0,sizeof(GLuint)*9*model->num_tris);
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
                    "%ud/%ud/%ud %ud/%ud/%ud %ud/%ud/%ud",
                    &v1,
                    &t1,
                    &n1,
                    &v2,
                    &t2,
                    &n2,
                    &v3,
                    &t3,
                    &n3);
                model->triangle_array[numfaces++] = v1;
                model->triangle_array[numfaces++] = t1;
                model->triangle_array[numfaces++] = n1;
                model->triangle_array[numfaces++] = v2;
                model->triangle_array[numfaces++] = t2;
                model->triangle_array[numfaces++] = n2;
                model->triangle_array[numfaces++] = v3;
                model->triangle_array[numfaces++] = t3;
                model->triangle_array[numfaces++] = n3;
                break;
            default:
                /*chomp the line*/
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
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
    
    fclose(file);
    return model;
}


/*eol@eof*/