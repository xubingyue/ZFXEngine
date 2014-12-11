#include "ZFXOpenGL_Skin.h"


ZFXOpenGLSkinManager::ZFXOpenGLSkinManager()
{
	m_nNumMaterials = 0;
	m_nNumTextures = 0;
	m_nNumSkins = 0;
	m_pMaterials = NULL;
	m_pTextures = NULL;
	m_pSkins = NULL;
	Log("Skin Manager Create");
}

ZFXOpenGLSkinManager::~ZFXOpenGLSkinManager()
{
	// �ͷ�D3D����������
	if (m_pTextures)
	{
		// Ӧ��Ϊ ZFXTEXTURE ʵ����������
		for (UINT i = 0; i < m_nNumTextures; i++)
		{
			if (m_pTextures[i].pData)
			{
				// ��������
				//((LPDIRECT3DTEXTURE9)(m_pTextures[i].pData))->Release();
				//m_pTextures[i].pData = NULL;
			}
			if (m_pTextures[i].pClrKeys)
			{
				// ��ɫ��ֵ
				delete[] m_pTextures[i].pClrKeys;
				m_pTextures[i].pClrKeys = NULL;
			}
			if (m_pTextures[i].chName)
			{
				// ��������
				delete[] m_pTextures[i].chName;
				m_pTextures[i].chName = NULL;
			}
		}
		free(m_pTextures);
		m_pTextures = NULL;
	}

	// �ͷŲ��ʶ���
	if (m_pMaterials)
	{
		free(m_pMaterials);
		m_pMaterials = NULL;
	}

	// �ͷ�Ƥ��
	if (m_pSkins)
	{
		free(m_pSkins);
		m_pSkins = NULL;
	}
	Log("Skin Manager Release");
}

void ZFXOpenGLSkinManager::Reset(void)
{
	// �ͷ�D3D����������
	if (m_pTextures)
	{
		// Ӧ��Ϊ ZFXTEXTURE ʵ����������
		for (UINT i = 0; i < m_nNumTextures; i++)
		{
			if (m_pTextures[i].pData)
			{
				// ��������
				//((LPDIRECT3DTEXTURE9)(m_pTextures[i].pData))->Release();
				//m_pTextures[i].pData = NULL;
			}
			if (m_pTextures[i].pClrKeys)
			{
				// ��ɫ��ֵ
				delete[] m_pTextures[i].pClrKeys;
				m_pTextures[i].pClrKeys = NULL;
			}
			if (m_pTextures[i].chName)
			{
				// ��������
				delete[] m_pTextures[i].chName;
				m_pTextures[i].chName = NULL;
			}
		}
		free(m_pTextures);
		m_pTextures = NULL;
	}

	// �ͷŲ��ʶ���
	if (m_pMaterials)
	{
		free(m_pMaterials);
		m_pMaterials = NULL;
	}

	// �ͷ�Ƥ��
	if (m_pSkins)
	{
		free(m_pSkins);
		m_pSkins = NULL;
	}
	m_nNumMaterials = 0;
	m_nNumTextures = 0;
	m_nNumSkins = 0;
	Log("Skin Manager reset");
}

