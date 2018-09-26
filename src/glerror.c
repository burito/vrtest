/*
Copyright (c) 2017 Daniel Burke

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif


/* Produces a string from the glGetError() return value */
// http://www.opengl.org/sdk/docs/man4/html/glGetError.xml
char* glError(int error)
{
	switch(error)
	{
		case GL_NO_ERROR:			// 0
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:			// 0x0500
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:			// 0x0501
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:		// 0x0502
			return "GL_INVALID_OPERATION";
#ifndef __APPLE__
		case GL_STACK_OVERFLOW: 		// 0x0503
			return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:	 	// 0x0504
			return "GL_STACK_UNDERFLOW";
#endif
		case GL_OUT_OF_MEMORY:			// 0x0505
			return "GL_OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION:	// 0x0506
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default:
			return "Unknown";
	}
}

// For Framebuffer error codes as may be returned by glCheckFramebufferStatus()
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCheckFramebufferStatus.xhtml
char* glErrorFb(GLenum status)
{
	switch(status) {
	case GL_FRAMEBUFFER_COMPLETE:			// 0x8CD5
		return "GL_FRAMEBUFFER_COMPLETE";
	case GL_FRAMEBUFFER_UNDEFINED:			// 0x8219
		return "GL_FRAMEBUFFER_UNDEFINED";
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:	// 0x8CD6
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	// 0x8CD7
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:	// 0x8CDB
		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:	// 0x8CDC
		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
	case GL_FRAMEBUFFER_UNSUPPORTED:		// 0x8CDD
		return "GL_FRAMEBUFFER_UNSUPPORTED";
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:	// 0x8D56
		return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:	// 0x8DA8
		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
	case 0:
		return "An error has occured";
	default:
		return "Unknown";	
	}
}
