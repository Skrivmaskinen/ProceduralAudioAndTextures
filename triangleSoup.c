#include <stdio.h>  // For file input in soupReadOBJ()
#include <stdlib.h> // For malloc() and free()
#include <math.h>   // For sin() and cos() in soupCreateSphere()
#include <GLFW/glfw3.h>
#include "GL/glext.h"

#include "tnm084_lab1.h"  // To be able to use OpenGL extensions below

#include "triangleSoup.h"

/* Initialize a triangleSoup object to all zeros */
void soupInit(triangleSoup *soup) {
	soup->vao = 0;
	soup->vertexbuffer = 0;
	soup->indexbuffer = 0;
	soup->vertexarray = NULL;
	soup->indexarray = NULL;
	soup->nverts = 0;
	soup->ntris = 0;
}


/* Clean up allocated data in a triangleSoup object */
void soupDelete(triangleSoup *soup) {

	if(glIsVertexArray(soup->vao)) {
		glDeleteVertexArrays(1, &(soup->vao));
	}
	soup->vao = 0;

	if(glIsBuffer(soup->vertexbuffer)) {
		glDeleteBuffers(1, &(soup->vertexbuffer));
	}
	soup->vertexbuffer = 0;

	if(glIsBuffer(soup->indexbuffer)) {
		glDeleteBuffers(1, &(soup->indexbuffer));
	}
	soup->indexbuffer = 0;

	if(soup->vertexarray) {
		free((void*)soup->vertexarray);
	}
	if(soup->indexarray) 	{
		free((void*)soup->indexarray);
	}
	soup->nverts = 0;
	soup->ntris = 0;

};


/* Create a simple square geometry */
void soupCreateSquare(triangleSoup *soup) {

	// Delete any previous content in the triangleSoup object
	soupDelete(soup);
  
	soup->nverts = 4;
	soup->ntris = 2;
	soup->vertexarray = (float*)malloc(soup->nverts * 8 * sizeof(float));
	soup->indexarray = (int*)malloc(soup->ntris * 3 * sizeof(int));

	// The vertex array: 3D xyz, 3D normal, 2D st (8 floats per vertex)
	// First vertex: bottom left
	soup->vertexarray[0] = -1.0f;
	soup->vertexarray[1] = -1.0f;
	soup->vertexarray[2] = 0.0f;
	soup->vertexarray[3] = 0.0f;
	soup->vertexarray[4] = 0.0f;
	soup->vertexarray[5] = 1.0f;
	soup->vertexarray[6] = 0.0f;
	soup->vertexarray[7] = 0.0f;
	// Second vertex: bottom right
	soup->vertexarray[8] = 1.0f;
	soup->vertexarray[9] = -1.0f;
	soup->vertexarray[10] = 0.0f;
	soup->vertexarray[11] = 0.0f;
	soup->vertexarray[12] = 0.0f;
	soup->vertexarray[13] = 1.0f;
	soup->vertexarray[14] = 1.0f;
	soup->vertexarray[15] = 0.0f;
	// Third vertex: top left
	soup->vertexarray[16] = -1.0f;
	soup->vertexarray[17] = 1.0f;
	soup->vertexarray[18] = 0.0f;
	soup->vertexarray[19] = 0.0f;
	soup->vertexarray[20] = 0.0f;
	soup->vertexarray[21] = 1.0f;
	soup->vertexarray[22] = 0.0f;
	soup->vertexarray[23] = 1.0f;
	// Fourth vertex: top right
	soup->vertexarray[24] = 1.0f;
	soup->vertexarray[25] = 1.0f;
	soup->vertexarray[26] = 0.0f;
	soup->vertexarray[27] = 0.0f;
	soup->vertexarray[28] = 0.0f;
	soup->vertexarray[29] = 1.0f;
	soup->vertexarray[30] = 1.0f;
	soup->vertexarray[31] = 1.0f;

	soup->indexarray[0] = 0; // First triangle: vertices 0-1-2
	soup->indexarray[1] = 1;
	soup->indexarray[2] = 2;
	soup->indexarray[3] = 1; // Second triangle: vertices 1-3-2
	soup->indexarray[4] = 3;
	soup->indexarray[5] = 2;
	
	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &(soup->vao));
	glBindVertexArray(soup->vao);

	// Generate two buffer IDs
	glGenBuffers(1, &(soup->vertexbuffer));
	glGenBuffers(1, &(soup->indexbuffer));

 	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, soup->vertexbuffer);
 	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER,
		8*soup->nverts * sizeof(GLfloat), soup->vertexarray, GL_STATIC_DRAW);
	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // Texture coordinates
	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		8*sizeof(GLfloat), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8*sizeof(GLfloat), (void*)(3*sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8*sizeof(GLfloat), (void*)(6*sizeof(GLfloat))); // texcoords

 	// Activate the index buffer
 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, soup->indexbuffer);
 	// Present our vertex indices to OpenGL
 	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	 	3*soup->ntris*sizeof(GLuint), soup->indexarray, GL_STATIC_DRAW);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};


/* Render the geometry in a triangleSoup object */
void soupRender(triangleSoup soup) {
	
	glBindVertexArray(soup.vao);	
	glDrawElements(GL_TRIANGLES, 3 * soup.ntris, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);	

};

