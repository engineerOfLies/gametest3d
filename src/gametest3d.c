#include "simple_logger.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

GLuint BuildShaderProgram(const char *vsPath, const char *fsPath);
GLuint CreateShader(GLenum eShaderType, const char *strShaderFile);


int main(int argc, char *argv[])
{
    SDL_GLContext glContext;
    const unsigned char *version;
    GLenum glew_status;
    GLuint theShaderProgram;
    GLuint vao;
    GLuint triangleBufferObject;
    char bGameLoopRunning = 1;
    SDL_Event e;
    SDL_Window *window;
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
    
    window = SDL_CreateWindow("Gametest 3D",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;
    
    glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
    {
        slog("There was an error creating the OpenGL context!\n");
        return 0;
    }
    
    version = glGetString(GL_VERSION);
    if (version == NULL) 
    {
        slog("There was an error creating the OpenGL context!\n");
        return 1;
    }
    
    SDL_GL_MakeCurrent(window, glContext);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    
    //MUST make a context AND make it current BEFORE glewInit()!
    glewExperimental = GL_TRUE;
    glew_status = glewInit();
    if (glew_status != 0) 
    {
        slog("Error: %s", glewGetErrorString(glew_status));
        return 1;
    }
        
    
    theShaderProgram = BuildShaderProgram("vs1.glsl", "fs1.glsl");
    if (theShaderProgram == -1)
    {
        SDL_Quit();
        return 0;
    }
    
    slog("Using program %d", theShaderProgram);
    
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); //make our vertex array object, we need it to restore state we set after binding it. Re-binding reloads the state associated with it.
    
    glGenBuffers(1, &triangleBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers
    
    slog("glError: %d", glGetError());
    
    while (bGameLoopRunning)
    {
        if ( SDL_PollEvent(&e) ) 
        {
            if (e.type == SDL_QUIT)
                bGameLoopRunning = 0;
            else if (e.type == SDL_KEYUP && e.key.keysym.sym == SDLK_ESCAPE)
                bGameLoopRunning = 0;
        }

        glClearColor(0.0,0.0,0.0,0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        /* drawing code in here! */
        glUseProgram(theShaderProgram);
        glBindBuffer(GL_ARRAY_BUFFER, triangleBufferObject); //bind the buffer we're applying attributes to
        glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
        glEnableVertexAttribArray(1); //attribute 1 is for vertex color data
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0); //tell gl (shader!) how to interpret our vertex data
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)48); //color data is 48 bytes in to the array
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glUseProgram(0);
        /* drawing code above here! */

        SDL_GL_SwapWindow(window);
        SDL_Delay(20);
    } 
    
    SDL_GL_DeleteContext(glContext);
    SDL_Quit();
    return 0;
}

GLuint BuildShaderProgram(const char *vsPath, const char *fsPath)
{
    GLint infoLogLength;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint tempProgram;
    GLint status;
    GLchar strInfoLog[4096];
    
    vertexShader = CreateShader(GL_VERTEX_SHADER, vsPath);
    fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fsPath);
    
    /* So we've compiled our shaders, now we need to link them in to the program
    that will be used for rendering. */
    
    /*This section could be broken out into a separate function, but we're doing it here 
    because I'm not using C++ STL features that would make this easier. Tutorial doing so is 
    here: http://www.arcsynthesis.org/gltut/Basics/Tut01%20Making%20Shaders.html */
    
    tempProgram = glCreateProgram();
    
    glAttachShader(tempProgram, vertexShader);
    glAttachShader(tempProgram, fragmentShader);
    
    glLinkProgram(tempProgram); //linking!
    
    //error checking
    glGetProgramiv(tempProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

        glGetProgramInfoLog(tempProgram, infoLogLength, NULL, strInfoLog);
        slog("Shader linker failure: %s", strInfoLog);
        return -1;
    }
    else
        puts("Shader linked sucessfully!");
    
    glDetachShader(tempProgram, vertexShader);
    glDetachShader(tempProgram, fragmentShader);
    
    return tempProgram;
}

GLuint CreateShader(GLenum eShaderType, const char *strShaderFile)
{
    char shaderSource[4096];
    char inChar;
    FILE *shaderFile;
    int i = 0;
    GLuint shader;
    GLint status;
    GLint infoLogLength;
    GLchar strInfoLog[4096];
    char strShaderType[16];
    const char *ss;
    
    shaderFile = fopen(strShaderFile, "r");
    while(fscanf(shaderFile,"%c",&inChar) > 0)
    {
        shaderSource[i++] = inChar; //loading the file's chars into array
    }
    shaderSource[i - 1] = '\0';
    fclose(shaderFile);
    puts(shaderSource); //print to make sure the string is loaded
    
    shader = glCreateShader(eShaderType);
    ss = shaderSource;
    glShaderSource(shader, 1, &ss, NULL);
    
    glCompileShader(shader);
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        
        switch(eShaderType)
        {
            case GL_VERTEX_SHADER: sprintf(strShaderType, "vertex"); break;
            case GL_GEOMETRY_SHADER: sprintf(strShaderType, "geometry"); break;
            case GL_FRAGMENT_SHADER: sprintf(strShaderType, "fragment"); break;
        }

        slog("Compile failure in %s shader:\n%s", strShaderType, strInfoLog);
        return -1;
    }
    else
        puts("Shader compiled sucessfully!");

    return shader;
}
/*eol@eof*/