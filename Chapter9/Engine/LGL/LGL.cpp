
// Autogenerated by GLExtractor 0.5.93 (GL3 mode) (C) Sergey Kosarevsky
// Part of Linderdaum Engine
// http://www.linderdaum.com

#include "LGL.h"
#include "LGLAPI.h"

bool LGL::clGLExtRetriever::ChooseAndSetPixelFormat( sLGLAPI* LGL3, DEVICE_CONTEXT_TYPE DeviceContext, int BitsPerPixel, int ZBufferBits, int StencilBits, int Multisample )
{
#if defined( _WIN32 )
	PIXELFORMATDESCRIPTOR PFD;

	memset( &PFD, 0, sizeof( PFD ) );

	PFD.nSize        = sizeof( PIXELFORMATDESCRIPTOR );
	PFD.nVersion     = 1;
	PFD.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PFD.iPixelType   = PFD_TYPE_RGBA;
	PFD.cColorBits   = static_cast<BYTE>( BitsPerPixel & 0xFF );
	PFD.cDepthBits   = static_cast<BYTE>( ZBufferBits  & 0xFF );
	PFD.cStencilBits = static_cast<BYTE>( StencilBits  & 0xFF );
	PFD.iLayerType   = PFD_MAIN_PLANE;

	int PixelFormat = 0;

	if ( LGL3->wglChoosePixelFormatARB )
	{
		const int Attribs[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,  GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,  GL_TRUE,
			WGL_ACCELERATION_ARB  ,  WGL_FULL_ACCELERATION_ARB,
			WGL_DOUBLE_BUFFER_ARB ,  GL_TRUE,
			WGL_PIXEL_TYPE_ARB    ,  WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB    ,  BitsPerPixel,
			WGL_DEPTH_BITS_ARB    ,  ZBufferBits,
			WGL_STENCIL_BITS_ARB  ,  StencilBits,
			WGL_SAMPLE_BUFFERS_ARB,  GL_TRUE,
			WGL_SAMPLES_ARB       ,  Multisample,
			0 // end
		};

		unsigned int Count = 0;

		LGL3->wglChoosePixelFormatARB( DeviceContext, Attribs, NULL, 1, &PixelFormat, &Count );

		if ( !PixelFormat )
		{
			PixelFormat = ::ChoosePixelFormat( DeviceContext, &PFD );
		}

		return ::SetPixelFormat( DeviceContext, PixelFormat, NULL ) == TRUE;
	}

	if ( !PixelFormat )
	{
		PixelFormat = ::ChoosePixelFormat( DeviceContext, &PFD );
	}

	return ::SetPixelFormat( DeviceContext, PixelFormat, &PFD ) == TRUE;
#else
	return true;
#endif
}

GL_CONTEXT_TYPE LGL::clGLExtRetriever::CreateContext( sLGLAPI* LGL3, DEVICE_CONTEXT_TYPE DeviceContext, int VersionMajor, int VersionMinor )
{
#if defined( _WIN32 )
	HGLRC RenderContext = 0;

	if ( LGL3->wglCreateContextAttribsARB )
	{
		printf( "Using wglCreateContextAttribsARB()\n" );

		const int Attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, VersionMajor,
			WGL_CONTEXT_MINOR_VERSION_ARB, VersionMinor,
			WGL_CONTEXT_LAYER_PLANE_ARB  , 0,
			WGL_CONTEXT_PROFILE_MASK_ARB , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0 // end
		};

		RenderContext = LGL3->wglCreateContextAttribsARB( DeviceContext, 0, Attribs );
	}
	else
	{
		printf( "Using glCreateContext()\n" );

		RenderContext = LGL3->lglCreateContext( DeviceContext );
	}

	return RenderContext;
#else
	return 0;
#endif
}

