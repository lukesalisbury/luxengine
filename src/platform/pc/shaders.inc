/****************************
Copyright © 2011 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

static PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
static PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
static PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
static PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
static PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
static PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
static PFNGLUNIFORM4FARBPROC glUniform4fARB;
static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;

enum {
	SHADER_GLOW,
	SHADER_NEGATIVE,
	SHADER_REPLACE,
	NUM_SHADERS
};

typedef struct {
	GLuint program;
	GLuint vert_shader;
	GLuint frag_shader;
	const char * vert_source;
	const char * frag_source;
} ShaderData;

bool shaders_supported = false;

char * shadertext = NULL;

static ShaderData shaders[NUM_SHADERS] = {
	/* SHADER_GLOW */
	{ 0, 0, 0,
/* vertex shader */
"varying vec4 v_color;\n"
"varying vec2 v_texCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"	v_color = gl_Color;\n"
"	v_texCoord = vec2(gl_MultiTexCoord0);\n"
"}",
/* fragment shader */
"varying vec4 v_color;\n"
"varying vec2 v_texCoord;\n"
"uniform sampler2D tex0;\n"
"uniform vec4 FirstColour;\n"
"uniform vec4 SecondColour;\n"
"void main()\n"
"{\n"
"	vec4 color = texture2D(tex0, v_texCoord);\n"
"	vec4 sum;\n"
"	bool border = false;\n"
"	if (color.a < 0.9)\n"
"	{\n"
"		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, -0.05 )) );\n"
"		if (sum.a >= 0.5)\n"
"			border = true;\n"
"		sum = texture2D( tex0, abs(v_texCoord + vec2( -0.05, 0.00 )) );\n"
"		if (sum.a >= 0.5)\n"
"			border = true;\n"
"		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, 0.05 )) );\n"
"		if (sum.a >= 0.5)\n"
"			border = true;\n"
"		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.05, 0.00 )) );\n"
"		if (sum.a >= 0.5)\n"
"			border = true;\n"
"	}\n"
"	if ( border )\n"
"	{\n"
"		gl_FragColor = SecondColour;\n"
"	}\n"
"	else\n"
"	{\n"
"		gl_FragColor = color;\n"
"	}\n"
"}"
	},
/* SHADER_NEGATIVE */
	{ 0, 0, 0,
/* vertex shader */
"varying vec4 v_color;\n"
"varying vec2 v_texCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"	v_color = gl_Color;\n"
"	v_texCoord = vec2(gl_MultiTexCoord0);\n"
"}",
/* fragment shader */
"varying vec4 v_color;\n"
"varying vec2 v_texCoord;\n"
"uniform sampler2D tex0;\n"
"uniform vec4 FirstColour;\n"
"uniform vec4 SecondColour;\n"
"\n"
"void main()\n"
"{\n"
"	vec4 color = texture2D(tex0, v_texCoord);\n"
"	vec4 result;\n"
"	result.r=1.0-color.r;\n"
"	result.g=1.0-color.g;\n"
"	result.b=1.0-color.b;\n"
"	result.a=color.a;\n"
"	gl_FragColor = result;\n"
"}",
},
/* SHADER_REPLACE */
	{ 0, 0, 0,
/* vertex shader */
"varying vec4 v_color;\n"
"varying vec2 v_texCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"	v_color = gl_Color;\n"
"	v_texCoord = vec2(gl_MultiTexCoord0);\n"
"}",
/* fragment shader */
"varying vec4 v_color;\n"
"varying vec2 v_texCoord;\n"
"uniform sampler2D tex0;\n"
"uniform vec4 FirstColour;\n"
"uniform vec4 SecondColour;\n"
"\n"
"void main()\n"
"{\n"
"	vec4 color = texture2D(tex0, v_texCoord);\n"
"	if (FirstColour == color) {\n"
"		gl_FragColor = SecondColour;\n"
"	} else {\n"
"		gl_FragColor = color;\n"
"	}\n"
"}"
	}
};

bool Lux_OGL_QueryExtension(char *extName);

static bool CompileShaderCode(GLenum shader, const char *source)
{
	GLint status;

	glShaderSourceARB(shader, 1, &source, NULL);
	glCompileShaderARB(shader);
	glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
	if (status == 0)
	{
		GLint length;
		char *info = NULL;

		glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
		info = new char(length+1);
		glGetInfoLogARB(shader, length, NULL, info);
		std::cerr << "Failed to compile shader (" << source << "):" << info << std::endl;
		delete info;
	
	}
	return !!status;
}

