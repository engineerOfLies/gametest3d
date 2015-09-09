#include "obj.h"
#include "simple_logger.h"

typedef struct TriPoint_S
{
    GLuint v;   /**<vertex index*/
    GLuint n;   /**<normal index*/
    GLuint t;   /**<texel index*/
}TriPoint;

typedef struct
{
    TriPoint p[3];
}ObjTriangle;

typedef struct
{
    Uint32 num_tris;
    Uint32 num_vertices;
    Uint32 num_texels;
    Uint32 num_normals;
    
    float *vertex_array;
    float *texel_array;
    float *normal_array;
    ObjTriangle *triangle_array;
    
}ObjFile;

void obj_file_get_counts(ObjFile* model, FILE* file)
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

void obj_model_allocate(ObjFile *model)
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
        model->triangle_array = malloc(sizeof(ObjTriangle)*model->num_tris);
        if (model->triangle_array)
        {
            memset(model->triangle_array,0,sizeof(ObjTriangle)*model->num_tris);
        }
    }
}

void obj_file_parse(ObjFile * model, FILE* file)
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
                    "%u/%u/%u %u/%u/%u %u/%u/%u",
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
                numfaces++;
                break;
            default:
                /*chomp the line*/
                fgets(buf, sizeof(buf), file);
                break;
        }
    }
}

int obj_va_search(float *v,float *a,int count,float vtarget[3],float atarget[6])
{
    int i;
    if (!v || !a)
    {
        slog("error, missing a buffer to search");
        return -1;/*not found*/
    }
    for (i = 0; i < count;i++)
    {
        if (memcmp(&v[i * 3],vtarget,sizeof(float)*3) != 0)
        {
            /*not equal, keep searching*/
            continue;
        }
        if (memcmp(&a[i * 3],atarget,sizeof(float)*6) != 0)
        {
            /*not equal, keep searching*/
            continue;
        }
        return i;
    }
    return -1;/*not found*/
}

int obj_file_convert_to_model(ObjFile *objFile,Model *model)
{
    int i,j,count = 0;
    float *vertices; /**<working vertex buffer*/
    float *attributes; /**<working attribute buffer*/
    float *vtarget,atarget[6];
    if ((!model) || (!objFile))
    {
        slog("passed null data!");
        return -1;
    }
    /*go through each triangle, building one synchronous vertex buffer and one attribute buffer*/
    vertices = (float *)malloc(sizeof(float) * objFile->num_tris * 3 * 3);
    attributes = (float *)malloc(sizeof(float) * objFile->num_tris * 3 * 3 * 2);/*packing in normals and uvs*/
    if ((!vertices) || (!attributes))
    {
        slog("failed to alloacte vertex / attribute buffers!");
        if (vertices)free(vertices);
        if (attributes)free(attributes);
        return -1;
    }
    
    memset(atarget,0,sizeof(float)*6);
    /*for each triangle...*/
    for (i = 0; i < objFile->num_tris;i++)
    {
        /*for each triangle vertex...*/
        for (j = 0;j < 3;j++)
        {
            /*get a pointer to the vertex data*/
            vtarget = &objFile->vertex_array[objFile->triangle_array[i].p[j].v];

            /*copy the normal data*/
            memcpy(atarget,&objFile->normal_array[objFile->triangle_array[i].p[j].n],sizeof(float)*3);

            /*copy the texture data*/
            memcpy(&atarget[3],&objFile->texel_array[objFile->triangle_array[i].p[j].t],sizeof(float)*2);
            
            /*check if we already had this vertex/normal/uv set before*/
            if (obj_va_search(vertices,attributes,count,vtarget,atarget) == -1)
            {
                /*new index*/
            }
        }   
    }
    
    return 0;
}

Model *obj_load_model(char *filename)
{
    FILE *file;
    Model *model;
    ObjFile objFile;
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
        model_free(model);
        return NULL;
    }
        
    obj_file_get_counts(&objFile,file);
    
    slog("vertices: %i",objFile.num_vertices);
    slog("normals: %i",objFile.num_normals);
    slog("texels: %i",objFile.num_texels);
    slog("faces: %i",objFile.num_tris);
    
    obj_model_allocate(&objFile);
    obj_file_parse(&objFile, file);
    
    fclose(file);
    
    if (obj_file_convert_to_model(&objFile,model) != 0)
    {
        model_free(model);        
        return NULL;
    }
    
    return model;
}


/*eol@eof*/