GL_CONTEXT_TYPE LGL::clGLExtRetriever::CreateContextFull( sLGLAPI* LGL3, DEVICE_CONTEXT_TYPE DeviceContext, int BitsPerPixel, int ZBufferBits, int StencilBits, int Multisample, int VersionMajor, int VersionMinor )
{
#if defined( _WIN32 )
	bool FormatSet = ChooseAndSetPixelFormat( LGL3, DeviceContext, BitsPerPixel, ZBufferBits, StencilBits, Multisample );

	if ( !FormatSet )
	{
		printf( "Can't set the pixel format\n" );
		return 0;
	}

	HGLRC RenderContext = CreateContext( LGL3, DeviceContext, VersionMajor, VersionMinor );

	if ( !RenderContext )
	{
		printf( "Can't create an OpenGL rendering context\n" );
		return 0;
	}

	if ( !MakeCurrent( LGL3, DeviceContext, RenderContext ) )
	{
		printf( "Can't make context current\n" );
		return 0;
	}

	Reload( LGL3 );

	return RenderContext;
#else
	return 0;
#endif
}

GL_CONTEXT_TYPE LGL::clGLExtRetriever::MakeCurrent( sLGLAPI* LGL3, DEVICE_CONTEXT_TYPE DeviceContext, GL_CONTEXT_TYPE RenderContext )
{
#if defined( _WIN32 )

	if ( !LGL3->lglMakeCurrent( DeviceContext, RenderContext ) )
	{
		// Can't activate the OpenGL rendering context
		return 0;
	}

	return RenderContext;
#else
	return 0;
#endif
}

void LGL::clGLExtRetriever::DeleteContext( sLGLAPI* LGL3, DEVICE_CONTEXT_TYPE DeviceContext, GL_CONTEXT_TYPE RenderContext )
{
#if defined( _WIN32 )
	LGL3->lglMakeCurrent( DeviceContext, 0 );
	LGL3->lglDeleteContext( RenderContext );
#endif
}

void* LGL::clGLExtRetriever::GetGLProc( sLGLAPI* LGL3, const char* Name ) const
{
#if defined( _WIN32 )
	void* Proc = NULL;

	if ( LGL3->lglGetProcAddress )
	{
		Proc = ( void* )LGL3->lglGetProcAddress( Name );
	}

	return Proc ? Proc : ( FLibHandle ? ( void* )::GetProcAddress( ( HMODULE )FLibHandle, Name ) : NULL );
#else
	return 0;
#endif
}

void LGL::clGLExtRetriever::Reload( sLGLAPI* LGL3 )
{
#if defined( _WIN32 )

	// use this to obtain GL 1.0 entry points
	if ( !FLibHandle )
	{
		FLibHandle = ( void* )::LoadLibrary( "opengl32.dll" );
	}

	// WGL
	LGL3->lglGetProcAddress = ( PFNwglGetProcAddress )::GetProcAddress( ( HMODULE )FLibHandle, "wglGetProcAddress" );
	LGL3->lglCreateContext = ( PFNwglCreateContext )::GetProcAddress( ( HMODULE )FLibHandle, "wglCreateContext" );
	LGL3->lglGetCurrentContext = ( PFNwglGetCurrentContext )::GetProcAddress( ( HMODULE )FLibHandle, "wglGetCurrentContext" );
	LGL3->lglMakeCurrent = ( PFNwglMakeCurrent )::GetProcAddress( ( HMODULE )FLibHandle, "wglMakeCurrent" );
	LGL3->lglDeleteContext = ( PFNwglDeleteContext )::GetProcAddress( ( HMODULE )FLibHandle, "wglDeleteContext" );
#endif

	GetAPI( LGL3 );
}

