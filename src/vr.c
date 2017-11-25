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
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <stdio.h>
#include <string.h>
#include <openvr_capi.h>

#include "shader.h"
#include "3dmaths.h"
#include "glerror.h"
#include "main.h"

int vr_using = 0;

intptr_t VR_InitInternal( EVRInitError *peError, EVRApplicationType eType );
void VR_ShutdownInternal();
int VR_IsHmdPresent();
intptr_t VR_GetGenericInterface( const char *pchInterfaceVersion, EVRInitError *peError );
int VR_IsRuntimeInstalled();
const char * VR_GetVRInitErrorAsSymbol( EVRInitError error );
const char * VR_GetVRInitErrorAsEnglishDescription( EVRInitError error );


struct VR_IVRSystem_FnTable * OVR = NULL;
struct VR_IVRCompositor_FnTable * OVRC; 
// k_unMaxTrackedDeviceCount = 16	// gcc doesn't like the way this is declared
TrackedDevicePose_t m_rTrackedDevicePose [16];

struct FramebufferDesc
{
	GLuint m_nDepthBufferId;
	GLuint m_nRenderTextureId;
	GLuint m_nRenderFramebufferId;
	GLuint m_nResolveTextureId;
	GLuint m_nResolveFramebufferId;
};
struct FramebufferDesc leftEyeDesc;
struct FramebufferDesc rightEyeDesc;

uint32_t m_nRenderWidth;
uint32_t m_nRenderHeight;
int m_iValidPoseCount;
char m_strPoseClasses[16+1]; // should never get above 16 = k_unMaxTrackedDeviceCount
char m_rDevClassChar[16+1];
mat4x4 vrdevice_poses[16]; // 16 = k_unMaxTrackedDeviceCount
mat4x4 hmdPose;
mat4x4 eye_left;
mat4x4 eye_right;
GLSLSHADER *eye_prog;
GLuint eye_VAO, eye_VBO, eye_EAB;


//-----------------------------------------------------------------------------
// Purpose: Creates a frame buffer. Returns true if the buffer was set up.
//          Returns false if the setup failed.
//-----------------------------------------------------------------------------
bool CreateFrameBuffer( int nWidth, int nHeight, struct FramebufferDesc *framebufferDesc )
{
	glGenFramebuffers(1, &framebufferDesc->m_nRenderFramebufferId );
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc->m_nRenderFramebufferId);

	glGenRenderbuffers(1, &framebufferDesc->m_nDepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc->m_nDepthBufferId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight );
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,	framebufferDesc->m_nDepthBufferId );

	glGenTextures(1, &framebufferDesc->m_nRenderTextureId );
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc->m_nRenderTextureId );
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc->m_nRenderTextureId, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("framebuffer1 creation failed: %s\n", glErrorFb(status));
		return 1;
	}

	glGenFramebuffers(1, &framebufferDesc->m_nResolveFramebufferId );
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc->m_nResolveFramebufferId);

	glGenTextures(1, &framebufferDesc->m_nResolveTextureId );
	glBindTexture(GL_TEXTURE_2D, framebufferDesc->m_nResolveTextureId );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc->m_nResolveTextureId, 0);

	// check FBO status
	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("framebuffer2 creation failed\n");
		return 1;
	}
	printf("Framebuffer creation ok! %d, %d\n", nWidth, nHeight);
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	printf("Error = \"%s\"\n", glError(glGetError()));
	return 0;
}

void hmd_eye_calc(EVREye eye, mat4x4 * dest)
{
	float near = 0.1f;
	float far = 30.0f;

	mat4x4 proj = mov( OVR->GetProjectionMatrix( eye, near, far ) );
	mat4x4 pos = mov( OVR->GetEyeToHeadTransform( eye ) );
	pos = mat4x4_invert(pos);
	*dest = mul(proj, mul(pos, hmdPose));
}


