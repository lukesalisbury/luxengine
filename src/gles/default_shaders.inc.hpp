/****************************
Copyright (c) 2012 Luke Salisbury
This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
****************************/

#ifndef _GLES_DEFAULT_SHADERS_
	#define _GLES_DEFAULT_SHADERS_


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
		"uniform vec4 FirstColour;\n"
		"uniform vec4 SecondColour;\n"
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
		  "		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, -0.025 )) );\n"
		  "		if (sum.a >= 0.5)\n"
		  "			border = true;\n"
		  "		sum = texture2D( tex0, abs(v_texCoord + vec2( -0.025, 0.00 )) );\n"
		  "		if (sum.a >= 0.5)\n"
		  "			border = true;\n"
		  "		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.00, 0.025 )) );\n"
		  "		if (sum.a >= 0.5)\n"
		  "			border = true;\n"
		  "		sum = texture2D( tex0, abs(v_texCoord + vec2( 0.025, 0.00 )) );\n"
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
#endif
