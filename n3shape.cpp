/* Copyright (c) 2015 Stephen Meier
g++ n3shape.cpp -o n3shape.exe -I./include -L./lib -lmingw32 -lglew32 -lopengl32 -lSDL2main -lSDL2
*/

//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GL/glew.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

//-----------------------------------------------------------------------------
struct _N3Material {
	unsigned char data[92];
};

struct _N3VertexT1: public glm::vec3 {
	glm::vec3 n;
	float tu, tv;
};

struct _N3TexHeader {
	char szID[4];
	int nWidth;
	int nHeight;
	int Format;
	bool bMipMap;
};

struct _N3EdgeCollapse {
	int NumIndicesToLose;
	int NumIndicesToChange;
	int NumVerticesToLose;
	int iIndexChanges;
	int CollapseTo;
	bool bShouldCollapse;
};

struct _N3LODCtrlValue {
	float fDist;
	int iNumVertices;
};

enum _D3DFORMAT {
	D3DFMT_UNKNOWN,
	D3DFMT_DXT1 = 827611204,
	D3DFMT_DXT3 = 861165636,
	D3DFMT_DXT5 = 894720068,
};

typedef struct {
	SDL_bool up;
	SDL_bool down;
	SDL_bool left;
	SDL_bool right;
} Input;

//-----------------------------------------------------------------------------
SDL_Event       event;
Input           pInput;
SDL_Window*     window;
SDL_GLContext   context;
SDL_bool        running;
SDL_bool        debugMode;

_N3TexHeader    HeaderOrg;
unsigned char*  compTexData;
int             compTexSize;
unsigned short* m_pIndices0;
_N3VertexT1*    m_pVertices0;
int             m_iMaxNumIndices0;
int             m_iMaxNumVertices0;

//-----------------------------------------------------------------------------
void N3Init(void);
void N3Quit(void);

void N3LoadMesh(const char* szFN);
void N3LoadTexture(const char* szFN);