HRESULT ZFXOpenGLSkinManager::AddSkin(const ZFXCOLOR *pcAmbient, const ZFXCOLOR *pcDiffuse, const ZFXCOLOR *pcEmissive, const ZFXCOLOR *pcSpecular, float fSpecPower, UINT *nSkinID)
{
	UINT    nMat, n;
	bool    bMat = false;

	// allocate 50 new memory slots for skins if necessary
	// Skin���ڴ��������Ϊ 50
	if ((m_nNumSkins % 50) == 0)
	{
		n = (m_nNumSkins + 50)*sizeof(ZFXSKIN);
		m_pSkins = (ZFXSKIN*)realloc(m_pSkins, n);
		if (!m_pSkins) return ZFX_OUTOFMEMORY;
	}

	ZFXMATERIAL mat;
	mat.cAmbient = *pcAmbient;
	mat.cDiffuse = *pcDiffuse;
	mat.cEmissive = *pcEmissive;
	mat.cSpecular = *pcSpecular;
	mat.fPower = fSpecPower;

	// do we already have an equal material
	// �ж��Ƿ��Ѿ�����������ͬ�Ĳ���
	for (nMat = 0; nMat < m_nNumMaterials; nMat++)
	{
		if (MaterialEqual(&mat, &m_pMaterials[nMat]))
		{
			bMat = true;
			break;
		}
	} // for [MATERIALS]

	// if exists store its ID otherwise create it new
	if (bMat) m_pSkins[m_nNumSkins].nMaterial = nMat;
	else
	{
		// ���������ӵ������б�
		m_pSkins[m_nNumSkins].nMaterial = m_nNumMaterials;

		// allocate 50 new memory slots for materials if necessary
		if ((m_nNumMaterials % 50) == 0)
		{
			n = (m_nNumMaterials + 50)*sizeof(ZFXMATERIAL);
			m_pMaterials = (ZFXMATERIAL*)realloc(m_pMaterials, n);
			if (!m_pMaterials) return ZFX_OUTOFMEMORY;
		}
		memcpy(&m_pMaterials[m_nNumMaterials], &mat, sizeof(ZFXMATERIAL));
		m_nNumMaterials++;
	}

	// ����Skin�������б���ʼ��
	m_pSkins[m_nNumSkins].bAlpha = false;
	for (int i = 0; i < 8; i++) m_pSkins[m_nNumSkins].nTexture[i] = MAX_ID;

	// save ID and add to count
	(*nSkinID) = m_nNumSkins;
	m_nNumSkins++;

	return ZFX_OK;
}

HRESULT ZFXOpenGLSkinManager::AddTexture(UINT nSkinID, const char *chName, bool bAlpha, float fAlpha, ZFXCOLOR *cColorKeys, DWORD dwNumColorKeys)
{
	ZFXTEXTURE *pZFXTex = NULL;
	HRESULT     hr;
	UINT        nTex, n;
	bool        bTex = false;

	// is skin ID valid at all
	if (nSkinID >= m_nNumSkins) return ZFX_INVALIDID;

	// all 8 stages for this skin already set?
	if (m_pSkins[nSkinID].nTexture[7] != MAX_ID)
	{
		Log("error: AddTexture() failed, all 8 stages set");
		return ZFX_BUFFERSIZE;
	}

	// do we already have this texture
	// �鿴�����Ƿ����
	for (nTex = 0; nTex < m_nNumTextures; nTex++)
	{
		if (strcmp(chName, m_pTextures[nTex].chName) == 0)
		{
			bTex = true;
			break;
		}
	} // for [TEXTURES]

	// load new texture if not yet done
	if (!bTex)
	{
		// allocate 50 new memory slots for textures if necessary
		if ((m_nNumTextures % 50) == 0)
		{
			n = (m_nNumTextures + 50)*sizeof(ZFXTEXTURE);
			m_pTextures = (ZFXTEXTURE*)realloc(m_pTextures, n);
			if (!m_pTextures) {
				Log("error: AddTexture() failed, realloc()");
				return ZFX_OUTOFMEMORY;
			}
		}

		// we use alphablending at least from now on
		if (bAlpha) m_pSkins[nSkinID].bAlpha = true;
		else m_pTextures[m_nNumTextures].fAlpha = 1.0f;

		m_pTextures[m_nNumTextures].pClrKeys = NULL;

		// save texture name
		m_pTextures[m_nNumTextures].chName = new char[strlen(chName) + 1];
		memcpy(m_pTextures[m_nNumTextures].chName, chName, strlen(chName) + 1);

		// create d3d texture from that pointer
		// ������������
		hr = CreateTexture(&m_pTextures[m_nNumTextures], bAlpha);
		if (FAILED(hr)) {
			Log("error: CreateTexture() failed");
			return hr;
		}

		


		// save ID and add to count
		nTex = m_nNumTextures;
		m_nNumTextures++;
	}

	// put texture ID to skin ID
	for (int i = 0; i < 8; i++) {
		if (m_pSkins[nSkinID].nTexture[i] == MAX_ID) {
			m_pSkins[nSkinID].nTexture[i] = nTex;
			break;
		}
	}
	return ZFX_OK;
}

