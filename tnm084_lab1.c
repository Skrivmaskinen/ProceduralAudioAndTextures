/*
 * Framework library functions for the course TNM084
 * Author: Stefan Gustavson 2013 (stefan.gustavson@liu.se)
 * This code is in the public domain.
 *
 * The function pointer loading for all OpenGL extensions,
 * the code in loadExtensions(), is forced upon us by
 * Microsoft Windows.
 * A more general project would use GLEW, but we choose to
 * stay clear of that dependence. GLEW is a large library
 * that hides a lot of details from the programmer, and it
 * is instructive to see what it hides - if nothing else to
 * better appreciate GLEW as a tool for OpenGL under Windows.
 *
 */

#include <stdio.h>  // For shader files and console messages
#include <stdlib.h> // For malloc() and free() in shader creation
#include <math.h>   // For fmod() in computeFPS()
#include <GLFW/glfw3.h>
#include "GL/glext.h"

#include "tnm084_lab1.h"

#ifdef __WIN32__
/* Global function pointers for everything we need beyond OpenGL 1.1 */
PFNGLCREATEPROGRAMPROC           glCreateProgram      = NULL;
PFNGLDELETEPROGRAMPROC           glDeleteProgram      = NULL;
PFNGLUSEPROGRAMPROC              glUseProgram         = NULL;
PFNGLCREATESHADERPROC            glCreateShader       = NULL;
PFNGLDELETESHADERPROC            glDeleteShader       = NULL;
PFNGLSHADERSOURCEPROC            glShaderSource       = NULL;
PFNGLCOMPILESHADERPROC           glCompileShader      = NULL;
PFNGLGETSHADERIVPROC             glGetShaderiv        = NULL;
PFNGLGETPROGRAMIVPROC            glGetProgramiv       = NULL;
PFNGLATTACHSHADERPROC            glAttachShader       = NULL;
PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog   = NULL;
PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog  = NULL;
PFNGLLINKPROGRAMPROC             glLinkProgram        = NULL;
PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation = NULL;
PFNGLUNIFORM1FPROC               glUniform1f          = NULL;
PFNGLUNIFORM1FVPROC              glUniform1fv         = NULL;
PFNGLUNIFORM1IPROC               glUniform1i          = NULL;
PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv   = NULL;
PFNGLGENBUFFERSPROC              glGenBuffers         = NULL;
PFNGLISBUFFERPROC                glIsBuffer           = NULL;
PFNGLBINDBUFFERPROC              glBindBuffer         = NULL;
PFNGLBUFFERDATAPROC              glBufferData         = NULL;
PFNGLDELETEBUFFERSPROC           glDeleteBuffers      = NULL;
PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays    = NULL;
PFNGLISVERTEXARRAYPROC           glIsVertexArray      = NULL;
PFNGLBINDVERTEXARRAYPROC         glBindVertexArray    = NULL;
PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
PFNGLGENERATEMIPMAPPROC          glGenerateMipmap     = NULL;
#endif


/*
 * printError() - Signal an error.
 * Simple printf() to console for portability.
 */
void printError(const char *errtype, const char *errmsg) {
  fprintf(stderr, "%s: %s\n", errtype, errmsg);
}


/*
 * Override the Win32 filelength() function with
 * a version that takes a Unix-style file handle as
 * input instead of a file ID number, and which works
 * on platforms other than Windows.
 */
long filelength(FILE *file) {
    long numbytes;
    long savedpos = ftell(file); // Remember where we are
    fseek(file, 0, SEEK_END);    // Fast forward to the end
    numbytes = ftell(file);      // Index of last byte in file
    fseek(file, savedpos, SEEK_SET); // Get back to where we were
    return numbytes;             // This is the file length
}


/*
 * loadExtensions() - Load OpenGL extensions for anything above OpenGL
 * version 1.1. (This is a requirement only on Windows, so on other
 * platforms, this function just checks for the required extensions.)
 */
