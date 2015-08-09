#ifndef __MODEL_H__
#define __MODEL_H__
/**
 * model
 * @license The MIT License (MIT)
 *   @copyright Copyright (c) 2015 EngineerOfLies
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *    The above copyright notice and this permission notice shall be included in all
 *    copies or substantial portions of the Software.
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *    SOFTWARE.
 */
#include <SDL.h>
#include "sprite.h"

typedef struct TriPoint_S
{
    GLuint v;   /**<vertex index*/
    GLuint n;   /**<normal index*/
    GLuint t;   /**<texel index*/
}TriPoint;

typedef struct
{
    TriPoint p[3];
}Triangle;

typedef struct Model_S
{
    char   filename[512];
    
    Uint32 num_tris;
    Uint32 num_vertices;
    Uint32 num_texels;
    Uint32 num_normals;
    
    GLuint vertex_bo;
    GLuint vertex_ao;
    
    float *vertex_array;
    float *texel_array;
    float *normal_array;
    Triangle *triangle_array;
    
    Sprite *texture;        /**<pointer to texture data*/
    Uint8 used;             /**<refcount*/
}Model;


/**
 * @brief initialize the model system
 */
void model_init();

/**
 * @brief get a new model pointer
 * @return NULL on error or out of space, a zero initialized model pointer otherwise
 */
Model *model_new();

/**
 * @brief checks if the model file is already loaded and returns a pointer to it if it is
 * @param filename the file to check if its already loaded
 * @return NULL on error or not loaded, the model pointer otherwise
 */
Model *model_get_by_filename(char *filename);

/**
 * @brief assign a texture to the model
 * @param model the model to assign to
 * @param texture the name of the texture file to load
 */
void model_assign_texture(Model *model,char *texture);

/**
 * @brief get the memory size of the triangle buffer for the model
 * @param model the model to check
 * @return 0 on error, or the memory footprint otherwise
 */
size_t model_get_triangle_buffer_size(Model *model);

/**
 * @brief get the memory size of the vertex buffer for the model
 * @param model the model to check
 * @return 0 on error, or the memory footprint otherwise
 */
size_t model_get_vertex_buffer_size(Model *model);

#endif
