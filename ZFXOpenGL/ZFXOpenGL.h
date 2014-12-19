#ifndef _ZFXOPENGL_H_
#define _ZFXOPENGL_H_


#include <windows.h>
#include <gl\glew.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\freeglut.h>
#include "ZFXRenderDevice.h"
#include "zfx3d.h"
#include "ZFX.h"
#include <string>
#include <math.h>
#include <map>
#include <vector>

#include "ZFXGLSL.h"

BOOL WINAPI DllEntryPoint(HINSTANCE hDll,
	DWORD fdwReason,
	LPVOID lpvRserved);

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "zfx3d.lib")

#define _USE_MATH_DEFINES
#define RADIAN2DEGREE(radian) ((radian) * 180.0f / M_PI)
#define DEGREE2RADIAN(degree) ((degree) * M_PI / 180.0f)

#ifdef _DEBUG

#define CHECK_ERROR {\
	GLenum error = glGetError();\
	if(error != GL_NO_ERROR)\
		 Log("OpenGL Error > File:%s Line:%d Error:%d",__FILE__, __LINE__, error);\
}

#else

#define CHECK_ERROR {}

#endif



class ZFXOpenGL :
	public ZFXRenderDevice
{
public:
	enum
	{
		MAX_ID = 65535,
	};
	ZFXOpenGL(HINSTANCE hDLL);
	~ZFXOpenGL();

	std::string GetName();

	void MakeGLMatrix(GLfloat gl_matrix[16], ZFXMatrix matrix);

	ZFXSkinManager* GetSkinManager(void)
	{
		return m_pSkinMan;
	}

	ZFXVertexCacheManager* GetVertexManager(void)
	{
		return m_pVertexMan;
	}

	// 初始化引擎
	HRESULT Init(HWND mainWnd, const HWND* childWnds, int nWndsNum, int nMinDepth, int nMinStencil, bool bSaveLog);
	HRESULT InitWindowed(HWND mainWnd, const HWND* childWnds, int nWndsNum, bool bSaveLog);

	// 图形设备接口实现

	void Release(void);

	bool IsRunning(void)
	{
		return m_bRunning;
	}

	bool IsWindowed(void)
	{
		return m_bWindowed;
	}

	// 获取主窗口大小
	void GetResolution(POINT* pPt)
	{
		pPt->x = m_dwWidth;
		pPt->y = m_dwHeight;
	}

	void UseShaders(bool);
	bool IsUseShaders(void)
	{
		return m_bUseShaders;
	}

	bool CanDoShaders(void)
	{
		return m_bCanDoShaders;
	}

	HRESULT SetShaderConstant(ZFXSHADERTYPE, ZFXDATATYPE,
		UINT, UINT, const void*);

	void UseAdditiveBlending(bool);
	bool IsUseAdditiveBlending(void)
	{
		return m_bAdditive;
	}

	HRESULT SetView3D(const ZFXVector&, const ZFXVector&, const ZFXVector&, const ZFXVector&);
	HRESULT SetViewLookAt(const ZFXVector&, const ZFXVector&, const ZFXVector&);

	void SetClippingPlanes(float, float) override;

	HRESULT SetMode(ZFXENGINEMODE, int n) override;

	void SetOrthoScale(float fScale, int nStage) override;

	HRESULT InitStage(float, ZFXVIEWPORT*, int n) override;

	HRESULT GetFrustrum(ZFXPlane*) override;

	void Transform2Dto3D(const POINT &pt, ZFXVector *vcOrig, ZFXVector *vcDir) override;

	ZFXVector Transform2Dto2D(UINT nHwnd, float, const POINT*, ZFXAXIS) override;

	POINT Transform3Dto2D(const ZFXVector &vcPoint) override;

	void SetWorldTransform(const ZFXMatrix*) override;

	void SetBackfaceCulling(ZFXRENDERSTATE) override;

	void SetStencilBufferMode(ZFXRENDERSTATE, DWORD) override;

	void UseStencilShadowSettings(bool) override;

	void UseColorBuffer(bool) override;

	bool IsUseColorBuffer(void) override;

	void UseTextures(bool) override;

	bool IsUseTextures(void) override;

	void SetDepthBufferMode(ZFXRENDERSTATE) override;

	void SetShadeMode(ZFXRENDERSTATE, float, const ZFXCOLOR*) override;

	ZFXRENDERSTATE GetShadeMode(void) override;

	HRESULT SetTextureStage(UCHAR, ZFXRENDERSTATE) override;

	HRESULT SetLight(const ZFXLIGHT*, UCHAR) override;

	HRESULT CreateVShader(const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID) override;

	HRESULT CreatePShader(const void *pData, UINT nSize, bool bLoadFromFile, bool bIsCompiled, UINT *pID) override;

	HRESULT ActivateVShader(UINT id, ZFXVERTEXID VertexID) override;

	HRESULT ActivatePShader(UINT id) override;

	HRESULT UseWindow(UINT nHwnd) override;

	HRESULT BeginRendering(bool bClearPixel, bool bClearDepth, bool bClearStencil) override;

	void EndRendering(void) override;

	HRESULT Clear(bool, bool, bool) override;

	void SetClearColor(float fRed, float fGreen, float fBlue) override;

	void FadeScreen(float fR, float fG, float fB, float fA) override;

	HRESULT CreateFont(const char*, int, bool, bool, bool, DWORD, UINT*) override;

	HRESULT DrawText(UINT, int, int, UCHAR, UCHAR, UCHAR, char*, ...) override;

	HRESULT DrawText(UINT, int, int, DWORD, char*, ...) override;

	HRESULT DrawText(UINT, int, int, DWORD, char*) override;

	void SetAmbientLight(float fRed, float fGreen, float fBlue) override;

	
	// GL helper
	bool ActivateGLTextureUnit(UCHAR n);

	UINT GetActiveSkinID(void)     { return m_nActiveSkin; }
	void SetActiveSkinID(UINT nID) { m_nActiveSkin = nID; }
	UINT GetActiveVShadID(void)    { return m_nActiveVShader; }
	UINT GetActiveFShadID(void)    { return m_nActiveFShader; }
	ZFXCOLOR GetWireColor()
	{
		return m_WireColor;
	}

	GLenum GetTextureOp(int n);

private:
	HINSTANCE m_hDLL;
	HDC m_hDC[MAX_3DHWND];
	HGLRC m_hRC;
	bool m_bStencil;
	bool m_bTextures;
	ZFXCOLOR m_ClearColor;
	ZFXCOLOR m_WireColor;
	bool m_bIsSceneRunning;
	GLenum m_TextureOp[8];
	UCHAR m_nActivateTextureUnit;
	GLSLManager *m_pGLSLManager;
	std::string m_name;
	UINT m_nActiveSkin;
	UINT m_nActiveVShader;
	UINT m_nActiveFShader;

	ZFXMatrix m_mProj2D;
	ZFXMatrix m_mView2D;
	ZFXMatrix m_mWorld2D;


	// 透视投影
	ZFXMatrix m_mProjP[4];
	// 正交投影
	ZFXMatrix m_mProjO[4];
	// 3D View
	ZFXMatrix m_mView3D;
	ZFXMatrix m_mWorld3D;

	// 保存组合矩阵
	ZFXMatrix m_mViewProj;
	ZFXMatrix m_mWorldViewProj;

	void Prepare2D(void);
	void CalcViewProjMatrix(void);
	void CalcWorldViewProjMatrix(void);
	HRESULT CalcPerspProjMatrix(int nStage);
	HRESULT CalcPerspProjMatrix(float fFov, float fAspect, int nStage);
	HRESULT CalcOrthoProjMatrix(int nStage);

	bool InitPixelFormat(int nHWnd);

	HRESULT Go(void);

	void Log(char *, ...);
};

extern "C" _declspec(dllexport) HRESULT CreateRenderDevice(HINSTANCE hDLL, ZFXRenderDevice **pInterface);

extern "C" _declspec(dllexport) HRESULT ReleaseRenderDevice(ZFXRenderDevice **pInterface);

#endif