HRESULT ZFXOpenGLSkinManager::AddTextureHeightmapAsBump(UINT nSkinID, const char *chName)
{
	ZFXTEXTURE *pZFXTex = NULL;
	HRESULT     hr;
	UINT        nTex, n;
	bool        bTex = false;

	// is skin ID valid at all
	if (nSkinID >= m_nNumSkins) return ZFX_INVALIDID;

	// all 8 stages for this skin already set?
	if (m_pSkins[nSkinID].nTexture[7] != MAX_ID) {
		Log("error: AddTexture() failed, all 8 stages set");
		return ZFX_BUFFERSIZE;
	}

	// do we already have this texture
	for (nTex = 0; nTex < m_nNumTextures; nTex++) {
		if (strcmp(chName, m_pTextures[nTex].chName) == 0) {
			bTex = true;
			break;
		}
	} // for [TEXTURES]

	// load new texture if not yet done
	if (!bTex)
	{
		// allocate 50 new memory slots for textures if necessary
		if ((m_nNumTextures % 50) == 0) {
			n = (m_nNumTextures + 50)*sizeof(ZFXTEXTURE);
			m_pTextures = (ZFXTEXTURE*)realloc(m_pTextures, n);
			if (!m_pTextures) {
				Log("error: AddTexture() failed, realloc()");
				return ZFX_OUTOFMEMORY;
			}
		}

		// no alpha blending needed
		m_pTextures[m_nNumTextures].fAlpha = 1.0f;
		m_pTextures[m_nNumTextures].pClrKeys = NULL;

		// save texture name
		m_pTextures[m_nNumTextures].chName = new char[strlen(chName) + 1];
		memcpy(m_pTextures[m_nNumTextures].chName, chName, strlen(chName) + 1);

		// create d3d texture from that pointer
		hr = CreateTexture(&m_pTextures[m_nNumTextures], true);
		if (FAILED(hr)) {
			Log("error: CreateTexture() failed");
			return hr;
		}

		// build normals from heightvalues
		hr = ConvertToNormalmap(&m_pTextures[m_nNumTextures]);
		if (FAILED(hr)) {
			Log("error: ConvertToNormalmap() failed");
			return hr;
		}

		// save ID and add to count
		nTex = m_nNumTextures;
		m_nNumTextures++;
	} // load texture

	// put texture ID to skin ID
	for (int i = 0; i < 8; i++) {
		if (m_pSkins[nSkinID].nTexture[i] == MAX_ID) {
			m_pSkins[nSkinID].nTexture[i] = nTex;
			break;
		}
	}

	return ZFX_OK;
}

