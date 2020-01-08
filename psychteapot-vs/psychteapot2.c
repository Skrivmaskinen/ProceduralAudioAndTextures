// Ingemar's psychedelic teapot 2+
// Rewritten for modern OpenGL
// Teapot model generated with the OpenGL Utilities for Geometry Generation
// (also written by Ingemar) based on the original teapot data.

// Here with comfortable mouse dragging interface!

// gcc psychteapot2.c ../common/*.c ../common/Linux/*.c -lGL -o psychteapot2 -I../common -I../common/Linux -DGL_GLEXT_PROTOTYPES  -lXt -lX11 -lm

#include "MicroGlut.h"
// uses framework Cocoa
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

#define W 1980
#define H 1050

mat4 projectionMatrix;
mat4 viewMatrix, modelToWorldMatrix;

Model *m, *model1, *cube, *teapot, *squareModel;

// Reference to shader program
GLuint program;


GLfloat time;
GLfloat dTime;
// Vector constants
vec3 ZERO_VECTOR;
vec3 UP_VECTOR;
vec3 RIGHT_VECTOR;
vec3 FRONT_VECTOR;

GLfloat square[] = {
		-1,-1,0,
		-1,1, 0,
		1,1, 0,
		1,-1, 0 };

GLfloat squareTexCoord[] = {
		0, 0,
		0, 1,
		1, 1,
		1, 0 };
GLuint squareIndices[] = { 0, 1, 2, 0, 2, 3 };

typedef struct Camera
{
	/* Camera Properties */
	vec3 position;
	vec3 positionOffset;
	vec3 target;
	vec3 targetOffset;
	mat4 world2view;

	/* Frustum */
	float fovX; // Degrees
	float fovY; // Degrees
	float zNear;
	float zFar;
	mat4 projectionMatrix;


}Camera, *CameraPtr;

// A visible object in the scene
typedef struct SceneObject
{
	Model* model;
	GLuint shader;
	vec3 position;
	vec3 rotation;
	vec3 scaleV;
	struct SceneObject *next;

}SceneObject, *SceneObjectPtr;

Camera* player;
SceneObject* mainFire;

struct Camera*      newCamera(vec3 inPosition, vec3 inTarget);
struct SceneObject* newSceneObject(Model* inModel, GLuint inShader, vec3 inPosition, float inScale, SceneObject *root);
struct CoordTriple* newCoordTriple(vec3 position, vec3 target);
struct Portal*      newPortal(vec3 inRecorderPosition, vec3 inRecorderNormal, vec3 inSurfacePosition, vec3 inSurfaceNormal);
void enlistSceneObject(SceneObject *root, SceneObject *newSceneObject);

mat4 Rxyz(vec3 rotationVector);
vec3 getRotationOfNormal(vec3 theVector, vec3 x_axis, vec3 y_axis);
void getScreenAxis(SceneObject *this, vec3 *out_X, vec3 *out_Y);

////////////////////////////////////////////////////////////////////////////////////
/*                         SceneObject                                            */
////////////////////////////////////////////////////////////////////////////////////
struct SceneObject* newSceneObject(Model* inModel, GLuint inShader, vec3 inPosition, float inScale, SceneObject *root)
{
	// Allocate memory
	SceneObject* this;
	this = (SceneObject *)malloc(sizeof(SceneObject));

	// Set input
	this->model = inModel;
	this->shader = inShader;
	this->position = inPosition;
	this->scaleV = SetVector(inScale, inScale, inScale);
	this->rotation = SetVector(0, 0, 0);
	this->next = NULL;


	if (root != NULL)
	{
		enlistSceneObject(root, this);
	}

	// Return the created sceneObject
	return this;
}

void enlistSceneObject(SceneObject *root, SceneObject *newSceneObject)
{
	newSceneObject->next = root->next;
	root->next = newSceneObject;
}

////////////////////////////////////////////////////////////////////////////////////
/*                                Cameras                                         */
////////////////////////////////////////////////////////////////////////////////////

// Recalculate the projection matrix of a given camera. Used upon initialization and should be used after changing
// frustum attributes.
void calcProjection(Camera* this)
{
	this->projectionMatrix = perspective(this->fovY, this->fovX / this->fovY, this->zNear, this->zFar);
}

// Recalculate the cameras position and view-direction. Used upon initialization and should be used after changing
// position or target of camera.
void calcWorld2View(Camera* this)
{
	// Create the world2view
	this->world2view = lookAtv(VectorAdd(this->positionOffset, this->position), VectorAdd(this->position, this->targetOffset), UP_VECTOR);

}

// Create a new camera struct and return a pointer to it.
struct Camera* newCamera(vec3 inPosition, vec3 inTarget)
{
	// inPosition:  the position from which the camera looks from.
	// inTarget:    the point that the camera looks at.
	//////////////////////////////////////////////////////
	// Allocate memory for a new camera.
	Camera* this;
	this = (Camera *)malloc(sizeof(Camera));

	// Camera attributes
	this->position = inPosition;
	this->positionOffset = ZERO_VECTOR;

	this->target = inTarget;
	this->targetOffset = ZERO_VECTOR;

	// Matrixify
	calcWorld2View(this);

	// Frustum attributes
	this->fovX = 90 * W / H;
	this->fovY = 90;
	this->zNear = 0.001;
	this->zFar = 1000;
	// Matrixify
	calcProjection(this);

	return this;

}

// Add a vector 'addedPosition' to camera 'this' position.
void changeCameraPosition(Camera *this, vec3 *addedPosition)
{
	this->position = VectorAdd(this->position, *addedPosition);
	calcWorld2View(this);
}