//-----------------------------------------------------------------------------
int SDL_main(int argc, char* argv[]) {
	N3Init();

	/* === */

	// NOTE: check for the correct number of command line arguements
	if(argc != 4) {
		fprintf(stderr, "\nERROR: Incorrect number of arguements.\n");
		exit(-1);
	}

	// NOTE: debug mode will print out helpful information
	debugMode = (SDL_bool) atoi(argv[1]);

	// NOTE: get the filename for the shape
	char fnBuffer[0xFF];
	sprintf(fnBuffer, "%s.n3shape", argv[2]);

	FILE* fpShape = fopen(fnBuffer, "rb");
	/*
	*/

	// NOTE: length of the name of shape
	int nL0 = 0;
	fread(&nL0, sizeof(int), 1, fpShape);

	// NOTE: if the shape have a name read it in
	if(nL0 > 0) {
		char szFN0[(nL0+1)];
		memset(&szFN0, 0, (nL0+1));

		fread(&szFN0, sizeof(char), nL0, fpShape);
	}

	// NOTE: read in the position vector
	glm::vec3 m_vPos0;
	fread(&m_vPos0, sizeof(glm::vec3), 1, fpShape);

	// NOTE: read in the rotation coords
	glm::quat m_qRot0;
	fread(&m_qRot0, sizeof(glm::quat), 1, fpShape);

	// NOTE: read in the scale vector
	glm::vec3 m_vScale0;
	fread(&m_vScale0, sizeof(glm::vec3), 1, fpShape);

	/*
		m_KeyPos
	*/

	// NOTE: the count of the keypos AnimKey
	int m_nCount0 = 0;
	fread(&m_nCount0, sizeof(int), 1, fpShape);

	if(m_nCount0 > 0) {
		// NOTE: do some stuff
	}

	/*
		m_KeyRot
	*/

	// NOTE: the count of the keyrot AnimKey
	int m_nCount1 = 0;
	fread(&m_nCount1, sizeof(int), 1, fpShape);

	if(m_nCount1 > 0) {
		// NOTE: do some stuff
	}

	/*
		m_KeyScale
	*/

	// NOTE: the count of the keyscale AnimKey
	int m_nCount2 = 0;
	fread(&m_nCount2, sizeof(int), 1, fpShape);

	if(m_nCount2 > 0) {
		// NOTE: do some stuff
	}

	// NOTE: length of the filename of meshcollision
	int nL1 = 0;
	fread(&nL1, sizeof(int), 1, fpShape);

	// NOTE: if the shape has a collision mesh read it in
	if(nL1 > 0) {
		char szFN1[(nL1+1)];
		memset(&szFN1, 0, (nL1+1));

		fread(&szFN1, sizeof(char), nL1, fpShape);
		/*
		- TODO: load collision mesh
		*/
	}

	// NOTE: length of the filename of climbcollision
	int nL2 = 0;
	fread(&nL2, sizeof(int), 1, fpShape);

	// NOTE: if the shape has a climb mesh read it in
	if(nL2 > 0) {
		char szFN2[(nL2+1)];
		memset(&szFN2, 0, (nL2+1));

		fread(&szFN2, sizeof(char), nL2, fpShape);
		/*
		- TODO: load "climb" mesh
		*/
	}

	// NOTE: read in the number of parts for this shape
	int iPC0 = 0;
	fread(&iPC0, sizeof(int), 1, fpShape);

	// NOTE: print debug info
	if(debugMode) {
		printf("\n\nNOTE: This mesh has %d part(s)\n\n", iPC0);
	}

	// NOTE: if there are parts read them in
	if((iPC0 > 0) && (atoi(argv[3]) <= iPC0)) {
		for(int i=0; i<atoi(argv[3]); i++) {// iPC0; i++) {
			// NOTE: read in the part's pivot point
			glm::vec3 m_vPivot0;
			fread(&m_vPivot0, sizeof(glm::vec3), 1, fpShape);

			// NOTE: length of the filename for the part's mesh
			int nL3 = 0;
			fread(&nL3, sizeof(int), 1, fpShape);

			// NOTE: read in the filename for the mesh
			char szFN3[(nL3+1)];
			memset(&szFN3, 0, (nL3+1));

			fread(&szFN3, sizeof(char), nL3, fpShape);

			// NOTE: print debug info
			if(debugMode) {
				printf("\n\nNOTE: Loading %s mesh\n\n", szFN3);
			}

			// NOTE: !!** load the mesh information **!!
			N3LoadMesh(szFN3);

			// NOTE: read in the part's d3d material
			_N3Material m_Mtl0 = {};
			fread(&m_Mtl0, sizeof(_N3Material), 1, fpShape);

			// NOTE: read in the number of textures for this part
			int iTC0 = 0;
			fread(&iTC0, sizeof(int), 1, fpShape);

			// NOTE: read in the FPS for thetextures
			int m_fTexFPS0 = 0;
			fread(&m_fTexFPS0, sizeof(int), 1, fpShape);

			// NOTE: print debug info
			if(debugMode) {
				printf("\n\nNOTE: Loading %d textures\n\n", iTC0);
			}

			// NOTE: loop over the textures for the part and read them in
			for(int j=0; j<iTC0; j++) {
				// NOTE: length of the filename for the texture
				int nL4 = 0;
				fread(&nL4, sizeof(int), 1, fpShape);

				// NOTE: read in the filename for the texture
				char szFN4[(nL4+1)];
				memset(&szFN4, 0, (nL4+1));

				fread(&szFN4, sizeof(char), nL4, fpShape);

				// NOTE: !!** load the dxt texture **!!
				N3LoadTexture(szFN4);
			}
		}
	} else {
		fprintf(stderr, "\nERROR: Unknown piece request.\n");
		exit(-1);
	}

	// NOTE: after loading the parts set some of the final pieces of data
	int m_iBelong, m_iEventID, m_iEventType, m_iNPC_ID, m_iNPC_Status;

	fread(&m_iBelong, sizeof(int), 1, fpShape);
	fread(&m_iEventID, sizeof(int), 1, fpShape);
	fread(&m_iEventType, sizeof(int), 1, fpShape);
	fread(&m_iNPC_ID, sizeof(int), 1, fpShape);
	fread(&m_iNPC_Status, sizeof(int), 1, fpShape);

	/*
	*/
	fclose(fpShape);

	/* SET SHADER PROGRAM */
	// ========================================================================

	// NOTE: source code for the vertex shader
	const GLchar* vertSource = {
		"#version 110\n"\
		"\n"\
		"attribute vec3 pos;\n"\
		"attribute vec2 texcoord;\n"\
		"\n"\
		"varying vec2 fragTexcoord;\n"\
		"\n"\
		"uniform mat4 model;\n"\
		"uniform mat4 view;\n"\
		"uniform mat4 proj;\n"\
		"\n"\
		"void main() {\n"\
			"fragTexcoord = texcoord;\n"\
			"gl_Position = proj*view*model*vec4(pos, 1.0);\n"\
		"}\n"\
		"\0"
	};

	// NOTE: allocate vertex shader program
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);

	// NOTE: load the vertex shader's source code
	glShaderSource(vertShader, 1, &vertSource, NULL);

	// NOTE: compile the vertex shader's source code
	glCompileShader(vertShader);

	// NOTE: get the status of the compilation
	GLint status;
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);

	// NOTE: if the compilation failed print the error
	if(status == GL_FALSE) {
		char buffer[512];
		glGetShaderInfoLog(vertShader, 512, NULL, buffer);
		fprintf(stderr, "glCompileShader: Vertex\n%s\n", buffer);
		exit(-1);
	}

	// NOTE: source code for the fragment shader
	const GLchar* fragSource = {
		"#version 110\n"\
		"\n"\
		"varying vec2 fragTexcoord;\n"\
		"uniform sampler2D tex;\n"\
		"\n"\
		"void main() {\n"\
			"gl_FragColor = texture2D(tex, fragTexcoord)*vec4(1.0, 1.0, 1.0, 1.0);\n"\
		"}\n"\
		"\0"
	};

	// NOTE: allocate fragment shader program
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	// NOTE: load the fragment shader's source code
	glShaderSource(fragShader, 1, &fragSource, NULL);

	// NOTE: compile the vertex shader's source code
	glCompileShader(fragShader);

	// NOTE: get the status of the compilation
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);

	// NOTE: if the compilation failed print the error
	if(status == GL_FALSE) {
		char buffer[512];
		glGetShaderInfoLog(fragShader, 512, NULL, buffer);
		fprintf(stderr, "glCompileShader: Fragment\n%s\n", buffer);
		exit(-1);
	}

	// NOTE: create a shader program out of the vertex and fragment shaders
	GLuint shaderProgram = glCreateProgram();

	// NOTE: attach the vertex and fragment shaders
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

	// NOTE: link the shader program
	glLinkProgram(shaderProgram);

	// NTOE: get the status of linking the program
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);

	// NOTE: if the program failed to link print the error
	if(status == GL_FALSE) {
		char buffer[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
		fprintf(stderr, "glLinkProgram: \n%s\n", buffer);
		exit(-1);
	}

	// NOTE: use the newly compiled shader program
	glUseProgram(shaderProgram);

	/* END SET SHADER PROGRAM */
	// ========================================================================

	/* SET VERTEX INFORMATION */
	// ========================================================================

	// NOTE: create a vertex array object to store all the relationships
	// between vertex buffer objects and shader program attributes
	//GLuint verArray;
	//glGenVertexArrays(1, &verArray);

	// NOTE: bind to this vertex array when establishing all the connections
	// for the shaderProgram
	//glBindVertexArray(verArray);
	/*
	- if I every need to switch raw vertex data to program attributes all I
		need to do is bind a different vertex array object
	*/

	// NOTE: vertices for a triangle (clockwise)
	GLfloat vertices[5*m_iMaxNumVertices0];
	memset(vertices, 0, 5*m_iMaxNumVertices0);

	for(int i=0; i<m_iMaxNumVertices0; i++) {
		vertices[5*i+0] = m_pVertices0[i].x;
		vertices[5*i+1] = m_pVertices0[i].y;
		vertices[5*i+2] = m_pVertices0[i].z;

		vertices[5*i+3] = m_pVertices0[i].tu;
		vertices[5*i+4] = m_pVertices0[i].tv;
	}

	// NOTE: allocate an array buffer on the GPU
	GLuint verBuffer;
	glGenBuffers(1, &verBuffer);

	// NOTE: bind to the array buffer so that we may send our data to the GPU
	glBindBuffer(GL_ARRAY_BUFFER, verBuffer);

	// NOTE: send our vertex data to the GPU and set as STAIC
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// NOTE: get a pointer to the position attribute variable in the shader
	// program
	GLint posAttrib = glGetAttribLocation(shaderProgram, "pos");

	// NOTE: specify the stride (spacing) and offset for array buffer which
	// will be used in place of the attribute variable in the shader program
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	/*
	- this function call automatically directs the array buffer bound to
		GL_ARRAY_BUFFER towards the attribute in the shader program
	*/

	// NOTE: enable the attribute
	glEnableVertexAttribArray(posAttrib);

	// NOTE: get a pointer to the position attribute variable in the shader
	// program
	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");

	// NOTE: specify the stride (spacing) and offset for array buffer which
	// will be used in place of the attribute variable in the shader program
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));

	// NOTE: enable the attribute
	glEnableVertexAttribArray(texAttrib);

	/* END SET VERTEX INFORMATION */
	// ========================================================================

	/* SET ELEMENT INFORMATION */
	// ========================================================================

	// NOTE: index into the raw vertex array
	GLuint elements[m_iMaxNumIndices0];
	memset(elements, 0, m_iMaxNumIndices0);

	for(int i=0; i<m_iMaxNumIndices0; i++) {
		elements[i] = (GLuint) m_pIndices0[i];
	}

	// NOTE: allocate a GPU buffer for the element data
	GLuint eleBuffer;
	glGenBuffers(1, &eleBuffer);

	// NOTE: bind to the element buffer so that we may send our data to the GPU
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBuffer);

	// NOTE: send our element data to the GPU and set as STAIC
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	/* END SET ELEMENT INFORMATION */
	// ========================================================================

	/* SET TEXTURE INFORMATION */
	// ========================================================================

	// NOTE: allocate a GPU texture
	GLuint tex;
	glGenTextures(1, &tex);

	// NOTE: set the texture to unit 0
	glActiveTexture(GL_TEXTURE0);

	// NOTE: bind to the texture so that we may send our data to the GPU
	glBindTexture(GL_TEXTURE_2D, tex);

	// NOTE: send the pixels to the GPU (will have to convert enums from dxd to
	// opengl)
	GLenum texFormat;
	switch(HeaderOrg.Format) {
		case D3DFMT_DXT1: {
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		} break;
		case D3DFMT_DXT3: {
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		} break;
		case D3DFMT_DXT5: {
			texFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		} break;
		default: {
			fprintf(stderr, "ERROR: Unknown texture format.\n");
			exit(-1);
		} break;
	}

	glCompressedTexImage2D(GL_TEXTURE_2D, 0, texFormat, HeaderOrg.nWidth, HeaderOrg.nHeight, 0, compTexSize, compTexData);

	// NOTE: bind the uniform "tex" in the fragment shader to the unit 0
	// texture
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

	// NOTE: generate the mipmaps for scaling
	/*
	- can probably get rid of this when I'm loading all the iMMC
	*/
	glGenerateMipmapEXT(GL_TEXTURE_2D);

	/* END SET TEXTURE INFORMATION */
	// ========================================================================

	/* TESTING */
	// ========================================================================

	glm::mat4 model;
	float angle = (float) M_PI/500.0f;
	model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	float pDist = 4.2f;
	glm::mat4 view = glm::lookAt(
		glm::vec3(pDist, pDist, pDist),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(45.0f, 800.0f/600.0f, 1.0f, 60.0f);

	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	/* END TESTING */
	// ========================================================================

	/* === */

	/* === */

	// NOTE: start running
	running = SDL_TRUE;

	while(running) {
		// NOTE: poll for player input
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				// NOTE: check if window should close
				case SDL_QUIT: {
					running = SDL_FALSE;
				} break;

				// NOTE: check for keyup
				case SDL_KEYUP: {
					switch(event.key.keysym.sym) {
						case SDLK_UP: {
							pInput.up = SDL_FALSE;
						} break;
						case SDLK_DOWN: {
							pInput.down = SDL_FALSE;
						} break;
						case SDLK_LEFT: {
							pInput.left = SDL_FALSE;
						} break;
						case SDLK_RIGHT: {
							pInput.right = SDL_FALSE;
						} break;
					}
				} break;

				// NOTE: check for keydown
				case SDL_KEYDOWN: {
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE: running = SDL_FALSE;
						case SDLK_UP: {
							pInput.up = SDL_TRUE;
						} break;
						case SDLK_DOWN: {
							pInput.down = SDL_TRUE;
						} break;
						case SDLK_LEFT: {
							pInput.left = SDL_TRUE;
						} break;
						case SDLK_RIGHT: {
							pInput.right = SDL_TRUE;
						} break;
					}
				} break;
			}
		}

		if(pInput.up) {
			pDist -= 0.05f;
			glm::mat4 view = glm::lookAt(
				glm::vec3(pDist, pDist, pDist),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);

			GLint uniView = glGetUniformLocation(shaderProgram, "view");
			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		}

		if(pInput.down) {
			pDist += 0.05f;
			glm::mat4 view = glm::lookAt(
				glm::vec3(pDist, pDist, pDist),
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 1.0f, 0.0f)
			);

			GLint uniView = glGetUniformLocation(shaderProgram, "view");
			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
		}

		if(pInput.left) {
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		}

		if(pInput.right) {
			model = glm::rotate(model, -angle, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
		}

		// NOTE: clear the screen buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		/* === */

		// NOTE: draw to the screen
		glDrawElements(GL_TRIANGLES, m_iMaxNumIndices0, GL_UNSIGNED_INT, 0);

		/* === */

		// NOTE: swap the front and back buffers
		SDL_GL_SwapWindow(window);
	}

	/* === */

	// NOTE: free the GPU texture
	glDeleteTextures(1, &tex);

	// NOTE: free the arrays buffer on the GPU
	glDeleteBuffers(1, &eleBuffer);
	glDeleteBuffers(1, &verBuffer);

	// NOTE: free the vertex array attribute relation on the GPU
	//glDeleteVertexArrays(1, &verArray);

	// NOTE: detach the vertex and fragment shaders from the one shader program
	glDetachShader(shaderProgram, vertShader);
	glDetachShader(shaderProgram, fragShader);

	// NOTE: delete the shader program
	glDeleteProgram(shaderProgram);

	// NOTE: delete the compiled shaders from the GPU
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	/* === */

	N3Quit();
	return 0;
}