HRESULT ZFXOpenGLSkinManager::ExchangeTexture(UINT nSkinID, UINT nTexStage, const char *chName, bool bAlpha, float fAlpha, ZFXCOLOR *cColorKeys, DWORD dwNumColorKeys)
{
	ZFXTEXTURE *pZFXTex = NULL;
	HRESULT     hr;
	UINT        nTex, n;
	bool        bTex = false;

	// is skin ID valid at all
	if (nSkinID >= m_nNumSkins) return ZFX_INVALIDID;

	// we can only take 7 textures for one skin
	if (nTexStage > 7) return ZFX_BUFFERSIZE;

	// clear out texture
	if (!chName) {
		m_pSkins[nSkinID].nTexture[nTexStage] = MAX_ID;
		return ZFX_OK;
	}

	// do we already have this texture
	for (nTex = 0; nTex < m_nNumTextures; nTex++) {
		if (strcmp(chName, m_pTextures[nTex].chName) == 0) {
			bTex = true;
			break;
		}
	} // for [TEXTURES]

	// load new texture if not yet done
	if (!bTex) 
	{
		// allocate 50 new memory slots for textures if necessary
		if ((m_nNumTextures % 50) == 0) {
			n = (m_nNumTextures + 50)*sizeof(ZFXTEXTURE);
			m_pTextures = (ZFXTEXTURE*)realloc(m_pTextures, n);
			if (!m_pTextures) return ZFX_OUTOFMEMORY;
		}

		// we use alphablending at least from now on
		if (bAlpha) m_pSkins[nSkinID].bAlpha = true;
		else m_pTextures[m_nNumTextures].fAlpha = 1.0f;

		m_pTextures[m_nNumTextures].pClrKeys = NULL;

		// save texture name
		m_pTextures[m_nNumTextures].chName = new char[strlen(chName) + 1];
		memcpy(m_pTextures[m_nNumTextures].chName, chName, strlen(chName) + 1);

		// create d3d texture from that pointer
		hr = CreateTexture(&m_pTextures[m_nNumTextures], bAlpha);
		if (FAILED(hr)) {
			Log("error: CreateTexture() failed");
			return hr;
		}

		// add alpha values if needed
		if (bAlpha) {

			pZFXTex = &m_pTextures[m_nNumTextures];

			// remind information
			pZFXTex->dwNum = dwNumColorKeys;
			pZFXTex->pClrKeys = new ZFXCOLOR[dwNumColorKeys];
			memcpy(pZFXTex->pClrKeys, cColorKeys,
				sizeof(ZFXCOLOR)*pZFXTex->dwNum);

			GLuint texture = (*(GLuint*)pZFXTex->pData);

			// set alpha keys first
			for (DWORD dw = 0; dw < dwNumColorKeys; dw++) {
				hr = SetAlphaKey(texture,
					UCHAR(cColorKeys[dw].fR * 255),
					UCHAR(cColorKeys[dw].fG * 255),
					UCHAR(cColorKeys[dw].fB * 255),
					UCHAR(cColorKeys[dw].fA * 255));
				if (FAILED(hr)) {
					Log("error: SetAlphaKey() failed");
					return hr;
				}
			}

			if (fAlpha < 1.0f) {
				// remind that value for info purpose
				pZFXTex->fAlpha = fAlpha;

				// now generell transparency
				// ����ȫ��alpha
				hr = SetTransparency(texture, UCHAR(fAlpha * 255));
				if (FAILED(hr)) {
					Log("error: SetTransparency() failed");
					return hr;
				}
			}
		}
		// save ID and add to count
		nTex = m_nNumTextures;
		m_nNumTextures++;
	}

	// put texture ID to skin ID
	m_pSkins[nSkinID].nTexture[nTexStage] = nTex;
	return ZFX_OK;
}

HRESULT ZFXOpenGLSkinManager::ExchangeMaterial(UINT nSkinID, const ZFXCOLOR *pcAmb, const ZFXCOLOR *pcDif, const ZFXCOLOR *pcEmis, const ZFXCOLOR *pcSpec, float fSpecPower)
{
	UINT nMat = 0, n = 0;
	bool bMat = false;

	// is skin ID valid at all
	if (nSkinID >= m_nNumSkins) return ZFX_INVALIDID;

	ZFXMATERIAL mat;
	mat.cAmbient = *pcAmb;
	mat.cDiffuse = *pcDif;
	mat.cEmissive = *pcEmis;
	mat.cSpecular = *pcSpec;
	mat.fPower = fSpecPower;

	// do we already have an equal material
	for (nMat = 0; nMat < m_nNumMaterials; nMat++) {
		if (MaterialEqual(&mat, &m_pMaterials[nMat])) {
			bMat = true;
			break;
		}
	} // for [MATERIALS]

	// if exists store its ID otherwise create it new
	if (bMat) m_pSkins[nSkinID].nMaterial = nMat;

	// else make a new material accordingly
	else {
		m_pSkins[nSkinID].nMaterial = m_nNumMaterials;

		// allocate 50 new memory slots for materials if necessary
		if ((m_nNumMaterials % 50) == 0) {
			n = (m_nNumMaterials + 50)*sizeof(ZFXMATERIAL);
			m_pMaterials = (ZFXMATERIAL*)realloc(m_pMaterials, n);
			if (!m_pMaterials) return ZFX_OUTOFMEMORY;
		}
		memcpy(&m_pMaterials[m_nNumMaterials], &mat, sizeof(ZFXMATERIAL));
		m_nNumMaterials++;
	}

	return ZFX_OK;
}