void CompileShader( ShaderData * data )
{
	/* Create one program object to rule them all */
	data->program = glCreateProgramObjectARB();

	/* Create the vertex shader */
	data->vert_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	if (!CompileShaderCode(data->vert_shader, data->vert_source))
	{
		shaders_supported = false;
		return;
	}
	//glShaderSourceARB(data->vert_shader, 1, &data->vert_source, NULL);
	//glCompileShaderARB(data->vert_shader);

	/* Create the fragment shader */
	data->frag_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	if (!CompileShaderCode(data->frag_shader, data->frag_source))
	{
		shaders_supported = false;
		return;
	}
	//glShaderSourceARB(data->frag_shader, 1, &data->frag_source, NULL);
	//glCompileShaderARB(data->frag_shader);

	/* ... and in the darkness bind them */
	glAttachObjectARB(data->program, data->vert_shader);
	glAttachObjectARB(data->program, data->frag_shader);
	glLinkProgramARB(data->program);


}

void setShaders()
{
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | GL_ARB_shader_objects :" << Lux_OGL_QueryExtension("GL_ARB_shader_objects") << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | GL_ARB_shading_language_100 :" << Lux_OGL_QueryExtension("GL_ARB_shading_language_100") << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | GL_ARB_vertex_shader :" << Lux_OGL_QueryExtension("GL_ARB_vertex_shader") << std::endl;
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | GL_ARB_fragment_shader :" << Lux_OGL_QueryExtension("GL_ARB_fragment_shader") << std::endl;
	if ( !lux::config->GetBoolean("shader.able") )
	{
		return;
	}
	
	#if defined(__APPLE__)
	lux::core->SystemMessage(SYSTEM_MESSAGE_INFO) << __FILE__ << ": | Shaders disabled on OS X" << std::endl;
	return;
	#endif

	if ( Lux_OGL_QueryExtension("GL_ARB_shader_objects") && Lux_OGL_QueryExtension("GL_ARB_shading_language_100") && Lux_OGL_QueryExtension("GL_ARB_vertex_shader") && Lux_OGL_QueryExtension("GL_ARB_fragment_shader") )
	{

		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
		glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) SDL_GL_GetProcAddress("glCompileShaderARB");
		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
		glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
		glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetUniformLocationARB");
		glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) SDL_GL_GetProcAddress("glLinkProgramARB");
		glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glShaderSourceARB");
		glUniform4fARB = (PFNGLUNIFORM4FARBPROC) SDL_GL_GetProcAddress("glUniform4fARB");
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
		shaders_supported = true;

		

		//elix::File * shader = new elix::File("shader.txt");
		//shader->ReadAll( (mem_pointer*)&shadertext, true );
		//delete shader;

		for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
		{
			CompileShader( &shaders[i] );
		}

	}
}

void closeShader()
{
	if ( shaders_supported )
	{
		for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
		{
			if (glDeleteObjectARB)
			{
				glDeleteObjectARB(shaders[i].vert_shader);
				glDeleteObjectARB(shaders[i].frag_shader);
				glDeleteObjectARB(shaders[i].program);
			}
		}
	}
}


void enableShader( uint8_t shader, ObjectEffect effect )
{
	if ( shaders_supported )
	{
		uint8_t s = NUM_SHADERS;
		if ( effect.style == STYLE_NEGATIVE )
		{
			s = SHADER_NEGATIVE;
		}
		else if ( effect.style == STYLE_REPCOLOUR )
		{
			s = SHADER_REPLACE;
		}
		else if ( effect.style == STYLE_GLOW || effect.style == STYLE_OUTLINE )
		{
			s = SHADER_GLOW;
		}

		if ( shader && s < NUM_SHADERS )
		{
			glUseProgramObjectARB( shaders[s].program );
			GLint loc1 = glGetUniformLocationARB(shaders[s].program,"FirstColour");
			GLint loc2 = glGetUniformLocationARB(shaders[s].program,"SecondColour");
			glUniform4fARB(loc1, (float)effect.primary_colour.r/255.0, (float)effect.primary_colour.g/255.0, (float)effect.primary_colour.b/255.0, (float)effect.primary_colour.a/255.0 );
			glUniform4fARB(loc2, (float)effect.secondary_colour.r/255.0, (float)effect.secondary_colour.g/255.0, (float)effect.secondary_colour.b/255.0, (float)effect.secondary_colour.a/255.0 );
		}
		else
		{
			glUseProgramObjectARB( 0 );
		}
	}
}