#if defined( _WIN32 )
// read OpenGL 3.x
void LGL::clGLExtRetriever::GetAPI( sLGLAPI* API ) const
{
	API->glActiveTexture = ( PFNGLACTIVETEXTUREPROC )GetGLProc( API, "glActiveTexture" );
	API->glAttachShader = ( PFNGLATTACHSHADERPROC )GetGLProc( API, "glAttachShader" );
	API->glBindAttribLocation = ( PFNGLBINDATTRIBLOCATIONPROC )GetGLProc( API, "glBindAttribLocation" );
	API->glBindBuffer = ( PFNGLBINDBUFFERPROC )GetGLProc( API, "glBindBuffer" );
	API->glBindFragDataLocation = ( PFNGLBINDFRAGDATALOCATIONPROC )GetGLProc( API, "glBindFragDataLocation" );
	API->glBindFramebuffer = ( PFNGLBINDFRAMEBUFFERPROC )GetGLProc( API, "glBindFramebuffer" );
	API->glBindTexture = ( PFNGLBINDTEXTUREPROC )GetGLProc( API, "glBindTexture" );
	API->glBindVertexArray = ( PFNGLBINDVERTEXARRAYPROC )GetGLProc( API, "glBindVertexArray" );
	API->glBlendFunc = ( PFNGLBLENDFUNCPROC )GetGLProc( API, "glBlendFunc" );
	API->glBufferData = ( PFNGLBUFFERDATAPROC )GetGLProc( API, "glBufferData" );
	API->glBufferSubData = ( PFNGLBUFFERSUBDATAPROC )GetGLProc( API, "glBufferSubData" );
	API->glCheckFramebufferStatus = ( PFNGLCHECKFRAMEBUFFERSTATUSPROC )GetGLProc( API, "glCheckFramebufferStatus" );
	API->glClear = ( PFNGLCLEARPROC )GetGLProc( API, "glClear" );
	API->glClearColor = ( PFNGLCLEARCOLORPROC )GetGLProc( API, "glClearColor" );
	API->glClearDepth = ( PFNGLCLEARDEPTHPROC )GetGLProc( API, "glClearDepth" );
	API->glClearStencil = ( PFNGLCLEARSTENCILPROC )GetGLProc( API, "glClearStencil" );
	API->glColorMask = ( PFNGLCOLORMASKPROC )GetGLProc( API, "glColorMask" );
	API->glCompileShader = ( PFNGLCOMPILESHADERPROC )GetGLProc( API, "glCompileShader" );
	API->glCreateProgram = ( PFNGLCREATEPROGRAMPROC )GetGLProc( API, "glCreateProgram" );
	API->glCreateShader = ( PFNGLCREATESHADERPROC )GetGLProc( API, "glCreateShader" );
	API->glCullFace = ( PFNGLCULLFACEPROC )GetGLProc( API, "glCullFace" );
	API->glDeleteBuffers = ( PFNGLDELETEBUFFERSPROC )GetGLProc( API, "glDeleteBuffers" );
	API->glDeleteFramebuffers = ( PFNGLDELETEFRAMEBUFFERSPROC )GetGLProc( API, "glDeleteFramebuffers" );
	API->glDeleteProgram = ( PFNGLDELETEPROGRAMPROC )GetGLProc( API, "glDeleteProgram" );
	API->glDeleteQueries = ( PFNGLDELETEQUERIESPROC )GetGLProc( API, "glDeleteQueries" );
	API->glDeleteShader = ( PFNGLDELETESHADERPROC )GetGLProc( API, "glDeleteShader" );
	API->glDeleteTextures = ( PFNGLDELETETEXTURESPROC )GetGLProc( API, "glDeleteTextures" );
	API->glDeleteVertexArrays = ( PFNGLDELETEVERTEXARRAYSPROC )GetGLProc( API, "glDeleteVertexArrays" );
	API->glDepthMask = ( PFNGLDEPTHMASKPROC )GetGLProc( API, "glDepthMask" );
	API->glDisable = ( PFNGLDISABLEPROC )GetGLProc( API, "glDisable" );
	API->glDisablei = ( PFNGLDISABLEIPROC )GetGLProc( API, "glDisablei" );
	API->glDisableVertexAttribArray = ( PFNGLDISABLEVERTEXATTRIBARRAYPROC )GetGLProc( API, "glDisableVertexAttribArray" );
	API->glDrawArrays = ( PFNGLDRAWARRAYSPROC )GetGLProc( API, "glDrawArrays" );
	API->glDrawElements = ( PFNGLDRAWELEMENTSPROC )GetGLProc( API, "glDrawElements" );
	API->glEnable = ( PFNGLENABLEPROC )GetGLProc( API, "glEnable" );
	API->glEnablei = ( PFNGLENABLEIPROC )GetGLProc( API, "glEnablei" );
	API->glEnableVertexAttribArray = ( PFNGLENABLEVERTEXATTRIBARRAYPROC )GetGLProc( API, "glEnableVertexAttribArray" );
	API->glFinish = ( PFNGLFINISHPROC )GetGLProc( API, "glFinish" );
	API->glFlush = ( PFNGLFLUSHPROC )GetGLProc( API, "glFlush" );
	API->glFramebufferTexture2D = ( PFNGLFRAMEBUFFERTEXTURE2DPROC )GetGLProc( API, "glFramebufferTexture2D" );
	API->glGenBuffers = ( PFNGLGENBUFFERSPROC )GetGLProc( API, "glGenBuffers" );
	API->glGenerateMipmap = ( PFNGLGENERATEMIPMAPPROC )GetGLProc( API, "glGenerateMipmap" );
	API->glGenFramebuffers = ( PFNGLGENFRAMEBUFFERSPROC )GetGLProc( API, "glGenFramebuffers" );
	API->glGenQueries = ( PFNGLGENQUERIESPROC )GetGLProc( API, "glGenQueries" );
	API->glGenTextures = ( PFNGLGENTEXTURESPROC )GetGLProc( API, "glGenTextures" );
	API->glGenVertexArrays = ( PFNGLGENVERTEXARRAYSPROC )GetGLProc( API, "glGenVertexArrays" );
	API->glGetActiveAttrib = ( PFNGLGETACTIVEATTRIBPROC )GetGLProc( API, "glGetActiveAttrib" );
	API->glGetActiveUniform = ( PFNGLGETACTIVEUNIFORMPROC )GetGLProc( API, "glGetActiveUniform" );
	API->glGetAttribLocation = ( PFNGLGETATTRIBLOCATIONPROC )GetGLProc( API, "glGetAttribLocation" );
	API->glGetError = ( PFNGLGETERRORPROC )GetGLProc( API, "glGetError" );
	API->glGetIntegerv = ( PFNGLGETINTEGERVPROC )GetGLProc( API, "glGetIntegerv" );
	API->glGetProgramInfoLog = ( PFNGLGETPROGRAMINFOLOGPROC )GetGLProc( API, "glGetProgramInfoLog" );
	API->glGetProgramiv = ( PFNGLGETPROGRAMIVPROC )GetGLProc( API, "glGetProgramiv" );
	API->glGetShaderInfoLog = ( PFNGLGETSHADERINFOLOGPROC )GetGLProc( API, "glGetShaderInfoLog" );
	API->glGetShaderiv = ( PFNGLGETSHADERIVPROC )GetGLProc( API, "glGetShaderiv" );
	API->glGetString = ( PFNGLGETSTRINGPROC )GetGLProc( API, "glGetString" );
	API->glGetStringi = ( PFNGLGETSTRINGIPROC )GetGLProc( API, "glGetStringi" );
	API->glGetTexImage = ( PFNGLGETTEXIMAGEPROC )GetGLProc( API, "glGetTexImage" );
	API->glGetTexLevelParameteriv = ( PFNGLGETTEXLEVELPARAMETERIVPROC )GetGLProc( API, "glGetTexLevelParameteriv" );
	API->glGetUniformLocation = ( PFNGLGETUNIFORMLOCATIONPROC )GetGLProc( API, "glGetUniformLocation" );
	API->glIsProgram = ( PFNGLISPROGRAMPROC )GetGLProc( API, "glIsProgram" );
	API->glIsShader = ( PFNGLISSHADERPROC )GetGLProc( API, "glIsShader" );
	API->glLinkProgram = ( PFNGLLINKPROGRAMPROC )GetGLProc( API, "glLinkProgram" );
	API->glPatchParameteri = ( PFNGLPATCHPARAMETERIPROC )GetGLProc( API, "glPatchParameteri" );
	API->glPixelStorei = ( PFNGLPIXELSTOREIPROC )GetGLProc( API, "glPixelStorei" );
	API->glPolygonMode = ( PFNGLPOLYGONMODEPROC )GetGLProc( API, "glPolygonMode" );
	API->glScissor = ( PFNGLSCISSORPROC )GetGLProc( API, "glScissor" );
	API->glProgramParameteri = ( PFNGLPROGRAMPARAMETERIPROC )GetGLProc( API, "glProgramParameteri" );
	API->glReadPixels = ( PFNGLREADPIXELSPROC )GetGLProc( API, "glReadPixels" );
	API->glShaderSource = ( PFNGLSHADERSOURCEPROC )GetGLProc( API, "glShaderSource" );
	API->glTexSubImage2D = ( PFNGLTEXSUBIMAGE2DPROC )GetGLProc( API, "glTexSubImage2D" );
	API->glTexImage2D = ( PFNGLTEXIMAGE2DPROC )GetGLProc( API, "glTexImage2D" );
	API->glTexParameterf = ( PFNGLTEXPARAMETERFPROC )GetGLProc( API, "glTexParameterf" );
	API->glTexParameteri = ( PFNGLTEXPARAMETERIPROC )GetGLProc( API, "glTexParameteri" );
	API->glUniform1f = ( PFNGLUNIFORM1FPROC )GetGLProc( API, "glUniform1f" );
	API->glUniform1fv = ( PFNGLUNIFORM1FVPROC )GetGLProc( API, "glUniform1fv" );
	API->glUniform1i = ( PFNGLUNIFORM1IPROC )GetGLProc( API, "glUniform1i" );
	API->glUniform1iv = ( PFNGLUNIFORM1IVPROC )GetGLProc( API, "glUniform1iv" );
	API->glUniform3fv = ( PFNGLUNIFORM3FVPROC )GetGLProc( API, "glUniform3fv" );
	API->glUniform4fv = ( PFNGLUNIFORM4FVPROC )GetGLProc( API, "glUniform4fv" );
	API->glUniformMatrix3fv = ( PFNGLUNIFORMMATRIX3FVPROC )GetGLProc( API, "glUniformMatrix3fv" );
	API->glUniformMatrix4fv = ( PFNGLUNIFORMMATRIX4FVPROC )GetGLProc( API, "glUniformMatrix4fv" );
	API->glUseProgram = ( PFNGLUSEPROGRAMPROC )GetGLProc( API, "glUseProgram" );
	API->glValidateProgram = ( PFNGLVALIDATEPROGRAMPROC )GetGLProc( API, "glValidateProgram" );
	API->glVertexAttribPointer = ( PFNGLVERTEXATTRIBPOINTERPROC )GetGLProc( API, "glVertexAttribPointer" );
	API->glViewport = ( PFNGLVIEWPORTPROC )GetGLProc( API, "glViewport" );

	API->wglChoosePixelFormatARB = ( PFNWGLCHOOSEPIXELFORMATARBPROC )GetGLProc( API, "wglChoosePixelFormatARB" );
	API->wglCreateContextAttribsARB = ( PFNWGLCREATECONTEXTATTRIBSARBPROC )GetGLProc( API, "wglCreateContextAttribsARB" );
	API->wglGetExtensionsStringARB = ( PFNWGLGETEXTENSIONSSTRINGARBPROC )GetGLProc( API, "wglGetExtensionsStringARB" );
	API->wglSwapIntervalEXT = ( PFNWGLSWAPINTERVALEXTPROC )GetGLProc( API, "wglSwapIntervalEXT" );
}
#endif