bool ZFXOpenGLSkinManager::MaterialEqual(const ZFXMATERIAL *pMat0, const ZFXMATERIAL *pMat1)
{
	return (*pMat0) == (*pMat1);
}

bool ZFXOpenGLSkinManager::ColorEqual(const ZFXCOLOR *pCol0, const ZFXCOLOR *pCol1)
{
	return (*pCol0) == (*pCol1);
}

void ZFXOpenGLSkinManager::LogCurrentStatus(char *chLog, bool bDetailed)
{
	Log("\n\nSKINMANAGER_SITREP: skins: %d, mats: %d, texrs: %d \n",
		m_nNumSkins, m_nNumMaterials, m_nNumTextures);

	if (!bDetailed) return;

	Log("SKINS { \n");
	for (UINT i = 0; i < m_nNumSkins; i++) {
		Log("   ID: %d { matrl: %d, ", i, m_pSkins[i].nMaterial);
		Log("textr: [%d,%d,%d,%d,%d,%d,%d,%d] }\n",
			m_pSkins[i].nTexture[0], m_pSkins[i].nTexture[1],
			m_pSkins[i].nTexture[2], m_pSkins[i].nTexture[3],
			m_pSkins[i].nTexture[4], m_pSkins[i].nTexture[5],
			m_pSkins[i].nTexture[6], m_pSkins[i].nTexture[7]);
	}
	Log("   } \n");

	Log("TEXTURES { \n");
	for (UINT j = 0; j < m_nNumTextures; j++) {
		Log("   ID: %d {\"%s\"} \n", j, m_pTextures[j].chName);
	}
	Log("   } \n\n");

	Log("MATERIALS { \n");
	for (UINT k = 0; k < m_nNumMaterials; k++) {
		Log("   ID: %d\n", k);
		Log("   Diffuse (%f,%f,%f,%f)\n", m_pMaterials[k].cDiffuse.c[0],
			m_pMaterials[k].cDiffuse.c[1], m_pMaterials[k].cDiffuse.c[2],
			m_pMaterials[k].cDiffuse.c[3]);
		Log("   Ambient (%f,%f,%f,%f)\n", m_pMaterials[k].cAmbient.c[0],
			m_pMaterials[k].cAmbient.c[1], m_pMaterials[k].cAmbient.c[2],
			m_pMaterials[k].cAmbient.c[3]);
		Log("   Emissive (%f,%f,%f,%f)\n", m_pMaterials[k].cEmissive.c[0],
			m_pMaterials[k].cEmissive.c[1], m_pMaterials[k].cEmissive.c[2],
			m_pMaterials[k].cEmissive.c[3]);
		Log("   Specular (%f,%f,%f,%f) power: %f\n", m_pMaterials[k].cSpecular.c[0],
			m_pMaterials[k].cSpecular.c[1], m_pMaterials[k].cSpecular.c[2],
			m_pMaterials[k].cSpecular.c[3], m_pMaterials[k].fPower);
	}
	Log("   } \n\n");
}

UINT ZFXOpenGLSkinManager::GetNumSkins(void)
{
	return m_nNumSkins;
}