void vr_init(void)
{
	printf("About to test for Headset\n");
	
	EVRInitError eError;

	if( VR_IsHmdPresent() )
	{
		printf("VR Headset Present\n");
	}

	printf("not dead yet\n");
	
	if( VR_IsRuntimeInstalled() )
	{
		printf("VR Runtime Installed\n");
	}

	printf("not dead yet\n");
	
	uint32_t vrToken = VR_InitInternal(&eError, EVRApplicationType_VRApplication_Scene);

	switch(eError){
		case EVRInitError_VRInitError_None:
			printf("all good\n");
			break;
		default:
			printf("error = %d\n", eError);
	}

	char fnTableName[128];
	int result1 = sprintf(fnTableName, "FnTable:%s", IVRSystem_Version);

	printf("FNTable: %s\n", fnTableName);
	OVR = (struct VR_IVRSystem_FnTable *)VR_GetGenericInterface(fnTableName, &eError);

	result1 = sprintf(fnTableName, "FnTable:%s", IVRCompositor_Version);
	printf("FNTableC: %s\n", fnTableName);
	OVRC = (struct VR_IVRCompositor_FnTable *)VR_GetGenericInterface(fnTableName, &eError);
	
	if (eError != EVRInitError_VRInitError_None)
		return;
	if (OVR == NULL)
		return;

	bool result2 = OVR->IsDisplayOnDesktop();

	if (result2)
		printf("Display is on desktop\n");
	else
		printf("Display is NOT on desktop\n");

	OVR->GetRecommendedRenderTargetSize( &m_nRenderWidth, &m_nRenderHeight );
	CreateFrameBuffer( m_nRenderWidth, m_nRenderHeight, &leftEyeDesc );
	CreateFrameBuffer( m_nRenderWidth, m_nRenderHeight, &rightEyeDesc );

	eye_prog = shader_load(
		"data/shaders/window.vert",
		"data/shaders/window.frag" );

	float eye_verts[] = {
		// left eye
		-0.9f, -0.9f, 0.0f, 0.0f,
		0.0f, -0.9f, 1.0f, 0.0f,
		0.0f, 0.9f, 1.0f, 1.0f,
		-0.9f, 0.9f, 0.0f, 1.0f,
		// right eye
		0.1f, -0.9f, 0.0f, 0.0f,
		0.9f, -0.9f, 1.0f, 0.0f,
		0.9f, 0.9f, 1.0f, 1.0f,
		0.1f, 0.9f, 0.0f, 1.0f
	};

	GLushort eye_ind[]  = { 0, 1, 2,   0, 2, 3,   4, 5, 6,   4, 6, 7};

	glGenVertexArrays( 1, &eye_VAO );
	glBindVertexArray( eye_VAO );

	glGenBuffers( 1, &eye_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, eye_VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof(eye_verts), &eye_verts[0], GL_STATIC_DRAW );

	glGenBuffers( 1, &eye_EAB );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, eye_EAB );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(eye_ind), &eye_ind[0], GL_STATIC_DRAW );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void *)0 );

	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void *)8 );

	glBindVertexArray( 0 );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	vr_using = 1;
}

void vr_end(void)
{
	VR_ShutdownInternal();
	vr_using = 0;
}

