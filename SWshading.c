/*
 * A framework for GLSL programming in TNM084 for MT1 2013.
 *
 * This is based on a framweork designed to be easy to understand
 * for students in a computer graphics course in the first year
 * of a M Sc curriculum. It uses custom code for some things that
 * are better solved by external libraries like GLEW and GLM, but
 * the emphasis is on simplicity and readability, not generality.
 * For the window management, GLFW 3.x is used for convenience.
 * The framework should work in Windows, MacOS X and Linux.
 * Some Windows-specific stuff for extension loading is still
 * here. GLEW could have been used instead, but for clarity
 * and minimal dependence on other code, we rolled our own extension
 * loading for the things we need. That code is short-circuited on
 * platforms other than Windows. This code is dependent only on
 * the GLFW and OpenGL libraries. OpenGL 3.x or higher is assumed.
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2013
 * This code is in the public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <GL/glext.h>

#include "tnm084_lab1.h"
#include "triangleSoup.h"

#include "noise1234.h"
#include "simplexnoise1234.h"
#include "cellular.h"

#define IMAGE_SIZE 400

/*
 * setupViewport() - set up the OpenGL viewport to handle window resizing
 */
void setupViewport(GLFWwindow* window) {

    int width, height;

    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( window, &width, &height );

    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, width, height ); // The entire window
}


/*
 * main(argc, argv) - the standard C entry point for the program
 */
int main(int argc, char *argv[]) {

	triangleSoup myShape;
    GLuint programObject; // Our single shader program
    GLuint location_tex;
    GLuint textureID;     // Our single texture

	double fps = 0.0;
    double time;
    int i, j, k;
    double x, y, z;
	int red, grn, blu;
	double point[3];
	double F[2];
	double delta[2][3];
	long ID[2];

	GLFWmonitor* monitor;
    const GLFWvidmode* vidmode;  // GLFW struct to hold information on the display
	GLFWwindow* window;

 	// The software-generated texture
 	unsigned char *pixels;
	
    // Initialise GLFW, bail out of unsuccesful
    if (!glfwInit()) return -1;

	monitor = glfwGetPrimaryMonitor();
	vidmode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(IMAGE_SIZE, IMAGE_SIZE, "Software shading demo", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // glfwSwapInterval() appears to be broken in Windows 7 with NVidia cards.
	// Change the system-wide settings instead to turn vertical sync on or off.
    glfwSwapInterval(0); // Do not wait for screen refresh between frames

    // Load the extensions for GLSL - note that this has to be done
    // *after* the window has been opened, or we won't have a GL context
    // to query for those extensions and connect to instances of them.
    loadExtensions();

    printf("GL vendor:       %s\n", glGetString(GL_VENDOR));
    printf("GL renderer:     %s\n", glGetString(GL_RENDERER));
    printf("GL version:      %s\n", glGetString(GL_VERSION));
    printf("Desktop size:    %d x %d pixels\n", vidmode->width, vidmode->height);


	// Create geometry for rendering
	soupInit(&myShape); // Initialize all fields to zero
	//soupCreateSphere(&myShape, 1.0, 10);
	soupCreateSquare(&myShape); // Unit square built from two triangles

	// Create a shader program object from GLSL code in two files
	programObject = createShader("vertexshader.glsl", "fragmentshader.glsl");
	
	location_tex = glGetUniformLocation( programObject, "tex" );

	pixels = (unsigned char*) calloc(IMAGE_SIZE*IMAGE_SIZE*4, sizeof(char));

    glGenTextures (1, &textureID );
    glBindTexture ( GL_TEXTURE_2D , textureID );
    // Set parameters to determine how the texture is resized
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR );
    // Set parameters to determine how the texture wraps at edges
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT );

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate and update the frames per second (FPS) display
        fps = computeFPS(window);
        time = glfwGetTime();

		// Set the clear color and depth, and clear the buffers for drawing
        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the viewport
        setupViewport(window);

		// Activate our shader program.
		glUseProgram( programObject );

		if ( location_tex != -1 ) {
             glUniform1i ( location_tex , 0);
		}

	    // Regenerate all the texture data on the CPU for every frame
	    for(i=0; i<IMAGE_SIZE; i++)
		{
			x = (double)i / IMAGE_SIZE;
		    for(j=0; j<IMAGE_SIZE; j++)
			{
				y = (double)j / IMAGE_SIZE;

				// Perlin noise
				red = 128 + 127*noise3(8.0*x, 8.0*y, 0.5*time);

				// Cellular (Worley) noise
//				point[0] = 12.0*x;
//				point[1] = 12.0*y;
//				point[2] = 0.2*time;
//				Worley(point, 2, F, delta, ID);
//				red = 120*(F[1]-F[0]);

				// Set red=grn=blu for grayscale image
				grn = red;
				blu = red;

				k = (i + j*IMAGE_SIZE)*4;
				pixels[k] = red;
				pixels[k + 1] = grn;
				pixels[k + 2] = blu;
				pixels[k + 3] = 255;
		    }
	    }
		
		// Upload the texture data to the GPU
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_SIZE, IMAGE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Generate mipmaps to get nice minification
		//glGenerateMipmap(GL_TEXTURE_2D);

		// Render the geometry
		soupRender(myShape);

		// Play nice and deactivate the shader program
		glUseProgram(0);

		// Swap buffers, i.e. display the image and prepare for next frame.
        glfwSwapBuffers(window);

		glfwPollEvents();

        // Exit if the ESC key is pressed.
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
          glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }

    // Close the OpenGL window and terminate GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