ZFXSKIN ZFXOpenGLSkinManager::GetSkin(UINT nSkinID)
{
	if (nSkinID < m_nNumSkins) return m_pSkins[nSkinID];
	else
	{
		ZFXSKIN EmptySkin;
		return EmptySkin;
	}
}

ZFXMATERIAL ZFXOpenGLSkinManager::GetMaterial(UINT nMatID)
{
	if (nMatID < m_nNumMaterials)
		return m_pMaterials[nMatID];
	else
	{
		ZFXMATERIAL EmptyMaterial;
		return EmptyMaterial;
	}
}

const char* ZFXOpenGLSkinManager::GetTextureName(UINT nTexID, float *fAlpha, ZFXCOLOR *AlphaKeys, UCHAR *NumKeys)
{
	if (nTexID >= m_nNumTextures) return NULL;

	if (fAlpha) *fAlpha = m_pTextures[nTexID].fAlpha;

	if (NumKeys) *NumKeys = m_pTextures[nTexID].dwNum;

	if (m_pTextures[nTexID].pClrKeys && AlphaKeys)
	{
		memcpy(AlphaKeys, m_pTextures[nTexID].pClrKeys,
			sizeof(ZFXCOLOR) * m_pTextures[nTexID].dwNum);
	}

	return m_pTextures[nTexID].chName;
}

void ZFXOpenGLSkinManager::Log(char *fmt, ...)
{
	char ch[1024];

	va_list args;
	va_start(args, fmt);
	vsprintf_s(ch, fmt, args);
	va_end(args);

	GetLogger().Print(ch);
}

HRESULT ZFXOpenGLSkinManager::CreateTexture(ZFXTEXTURE *pTexture, bool bAlpha)
{
	if (pTexture == NULL)
		return E_INVALIDARG;

	GLenum fmt;
	DIBSECTION dibs;
	HRESULT hr;
	
	// �̶�BMP
	HBITMAP hBMP = (HBITMAP)LoadImage(NULL, pTexture->chName,
		IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);

	if (!hBMP)
	{
		Log("Cannot open texture [%s]", pTexture->chName);
		return ZFX_FILENOTFOUND;
	}

	GetObject(hBMP, sizeof(DIBSECTION), &dibs);

	
	if (dibs.dsBmih.biBitCount != 24)
	{
		DeleteObject(hBMP);
		Log("Texture is not 24 bit [%s]", pTexture->chName);
		return ZFX_INVALIDFILE;
	}

	if (bAlpha)  fmt = GL_RGB;
	else fmt = GL_RGB10;

	long lWidth = dibs.dsBmih.biWidth;
	long lHeight = dibs.dsBmih.biHeight;
	BYTE *pBMPBits = (BYTE*)dibs.dsBm.bmBits;

	pTexture->pData = (void *) new GLuint;

	// ����һ����������
	GLuint texture = 0;
	glGenTextures(1, &texture);
	// �ж��Ƿ񴴽�ʧ��

	// ��һ����������
	glBindTexture(GL_TEXTURE_2D, texture);

	// Ϊ��ǰ�󶨵����������������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lWidth, lHeight, 0, fmt, GL_UNSIGNED_BYTE, pBMPBits);

	(*(GLuint*)pTexture->pData) = texture;
	DeleteObject(hBMP);
	return ZFX_OK;
}

// encode vector data as RGBA color value for normal map
DWORD VectortoRGBA(ZFXVector *vc, float fHeight) {
	DWORD r = (DWORD)(127.0f * vc->x + 128.0f);
	DWORD g = (DWORD)(127.0f * vc->y + 128.0f);
	DWORD b = (DWORD)(127.0f * vc->z + 128.0f);
	DWORD a = (DWORD)(255.0f * fHeight);
	return((a << 24L) + (r << 16L) + (g << 8L) + (b << 0L));
} // VectortoRGBA