//-----------------------------------------------------------------------------
void N3LoadMesh(const char* szFN) {
	FILE* fpMesh = fopen(szFN, "rb");
	if(fpMesh == NULL) {
		fprintf(stderr, "\nERROR: Missing mesh %s\n", szFN);
		exit(-1);
	}
	/*
	*/

	// NOTE: length of the name for the mesh
	int nL0 = 0;
	fread(&nL0, sizeof(int), 1, fpMesh);

	// NOTE: if the shape has a mesh name read it in
	char m_szName0[(nL0+1)];

	if(nL0 > 0) {
		memset(&m_szName0, 0, (nL0+1));
		fread(&m_szName0, sizeof(char), nL0, fpMesh);
	}

	// NOTE: read in the number of "collapses"
	int m_iNumCollapses0;
	fread(&m_iNumCollapses0, sizeof(int), 1, fpMesh);

	// NOTE: read in the total index changes
	int m_iTotalIndexChanges0;
	fread(&m_iTotalIndexChanges0, sizeof(int), 1, fpMesh);

	// NOTE: read in the max num of vertices
	fread(&m_iMaxNumVertices0, sizeof(int), 1, fpMesh);

	// NOTE: read in the max num of indices
	fread(&m_iMaxNumIndices0, sizeof(int), 1, fpMesh);

	// NOTE: read in the min num of vertices
	int m_iMinNumVertices0;
	fread(&m_iMinNumVertices0, sizeof(int), 1, fpMesh);

	// NOTE: read in the min num of indices
	int m_iMinNumIndices0;
	fread(&m_iMinNumIndices0, sizeof(int), 1, fpMesh);

	// NOTE: free the previous vertex data
	if(m_pVertices0) {
		delete m_pVertices0;
		m_pVertices0 = NULL;
	}

	// NOTE: if there is a max vertex amount allocate space for it
	if(m_iMaxNumVertices0 > 0) {
		m_pVertices0 = new _N3VertexT1[m_iMaxNumVertices0];
		memset(m_pVertices0, 0, sizeof(_N3VertexT1)*m_iMaxNumVertices0);

		// NOTE: read in the vertex data
		fread(m_pVertices0, sizeof(_N3VertexT1), m_iMaxNumVertices0, fpMesh);
	}

	// NOTE: free the previous index data
	if(m_pIndices0) {
		delete m_pIndices0;
		m_pIndices0 = NULL;
	}

	// NOTE: if there is a max index amount allocate space for it
	if(m_iMaxNumIndices0 > 0) {
		m_pIndices0 = new unsigned short[m_iMaxNumIndices0];
		memset(m_pIndices0, 0, sizeof(unsigned short)*m_iMaxNumIndices0);

		// NOTE: read in the vertex data
		fread(m_pIndices0, sizeof(unsigned short), m_iMaxNumIndices0, fpMesh);
	}

	// NOTE: read in the "collapses" (I think this is used to set the vertices
	// based on how close the player is to the object)
	_N3EdgeCollapse m_pCollapses[(m_iNumCollapses0+1)];

	if(m_iNumCollapses0 > 0) {
		fread(m_pCollapses, sizeof(_N3EdgeCollapse), m_iNumCollapses0, fpMesh);
		memset(&m_pCollapses[m_iNumCollapses0], 0, sizeof(_N3EdgeCollapse));
	}

	// NOTE: read in the index changes
	int m_pAllIndexChanges[m_iTotalIndexChanges0];

	if(m_iTotalIndexChanges0 > 0) {
		fread(m_pAllIndexChanges, sizeof(int), m_iTotalIndexChanges0, fpMesh);
	}

	// NOTE: read in m_iLODCtrlValueCount
	int m_iLODCtrlValueCount0;
	fread(&m_iLODCtrlValueCount0, sizeof(int), 1, fpMesh);

	// NOTE: read in the LODCtrls (current size seems to be 0)
	_N3LODCtrlValue m_pLODCtrlValues[m_iLODCtrlValueCount0];

	if(m_iLODCtrlValueCount0) {
		fread(m_pLODCtrlValues, sizeof(_N3LODCtrlValue), m_iLODCtrlValueCount0, fpMesh);
	}

	// NOTE: the actual number of indices and vertices for the specific
	// collapse
	if(m_pAllIndexChanges) {
		int m_iNumIndices = 0;
		int m_iNumVertices = 0;

		int c = 0;
		int LOD = 0;

		int iDiff = m_pLODCtrlValues[LOD].iNumVertices - m_iNumVertices;

		while(m_pLODCtrlValues[LOD].iNumVertices > m_iNumVertices) {
			if(c >= m_iNumCollapses0) break;
			if(m_pCollapses[c].NumVerticesToLose+m_iNumVertices > m_pLODCtrlValues[LOD].iNumVertices)
				break;

			m_iNumIndices += m_pCollapses[c].NumIndicesToLose;
			m_iNumVertices += m_pCollapses[c].NumVerticesToLose;
		
			int tmp0 = m_pCollapses[c].iIndexChanges;
			int tmp1 = tmp0+m_pCollapses[c].NumIndicesToChange;

			for(int i=tmp0; i<tmp1; i++) {
				m_pIndices0[m_pAllIndexChanges[i]] = m_iNumVertices-1;
			}

			c++;
		}

		// NOTE: if we break on a collapse that isn't intended to be one we
		// should collapse up to then keep collapsing until we find one
		while(m_pCollapses[c].bShouldCollapse) {
			/*
			- not sure if this is really needed
			*/
			if(c >= m_iNumCollapses0) break;

			m_iNumIndices += m_pCollapses[c].NumIndicesToLose;
			m_iNumVertices += m_pCollapses[c].NumVerticesToLose;
		
			int tmp0 = m_pCollapses[c].iIndexChanges;
			int tmp1 = tmp0+m_pCollapses[c].NumIndicesToChange;

			for(int i=tmp0; i<tmp1; i++) {
				m_pIndices0[m_pAllIndexChanges[i]] = m_iNumVertices-1;
			}

			c++;
		}
	}
	
	// NOTE: display debug info
	if(debugMode) {
		printf("\nMeshName: %s\n", m_szName0);
		printf("m_iNumCollapses      -> %d\n", m_iNumCollapses0);
		printf("m_iTotalIndexChanges -> %d\n", m_iTotalIndexChanges0);
		printf("m_iMaxNumVertices    -> %d\n", m_iMaxNumVertices0);
		printf("m_iMaxNumIndices     -> %d\n", m_iMaxNumIndices0);
		printf("m_iMinNumVertices    -> %d\n", m_iMinNumVertices0);
		printf("m_iMinNumIndices     -> %d\n", m_iMinNumIndices0);
		printf("m_iLODCtrlValueCount -> %d\n", m_iLODCtrlValueCount0);
	}

	/*
	*/
	fclose(fpMesh);
}