#if defined( ANDROID )
// read OpenGL ES 2.0

LGL_CALL void Emulate_glClearDepth( double Depth )
{
	glClearDepthf( static_cast<float>( Depth ) );
}

LGL_CALL void Emulate_glPolygonMode( GLenum, GLenum )
{
	// not supported
}

LGL_CALL void Emulate_glPatchParameteri( GLenum, GLint )
{
	// not supported
}

LGL_CALL void Emulate_glDisablei( GLenum mode, GLuint )
{
	glDisable( mode );
}

LGL_CALL void Emulate_glEnablei( GLenum mode, GLuint )
{
	glEnable( mode );
}

LGL_CALL void Emulate_glBindFragDataLocation( GLuint, GLuint, const GLchar* )
{
	// not supported
}

LGL_CALL void Emulate_glProgramParameteri( GLuint, GLenum, GLint )
{
	// not supported
}

LGL_CALL void Emulate_glGetTexLevelParameteriv( GLenum, GLint, GLenum, GLint* )
{
	// not supported
}

#define BIND_EMULATE( VarName, BindName, EmulProc ) VarName = BindName; VarName = (VarName ? VarName : EmulProc);

void LGL::clGLExtRetriever::GetAPI( sLGLAPI* API ) const
{
	API->glActiveTexture = &glActiveTexture;
	API->glAttachShader = &glAttachShader;
	API->glBindAttribLocation = &glBindAttribLocation;
	API->glBindBuffer = &glBindBuffer;
	API->glBindFragDataLocation = &Emulate_glBindFragDataLocation;
	API->glBindFramebuffer = &glBindFramebuffer;
	API->glBindTexture = &glBindTexture;
	API->glBlendFunc = &glBlendFunc;
	API->glBufferData = &glBufferData;
	API->glBufferSubData = &glBufferSubData;
	API->glCheckFramebufferStatus = &glCheckFramebufferStatus;
	API->glClear = &glClear;
	API->glClearColor = &glClearColor;
	API->glClearDepth = &Emulate_glClearDepth;
	API->glClearStencil = &glClearStencil;
	API->glColorMask = &glColorMask;
	API->glCompileShader = &glCompileShader;
	API->glCreateProgram = &glCreateProgram;
	API->glCreateShader = &glCreateShader;
	API->glCullFace = &glCullFace;
	API->glDeleteBuffers = &glDeleteBuffers;
	API->glDeleteFramebuffers = &glDeleteFramebuffers;
	API->glDeleteProgram = &glDeleteProgram;
	API->glDeleteShader = &glDeleteShader;
	API->glDeleteTextures = &glDeleteTextures;
	API->glDepthMask = &glDepthMask;
	API->glDisable = &glDisable;
	API->glDisablei = &Emulate_glDisablei;
	API->glDisableVertexAttribArray = &glDisableVertexAttribArray;
	API->glDrawArrays = &glDrawArrays;
	API->glDrawElements = &glDrawElements;
	API->glEnable = &glEnable;
	API->glEnablei = &Emulate_glEnablei;
	API->glEnableVertexAttribArray = &glEnableVertexAttribArray;
	API->glFinish = &glFinish;
	API->glFlush = &glFlush;
	API->glFramebufferTexture2D = &glFramebufferTexture2D;
	API->glGenBuffers = &glGenBuffers;
	API->glGenerateMipmap = &glGenerateMipmap;
	API->glGenFramebuffers = &glGenFramebuffers;
	API->glGenTextures = &glGenTextures;
	API->glGetActiveAttrib = &glGetActiveAttrib;
	API->glGetActiveUniform = &glGetActiveUniform;
	API->glGetAttribLocation = &glGetAttribLocation;
	API->glGetError = &glGetError;
	API->glGetIntegerv = &glGetIntegerv;
	API->glGetProgramInfoLog = &glGetProgramInfoLog;
	API->glGetProgramiv = &glGetProgramiv;
	API->glGetShaderInfoLog = &glGetShaderInfoLog;
	API->glGetShaderiv = &glGetShaderiv;
	API->glGetString = ( PFNGLGETSTRINGPROC )&glGetString;
	API->glGetTexLevelParameteriv = &Emulate_glGetTexLevelParameteriv;
	API->glGetUniformLocation = &glGetUniformLocation;
	API->glIsProgram = &glIsProgram;
	API->glIsShader = &glIsShader;
	API->glLinkProgram = &glLinkProgram;
	API->glPatchParameteri = &Emulate_glPatchParameteri;
	API->glPixelStorei = &glPixelStorei;
	API->glPolygonMode = &Emulate_glPolygonMode;
	API->glScissor = &glScissor;
	API->glProgramParameteri = &Emulate_glProgramParameteri;
	API->glReadPixels = &glReadPixels;
	// OpenGL ES 2 and OpenGL ES 3 have different "const" in prototype of glShaderSource
	API->glShaderSource = ( PFNGLSHADERSOURCEPROC )&glShaderSource;
	API->glTexSubImage2D = &glTexSubImage2D;
	API->glTexImage2D = &glTexImage2D;
	API->glTexParameterf = &glTexParameterf;
	API->glTexParameteri = &glTexParameteri;
	API->glUniform1f = &glUniform1f;
	API->glUniform1fv = &glUniform1fv;
	API->glUniform1i = &glUniform1i;
	API->glUniform1iv = &glUniform1iv;
	API->glUniform3fv = &glUniform3fv;
	API->glUniform4fv = &glUniform4fv;
	API->glUniformMatrix3fv = &glUniformMatrix3fv;
	API->glUniformMatrix4fv = &glUniformMatrix4fv;
	API->glUseProgram = &glUseProgram;
	API->glValidateProgram = &glValidateProgram;
	API->glVertexAttribPointer = &glVertexAttribPointer;
	API->glViewport = &glViewport;
}
#endif // ANDROID