// Set the Cameras position to 'newPosition'.
void setCameraPosition(Camera *this, vec3 *newPosition)
{
	this->position = *newPosition;
	calcWorld2View(this);
}

// Move the camera forward 'amount' meters.
// The movement vector is the xz-direction from the cameras position to its target.
void moveCamForward(Camera *this, float amount)
{
	vec3 moveVector = VectorSub(this->target, this->position);
	moveVector.y = 0;
	moveVector = Normalize(moveVector);
	moveVector = ScalarMult(moveVector, amount);
	this->position = VectorAdd(this->position, moveVector);
	this->target = VectorAdd(this->target, moveVector);
	calcWorld2View(this);
}

// Set camera 'this' target to 'newTarget'.
void setCameraTarget(Camera *this, vec3 *newTarget)
{
	this->target = *newTarget;
	calcWorld2View(this);
}

// Rotate the camera along the y-axis 'rotationRadian' radians.
void rotateCamera(Camera *this, float rotationRadian)
{
	vec3 newTarget = VectorSub(this->target, this->position);
	newTarget = MultVec3(ArbRotate(UP_VECTOR, rotationRadian), newTarget);
	newTarget = VectorAdd(newTarget, this->position);
	setCameraTarget(this, &newTarget);
}

void strafeCamera(Camera *this, float amount)
{
	vec3 lookingDirection = Normalize(VectorSub(this->target, this->position));
	vec3 strafeDirection = CrossProduct(lookingDirection, UP_VECTOR);
	strafeDirection = ScalarMult(strafeDirection, amount);
	this->position = VectorAdd(this->position, strafeDirection);
	this->target = VectorAdd(this->target, strafeDirection);
	calcWorld2View(this);
}

void initConstants()
{
	// Complex constants
	UP_VECTOR = SetVector(0, 1, 0);
	RIGHT_VECTOR = SetVector(1, 0, 0);
	FRONT_VECTOR = SetVector(0, 0, 1);
	ZERO_VECTOR = SetVector(0, 0, 0);
}
void initTime()
{
	time = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	dTime = 0.0;
}
void initShaders()
{

	program = loadShaders("psych.vert", "psych.frag");
}
void initModels()
{

	// Load models
	//model1 = LoadModelPlus("teapot.obj");
	//model1 = LoadModelPlus("stanford-bunny.obj");
	//cube = LoadModelPlus("objects/cubeplus.obj");
	//teapot = LoadModelPlus("teapot.obj");

	squareModel = LoadDataToModel(
		square, NULL, squareTexCoord, NULL,
		squareIndices, 4, 6);
}
void initSceneObjects()
{
	mainFire = newSceneObject(squareModel, program, SetVector(0, 0, 0), 2, NULL);
}
void init(void)
{
	initConstants();
	initTime();
	initShaders();
	initModels();
	initSceneObjects();

	// GL inits
	glClearColor(1.0,1.0,1.0,0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_TRUE);

	// Load and compile shader
	glUseProgram(program);
	
	// Upload geometry to the GPU:
	m = LoadModelPlus("teapot.obj");
	// End of upload of geometry		
	
	projectionMatrix = frustum(-0.5, 0.5, -0.5, 0.5, 1.0, 30.0);
	viewMatrix = lookAt(0, 1, 8, 0,0,0, 0,1,0);
	modelToWorldMatrix = IdentityMatrix();
//	mx = Mult(worldToViewMatrix, modelToWorldMatrix);


	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, viewMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
}

GLfloat a, b = 0.0;

void display(void)
{
	mat4 rot, trans, total;
	GLfloat t;

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	t = glutGet(GLUT_ELAPSED_TIME) / 100.0;

	trans = T(0, -1, 0); // Move teapot to center it
	total = Mult(modelToWorldMatrix, Mult(trans, Rx(-M_PI/2))); // trans centers teapot, Rx rotates the teapot to a comfortable default
	
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniform1f(glGetUniformLocation(program, "t"), t);

	DrawModel(m, program, "inPosition", NULL, "inTexCoord");
	
	glutSwapBuffers();
}


int prevx = 0, prevy = 0;

void mouseUpDown(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		prevx = x;
		prevy = y;
	}
}

void mouseDragged(int x, int y)
{
	vec3 p;
	mat4 m;
	mat3 wv3;
	
	// This is a simple and IMHO really nice trackball system:
	// You must have two things, the worldToViewMatrix and the modelToWorldMatrix
	// (and the latter is modified).
	
	// Use the movement direction to create an orthogonal rotation axis
	p.y = x - prevx;
	p.x = -(prevy - y);
	p.z = 0;

	// Transform the view coordinate rotation axis to world coordinates!
	wv3 = mat4tomat3(viewMatrix);
	p = MultMat3Vec3(InvertMat3(wv3), p);

	// Create a rotation around this axis and premultiply it on the model-to-world matrix
	m = ArbRotate(p, sqrt(p.x*p.x + p.y*p.y) / 50.0);
	modelToWorldMatrix = Mult(m, modelToWorldMatrix);
	
	prevx = x;
	prevy = y;
	
	glutPostRedisplay();
}

void mouse(int x, int y)
{
	b = x * 1.0;
	a = y * 1.0;
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("Ingemar's psychedelic teapot 2");
#ifdef WIN32
	glewInit();
#endif
	glutDisplayFunc(display); 
		glutMouseFunc(mouseUpDown);
		glutMotionFunc(mouseDragged);
	//	glutPassiveMotionFunc(mouse);
	glutRepeatingTimer(20);
	init ();
	glutMainLoop();
	exit(0);
}