void loadExtensions() {
    //These extension strings indicate that the OpenGL Shading Language
    // and GLSL shader objects are supported.
    if(!glfwExtensionSupported("GL_ARB_shading_language_100"))
    {
        printError("GL init error", "GL_ARB_shading_language_100 extension was not found");
        return;
    }
    if(!glfwExtensionSupported("GL_ARB_shader_objects"))
    {
        printError("GL init error", "GL_ARB_shader_objects extension was not found");
        return;
    }
    else
    {
#ifdef __WIN32__
        glCreateProgram      = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
        glDeleteProgram      = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram");
        glUseProgram         = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
        glCreateShader       = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
        glDeleteShader       = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
        glShaderSource       = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
        glCompileShader      = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
        glGetShaderiv        = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
        glGetShaderInfoLog   = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
        glAttachShader       = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
        glLinkProgram        = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
        glGetProgramiv       = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
        glGetProgramInfoLog  = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
        glUniform1f          = (PFNGLUNIFORM1FPROC)glfwGetProcAddress("glUniform1f");
        glUniform1fv         = (PFNGLUNIFORM1FVPROC)glfwGetProcAddress("glUniform1fv");
        glUniform1i          = (PFNGLUNIFORM1IPROC)glfwGetProcAddress("glUniform1i");
  		glUniformMatrix4fv   = (PFNGLUNIFORMMATRIX4FVPROC)glfwGetProcAddress("glUniformMatrix4fv");

        if( !glCreateProgram || !glDeleteProgram || !glUseProgram ||
            !glCreateShader || !glDeleteShader || !glShaderSource || !glCompileShader || 
            !glGetShaderiv || !glGetShaderInfoLog || !glAttachShader || !glLinkProgram ||
            !glGetProgramiv || !glGetProgramInfoLog || !glGetUniformLocation ||
            !glUniform1fv || !glUniform1f || !glUniform1i || !glUniformMatrix4fv )
        {
            printError("GL init error", "One or more required OpenGL shader extensions were not found");
            return;
        }
		
		glGenBuffers               = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");
		glIsBuffer                 = (PFNGLISBUFFERPROC)glfwGetProcAddress("glIsBuffer");
		glBindBuffer               = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");
		glBufferData               = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData");
		glDeleteBuffers            = (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers");
		glGenVertexArrays          = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
		glIsVertexArray            = (PFNGLISVERTEXARRAYPROC)glfwGetProcAddress("glIsVertexArray");
		glBindVertexArray          = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
		glDeleteVertexArrays       = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
		glEnableVertexAttribArray  = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray");
		glVertexAttribPointer      = (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer");
		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glDisableVertexAttribArray");
        glGenerateMipmap           = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmap");
		
		if( !glGenBuffers || !glIsBuffer || !glBindBuffer || !glBufferData || !glDeleteBuffers ||
		    !glGenVertexArrays || !glIsVertexArray || !glBindVertexArray || !glDeleteVertexArrays ||
			!glEnableVertexAttribArray || !glVertexAttribPointer ||
			!glDisableVertexAttribArray | !glGenerateMipmap )
        {
            printError("GL init error", "One or more required OpenGL vertex array functions were not found");
            return;
        }
		
#endif
    }
}


/*
 * readShaderFile(filename) - read a shader source string from a file
 */
unsigned char* readShaderFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        printError("ERROR", "Cannot open shader file!");
  		  return 0;
    }
    int bytesinfile = filelength(file);
    unsigned char *buffer = (unsigned char*)malloc(bytesinfile+1);
    int bytesread = fread( buffer, 1, bytesinfile, file);
    buffer[bytesread] = 0; // Terminate the string with 0
    fclose(file);
    
    return buffer;
}


/*
 * createShader() - create, load, compile and link the GLSL shader objects.
 */
GLuint createShader(char *vertexshaderfile, char *fragmentshaderfile) {
     GLuint programObject;
     GLuint vertexShader;
     GLuint fragmentShader;

     const char *vertexShaderStrings[1];
     const char *fragmentShaderStrings[1];
	 unsigned char *vertexShaderAssembly;
	 unsigned char *fragmentShaderAssembly;

     GLint vertexCompiled;
     GLint fragmentCompiled;
     GLint shadersLinked;
     char str[4096]; // For error messages from the GLSL compiler and linker

    // Create the vertex shader.
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    vertexShaderAssembly = readShaderFile(vertexshaderfile);
    if(vertexShaderAssembly) { // Don't try to use a NULL pointer
        vertexShaderStrings[0] = (char*)vertexShaderAssembly;
        glShaderSource(vertexShader, 1, vertexShaderStrings, NULL);
        glCompileShader(vertexShader);
        free((void *)vertexShaderAssembly);
    }

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS,
                               &vertexCompiled);
    if(vertexCompiled  == GL_FALSE)
  	{
        glGetShaderInfoLog(vertexShader, sizeof(str), NULL, str);
        printError("Vertex shader compile error", str);
  	}

  	// Create the fragment shader.
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    fragmentShaderAssembly = readShaderFile(fragmentshaderfile);
    if(fragmentShaderAssembly) { // Don't try to use a NULL pointer
    	fragmentShaderStrings[0] = (char*)fragmentShaderAssembly;
        glShaderSource(fragmentShader, 1, fragmentShaderStrings, NULL);
        glCompileShader(fragmentShader);
        free((void *)fragmentShaderAssembly);
    }

    glGetProgramiv(fragmentShader, GL_COMPILE_STATUS, &fragmentCompiled);
    if(fragmentCompiled == GL_FALSE)
   	{
        glGetShaderInfoLog(fragmentShader, sizeof(str), NULL, str);
        printError("Fragment shader compile error", str);
    }

    // Create a program object and attach the two compiled shaders.
    programObject = glCreateProgram();
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program object and print out the info log.
    glLinkProgram(programObject);
    glGetProgramiv(programObject, GL_LINK_STATUS, &shadersLinked);

    if(shadersLinked == GL_FALSE)
	{
		glGetProgramInfoLog( programObject, sizeof(str), NULL, str );
		printError("Program object linking error", str);
	}
	glDeleteShader(vertexShader);   // These are no longer needed
	glDeleteShader(fragmentShader); // after successful linking
	return programObject;
}


/*
 * computeFPS() - Calculate, display and return frame rate statistics.
 * Called every frame, but statistics are updated only once per second.
 * The time per frame is a better measure of performance than the
 * number of frames per second, so both are displayed.
 */
double computeFPS(GLFWwindow *window) {

    static double t0 = 0.0;
    static int frames = 0;
    static double fps = 0.0;
    static double frametime = 0.0;
    static char titlestring[200];

    double t;
    
    // Get current time
    t = glfwGetTime();  // Gets number of seconds since glfwInit()
    // If one second has passed, or if this is the very first frame
    if( (t-t0) > 1.0 || frames == 0 )
    {
        fps = (double)frames / (t-t0);
        if(frames > 0) frametime = 1000.0 * (t-t0) / frames;
        sprintf(titlestring, "TNM046, %.2f ms/frame (%.1f FPS)", frametime, fps);
        glfwSetWindowTitle(window, titlestring);
        // printf("Speed: %.1f FPS\n", fps);
        t0 = t;
        frames = 0;
    }
    frames ++;
    return fps;
}

