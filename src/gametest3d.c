/**
 * gametest3d
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
#include "simple_logger.h"
#include "graphics3d.h"
#include "shader.h"
#include "obj.h"
#include "vector.h"
#include "sprite.h"
#include "entity.h"
#include "space.h"

void set_camera(Vec3D position, Vec3D rotation);

Entity *newCube(Vec3D position)
{
    Entity * ent;
    ent = entity_new();
    if (!ent)
    {
        return NULL;
    }
    ent->objModel = obj_load("models/cube.obj");
    ent->texture = LoadSprite("models/cube_text.png",1024,1024);
    vec3d_cpy(ent->body.position,position);
    sprintf(ent->name,"cube");
    return ent;
}

int main(int argc, char *argv[])
{
    int i;
    GLuint vao;
    float r = 0;
    Space *space;
    Entity *cube1,*cube2;
    GLuint triangleBufferObject;
    char bGameLoopRunning = 1;
    Vec3D cameraPosition = {0,-10,0.3};
    Vec3D cameraRotation = {90,0,0};
    SDL_Event e;
    Obj *bgobj;
    Sprite *bgtext;
    const float triangleVertices[] = {
        0.0f, 0.5f, 0.0f, 1.0f,
        0.5f, -0.366f, 0.0f, 1.0f,
        -0.5f, -0.366f, 0.0f, 1.0f,
        //next part contains vertex colors
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f  
    }; //we love you vertices!
    
    init_logger("gametest3d.log");
    if (graphics3d_init(1024,768,1,"gametest3d",33) != 0)
    {
        return -1;
    }
    model_init();
    obj_init();
    entity_init(255);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); //make our vertex array object, we need it to restore state we set after binding it. Re-binding reloads the state associated with it.
    
    glGenBuffers(1, &triangleBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers
    
    bgobj = obj_load("models/mountainvillage.obj");
    bgtext = LoadSprite("models/mountain_text.png",1024,1024);
    
    cube1 = newCube(vec3d(0,0,0));
    cube2 = newCube(vec3d(10,0,0));
    
    cube2->body.velocity.x = -0.1;
    
    space = space_new();
    space_set_steps(space,100);
    
    space_add_body(space,&cube1->body);
    space_add_body(space,&cube2->body);
    while (bGameLoopRunning)
    {
        for (i = 0; i < 100;i++)
        {
            space_do_step(space);
        }
        while ( SDL_PollEvent(&e) ) 
        {
            if (e.type == SDL_QUIT)
            {
                bGameLoopRunning = 0;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    bGameLoopRunning = 0;
                }
                else if (e.key.keysym.sym == SDLK_SPACE)
                {
                    cameraPosition.z++;
                }
                else if (e.key.keysym.sym == SDLK_z)
                {
                    cameraPosition.z--;
                }
                else if (e.key.keysym.sym == SDLK_w)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            -sin(cameraRotation.z * DEGTORAD),
                            cos(cameraRotation.z * DEGTORAD),
                            0
                        ));
                }
                else if (e.key.keysym.sym == SDLK_s)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            sin(cameraRotation.z * DEGTORAD),
                            -cos(cameraRotation.z * DEGTORAD),
                            0
                        ));
                }
                else if (e.key.keysym.sym == SDLK_d)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            cos(cameraRotation.z * DEGTORAD),
                            sin(cameraRotation.z * DEGTORAD),
                            0
                        ));
                }
                else if (e.key.keysym.sym == SDLK_a)
                {
                    vec3d_add(
                        cameraPosition,
                        cameraPosition,
                        vec3d(
                            -cos(cameraRotation.z * DEGTORAD),
                            -sin(cameraRotation.z * DEGTORAD),
                            0
                        ));
                }
                else if (e.key.keysym.sym == SDLK_LEFT)
                {
                    cameraRotation.z += 1;
                }
                else if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    cameraRotation.z -= 1;
                }
                else if (e.key.keysym.sym == SDLK_UP)
                {
                    cameraRotation.x += 1;
                }
                else if (e.key.keysym.sym == SDLK_DOWN)
                {
                    cameraRotation.x -= 1;
                }
            }
        }

        graphics3d_frame_begin();
        
        glPushMatrix();
        set_camera(
            cameraPosition,
            cameraRotation);
        
        entity_draw_all();  
        obj_draw(
            bgobj,
            vec3d(0,0,2),
            vec3d(90,90,0),
            vec3d(5,5,5),
            vec4d(1,1,1,1),
            bgtext
        );
        
        if (r > 360)r -= 360;
        glPopMatrix();
        /* drawing code above here! */
        graphics3d_next_frame();
    } 
    return 0;
}

void set_camera(Vec3D position, Vec3D rotation)
{
    glRotatef(-rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(-rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(-rotation.z, 0.0f, 0.0f, 1.0f);
    glTranslatef(-position.x,
                 -position.y,
                 -position.z);
}

/*eol@eof*/