HRESULT ZFXOpenGLSkinManager::ConvertToNormalmap(ZFXTEXTURE *pTexture)
{
	if (pTexture == NULL)
		return E_INVALIDARG;
	HRESULT hr = ZFX_OK;

	GLuint texture = *((GLuint*)pTexture->pData);
	if (!glIsTexture(texture))
		return E_INVALIDARG;

	glBindTexture(GL_TEXTURE_2D, texture);

	GLint width = 0;
	GLint height = 0;
	GLint internalFmt = 0;

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFmt);

	if (internalFmt != GL_RGBA)
		return E_FAIL;

	DWORD *pData = new DWORD[width * height];
	if (pData == NULL)
		return E_OUTOFMEMORY;

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT, pData);

	DWORD pitch = width * sizeof(DWORD);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			DWORD color00 = pData[0];
			DWORD color10 = pData[1];
			DWORD color01 = pData[pitch / sizeof(DWORD)]; // �±����

			float fHeight00 = (float)((color00 & 0x00ff0000) >> 16) / 255.0f;
			float fHeight10 = (float)((color10 & 0x00ff0000) >> 16) / 255.0f;
			float fHeight01 = (float)((color01 & 0x00ff0000) >> 16) / 255.0f;

			ZFXVector vcPoint00(x + 0.0f, y + 0.0f, fHeight00);
			ZFXVector vcPoint10(x + 1.0f, y + 0.0f, fHeight10);
			ZFXVector vcPoint01(x + 0.0f, y + 1.0f, fHeight01);
			ZFXVector vc10 = vcPoint10 - vcPoint00;
			ZFXVector vc01 = vcPoint01 - vcPoint00;

			ZFXVector vcNormal;
			vcNormal.Cross(vc10, vc01);
			vcNormal.Normalize();

			// store normal as RGBA in normalmap
			*pData++ = VectortoRGBA(&vcNormal, fHeight00);
		}
	}
	return ZFX_OK;
}

HRESULT ZFXOpenGLSkinManager::SetAlphaKey(GLuint texture, UCHAR R, UCHAR G, UCHAR B, UCHAR A)
{
	if (!glIsTexture(texture))
		return E_INVALIDARG;

	GLint width = 0;
	GLint height = 0;
	GLint internalFmt = 0;

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFmt);

	if (internalFmt != GL_RGBA)
		return E_FAIL;

	DWORD *pData = new DWORD[width * height];
	if (pData == NULL)
		return E_OUTOFMEMORY;

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT, pData);
	
	DWORD dwKey, dwColor;
	dwKey = COLORDWORD(R, G, B, A);

	if (A > 0) dwColor = COLORDWORD(R, G, B, A);
	else dwColor = COLORDWORD(0, 0, 0, A);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (pData[width * y + x] == dwKey)
			{
				pData[width * y + x] = dwColor;
			}
		}
	}

	return ZFX_OK;
}

HRESULT ZFXOpenGLSkinManager::SetTransparency(GLuint texture, UCHAR alpha)
{
	DWORD dwColor;
	UCHAR A, R, G, B;

	if (!glIsTexture(texture))
		return E_INVALIDARG;

	GLint width = 0;
	GLint height = 0;
	GLint internalFmt = 0;

	glBindTexture(GL_TEXTURE_2D, texture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFmt);

	if (internalFmt != GL_RGBA)
		return E_FAIL;

	DWORD *pData = new DWORD[width * height];
	if (pData == NULL)
		return E_OUTOFMEMORY;

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_INT, pData);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			dwColor = pData[width * y + x];

			A = (UCHAR)((dwColor & 0xff000000) >> 24);
			R = (UCHAR)((dwColor & 0x00ff0000) >> 16);
			G = (UCHAR)((dwColor & 0x0000ff00) >> 8);
			B = (UCHAR)((dwColor & 0x000000ff));

			if (A >= alpha)
				A = alpha;

			pData[width * y + x] = COLORDWORD(R, G, B, A);
		}
	}

	return ZFX_OK;
}