/****************************
Copyright © 2006-2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/


bool shaders_supported = false;

char * shadertext = nullptr;


GLuint gvPositionHandle;


static ShaderData shaders[NUM_SHADERS] = {
	/* SHADER_DEFAULT */
	{ 0, 0, 0,
	/* vertex shader */
	"attribute vec4 vPosition;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vPosition;\n"
	"}",
	/* fragment shader */
	"precision mediump float;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = vec4 ( 1.0, 1.0, 1.0, 1.0 );\n"
	"}"
	},
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


static bool CompileShaderCode(GLenum shader, const char *source)
{
	GLint status;

	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == 0)
	{
		GLint length;
		char *info = nullptr;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		info = new char(length+1);
		glGetShaderInfoLog(shader, length, nullptr, info);
		std::cerr << "Failed to compile shader (" << source << "):" << info << std::endl;
		delete info;
	
	}
	return !!status;
}

void CompileShader( ShaderData * data )
{
	/* Create one program object to rule them all */
	data->program = glCreateProgram();

	/* Create the vertex shader */
	data->vert_shader = glCreateShader(GL_VERTEX_SHADER);
	if (!CompileShaderCode(data->vert_shader, data->vert_source))
	{
		shaders_supported = false;
		return;
	}

	/* Create the fragment shader */
	data->frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!CompileShaderCode(data->frag_shader, data->frag_source))
	{
		shaders_supported = false;
		return;
	}

	/* ... and in the darkness bind them */
	glAttachShader(data->program, data->vert_shader);
	glAttachShader(data->program, data->frag_shader);
	glLinkProgram(data->program);


}

void setShaders()
{
	for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
	{
		CompileShader( &shaders[i] );
	}
	gvPositionHandle = glGetAttribLocation(shaders[0].program, "vPosition");
}

void closeShader()
{
	for ( uint8_t i = 0; i < NUM_SHADERS; ++i)
	{
		glDeleteShader(shaders[i].vert_shader);
		glDeleteShader(shaders[i].frag_shader);
		glDeleteProgram(shaders[i].program);
	}
}


void enableShader( uint8_t shader, Effects effect )
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

	glUseProgram( shaders[SHADER_DEFAULT].program );
	if ( s < NUM_SHADERS && s != SHADER_DEFAULT )
	{
		glUseProgram( shaders[s].program );
		GLint loc1 = glGetAttribLocation(shaders[s].program, "FirstColour");
		GLint loc2 = glGetAttribLocation(shaders[s].program, "SecondColour");
		glUniform4f(loc1, (float)effect.colour.r/255.0, (float)effect.colour.g/255.0, (float)effect.colour.b/255.0, (float)effect.colour.a/255.0 );
		glUniform4f(loc2, (float)effect.colour2.r/255.0, (float)effect.colour2.g/255.0, (float)effect.colour2.b/255.0, (float)effect.colour2.a/255.0 );
	}
}