//-----------------------------------------------------------------------------
void N3LoadTexture(const char* szFN) {
	FILE* fpTexture = fopen(szFN, "rb");
	if(fpTexture == NULL) {
		fprintf(stderr, "ERROR: Missing texture %s\n", szFN);
		exit(-1);
	}
	/*
	*/

	// NOTE: length of the texture name
	int nL0 = 0;
	fread(&nL0, sizeof(int), 1, fpTexture);

	// NOTE: if the texture has a name read it in
	char m_szName0[(nL0+1)];

	if(nL0 > 0) {
		memset(&m_szName0, 0, (nL0+1));
		fread(&m_szName0, sizeof(char), nL0, fpTexture);
	}

	// NOTE: read in the header information
	fread(&HeaderOrg, sizeof(_N3TexHeader), 1, fpTexture);

	struct _N3TexHeader {
		char szID[4];
		int nWidth;
		int nHeight;
		int Format;
		bool bMipMap;
	};

	// NOTE: display debug info
	if(debugMode) {
		printf("\nTexName: %s\n", m_szName0);
		printf("H.szID    -> %c%c%c%hu\n",
			HeaderOrg.szID[0],
			HeaderOrg.szID[1],
			HeaderOrg.szID[2],
			(unsigned char) HeaderOrg.szID[3]
		);
		printf("H.nWidth  -> %d\n", HeaderOrg.nWidth);
		printf("H.nHeight -> %d\n", HeaderOrg.nHeight);
		printf("H.Format  -> %d\n", HeaderOrg.Format);
		printf("H.bMipMap -> %d\n\n", HeaderOrg.bMipMap);
	}

	// NOTE: the textures contain multiple mipmap data "blocks"
	switch(HeaderOrg.Format) {
		case D3DFMT_DXT1: {
			compTexSize = (HeaderOrg.nWidth*HeaderOrg.nHeight/2);
		} break;
		case D3DFMT_DXT3: {
			compTexSize = (HeaderOrg.nWidth*HeaderOrg.nHeight);
		} break;
		case D3DFMT_DXT5: {
			compTexSize = (HeaderOrg.nWidth*HeaderOrg.nHeight*2);
			printf("D3DFMT_DXT5 tex: need to verify size\n");
		} break;
	}

	if(compTexData) {
		delete compTexData;
		compTexData = NULL;
	}

	compTexData = new unsigned char[compTexSize];
	fread(compTexData, sizeof(unsigned char), compTexSize, fpTexture);

	int nMMC = 1;
	if(HeaderOrg.bMipMap) {
		// NOTE: calculate the number of MipMap which are possible with this
		// texture
		nMMC = 0;
		for(
			int nW=HeaderOrg.nWidth, nH=HeaderOrg.nHeight;
			nW>=4 && nH>=4;
			nW/=2, nH/=2
		) nMMC++;
	} else {
		// NOTE: not a mipmap
		printf("HeaderOrg.bMipMap tex: need to implement non-mipmap textures\n");
		exit(-1);
	}

	/*
	*/
	fclose(fpTexture);
}