void vr_loop( void render(mat4x4) )
{
// Process OpenVR events
	struct VREvent_t vre;
	while( OVR->PollNextEvent(&vre, sizeof(vre)) )
	{
		switch(vre.eventType) {
			case EVREventType_VREvent_TrackedDeviceActivated:
				printf("device activated\n");
				break;
			case EVREventType_VREvent_TrackedDeviceDeactivated:
				printf("device deavtivated\n");
				break;
			case EVREventType_VREvent_TrackedDeviceUpdated:
				printf("device updated\n");
				break;
		}
	}
	
	// Process OpenVR Controller			// k_unMaxTrackedDeviceCount = 16
	for( TrackedDeviceIndex_t unDevice = 0; unDevice < 16; unDevice++)
	{
		struct VRControllerState_t state;
		if( OVR->GetControllerState( unDevice, &state, sizeof(state) ) )
		{
		//	printf(" it equals \"%d\"\n", state.ulButtonPressed );
		// it tracks buttons at least
			//printf(" it equals 0?\n");

		}
	}

// Process HMD Position
	// UpdateHMDMatrixPose()		// k_unMaxTrackedDeviceCount = 16
	OVRC->WaitGetPoses(m_rTrackedDevicePose, 16, NULL, 0);
	m_iValidPoseCount = 0;
	m_strPoseClasses[0] = 0;
		
	for(int nDevice = 0; nDevice < 16; nDevice++)
	if(m_rTrackedDevicePose[nDevice].bPoseIsValid)
	{
		m_iValidPoseCount++;
		// m_rmat4DevicePose[nDevice]
		vrdevice_poses[nDevice] = mov( m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking );
		if (m_rDevClassChar[nDevice]==0)
		{
			switch (OVR->GetTrackedDeviceClass(nDevice))
			{
			case ETrackedDeviceClass_TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
			case ETrackedDeviceClass_TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
			case ETrackedDeviceClass_TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
			case ETrackedDeviceClass_TrackedDeviceClass_GenericTracker:    m_rDevClassChar[nDevice] = 'G'; break;
			case ETrackedDeviceClass_TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
			default:                                       m_rDevClassChar[nDevice] = '?'; break;
			}
		}
		m_strPoseClasses[nDevice] += m_rDevClassChar[nDevice];
	}

	if ( m_rTrackedDevicePose[k_unTrackedDeviceIndex_Hmd].bPoseIsValid )
	{
		hmdPose = vrdevice_poses[k_unTrackedDeviceIndex_Hmd];
	}
	// Get hmd position matrices
	hmd_eye_calc(EVREye_Eye_Left, &eye_left);
	hmd_eye_calc(EVREye_Eye_Right, &eye_right);

// Render to the Headset
	glUseProgram( 0 );

	glEnable( GL_MULTISAMPLE );
	glBindFramebuffer( GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId );
	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight );

	// render scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render(eye_left);
	//render finish
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glDisable( GL_MULTISAMPLE );

	glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId );

	glBlitFramebuffer( 0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight, 
		GL_COLOR_BUFFER_BIT, GL_LINEAR );

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );

	glEnable( GL_MULTISAMPLE );

	// Right Eye
	glBindFramebuffer( GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
	glViewport(0, 0, m_nRenderWidth, m_nRenderHeight );
	// render scene
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render(eye_right);
	
	// render finish
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glDisable( GL_MULTISAMPLE );

	glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId );
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId );

	glBlitFramebuffer( 0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight, 
		GL_COLOR_BUFFER_BIT, GL_LINEAR );

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0 );

	// functions from example
	// RenderControllerAxes()
	// RenderStereoTargets()
	// RenderCompanionWindow()
	EVRCompositorError cErr;
	VRTextureBounds_t pBounds = { 0.0f, 0.0f, 1.0f, 1.0f };
	Texture_t leftEyeTexture = {(void*)(uintptr_t)leftEyeDesc.m_nResolveTextureId, ETextureType_TextureType_OpenGL, EColorSpace_ColorSpace_Gamma};
	cErr = OVRC->Submit(EVREye_Eye_Left, &leftEyeTexture, &pBounds, EVRSubmitFlags_Submit_Default);


	Texture_t rightEyeTexture = {(void*)(uintptr_t)rightEyeDesc.m_nResolveTextureId, ETextureType_TextureType_OpenGL, EColorSpace_ColorSpace_Gamma};
	cErr = OVRC->Submit(EVREye_Eye_Right, &rightEyeTexture, &pBounds, EVRSubmitFlags_Submit_Default);

// render to the monitor		
	glDisable(GL_DEPTH_TEST);
	glViewport( 0, 0, vid_width, vid_height );

	glBindVertexArray( eye_VAO );
	glUseProgram( eye_prog->prog );

	// render left eye (first half of index array )
	glBindTexture(GL_TEXTURE_2D, leftEyeDesc.m_nResolveTextureId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

	// render right eye (second half of index array )
	glBindTexture(GL_TEXTURE_2D, rightEyeDesc.m_nResolveTextureId  );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)(uintptr_t)(12) );

	glBindVertexArray( 0 );
	glUseProgram( 0 );
// work around to force HMD vsync from official example
	glFinish();

	glFlush();
	glFinish();
}