//-----------------------------------------------------------------------------
void N3Init(void) {
	// NOTE: initialize SDL2 library
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		exit(-1);
	}

	// NOTE: create game window
	window = SDL_CreateWindow(
		"N3Shape",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);

	// NOTE: check for window
	if(window == NULL) {
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		exit(-1);
	}

	// NOTE: create an OpenGL context for the window
	context = SDL_GL_CreateContext(window);

	// NOTE: check context
	if(context == NULL) {
		fprintf(stderr, "SDL_GL_CreateContext: %s\n", SDL_GetError());
		exit(-1);
	}

	// NOTE: set the buffer swap interval to get vsync
	if(SDL_GL_SetSwapInterval(1) != 0) {
		fprintf(stderr, "SDL_GL_SetSwapInterval: %s\n", SDL_GetError());
		exit(-1);
	}

	// NTOE: initialize the OpenGL library function calls
	GLenum glError = glewInit();

	// NOTE: check for error
	if(glError != GLEW_OK) {
		fprintf(stderr, "glewInit: %s\n", glewGetErrorString(glError));
		exit(-1);
	}

	// NOTE: enable the depth test
	glEnable(GL_DEPTH_TEST);
}

//-----------------------------------------------------------------------------
void N3Quit(void) {
	// NOTE: free the OpenGL context
	SDL_GL_DeleteContext(context);
	context = NULL;
	
	// NOTE: free the SDL2 window
	SDL_DestroyWindow(window);
	window = NULL;

	// NOTE: quit the SDL2 library
	SDL_Quit();
}
