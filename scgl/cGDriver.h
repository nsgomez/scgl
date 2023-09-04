#pragma once
#include <cstdio>
#include <string>
#include <vector>
#include <cRZRefCount.h>
#include "cIGZGDriver.h"
#include "sGDMode.h"
#include "ext/cIGZGBufferRegionExtension.h"
#include "ext/cIGZGDriverLightingExtension.h"
#include "ext/cIGZGDriverVertexBufferExtension.h"
#include "ext/cIGZGSnapshotExtension.h"

extern FILE* gLogFile;

#ifdef NDEBUG
#define NOTIMPL()
#define SIZE_CHECK(...)
#define SIZE_CHECK_RETVAL(...)
#else
#define NOTIMPL() { if (gLogFile == nullptr) gLogFile = fopen("cGDriver.notimpl.log", "w"); fprintf(gLogFile, "%s\n", __FUNCSIG__); fflush(gLogFile); }
#define UNEXPECTED NOTIMPL
#define SIZE_CHECK(param, map) if (param >= sizeof(map) / sizeof(map[0])) { UNEXPECTED(); return; }
#define SIZE_CHECK_RETVAL(param, map, ret) if (param >= sizeof(map) / sizeof(map[0])) { UNEXPECTED(); return ret; }
#endif

namespace nSCGL
{
	constexpr size_t MAX_BUFFER_REGIONS = sizeof(uint8_t) * 8U;
	constexpr size_t MAX_TEXTURE_UNITS = 8;

	class cGDriver final :
		public cIGZGDriver,
		public cIGZGBufferRegionExtension,
		public cIGZGDriverLightingExtension,
		public cIGZGDriverVertexBufferExtension,
		public cIGZGSnapshotExtension,
		public cRZRefCount
	{
	private:
		enum class DriverError
		{
			OK = 0,
			OUT_OF_RANGE = 2,
			NOT_SUPPORTED = 3,
			CREATE_CONTEXT_FAIL = 6,
			INVALID_ENUM = 0x500,
			INVALID_VALUE = 0x501,

			FORCE_DWORD = 0x7FFFFFFF
		};

		enum SGLMatrixMode
		{
			MODEL_VIEW = 0,
			PROJECTION,
			TEXTURE,
			COLOR,

			NUM_MATRIX_MODES = COLOR + 1,
		};

		struct TextureStageData
		{
			uint32_t coordSrc;
			void const* textureHandle;
			bool toBeEnabled;
			bool currentlyEnabled;
		};

	private:
		unsigned int refCount;
		DriverError lastError;

#ifndef NDEBUG
		uint32_t dbgLastError;
#endif

		std::vector<sGDMode> videoModes;
		int videoModeCount;
		int currentVideoMode;
		std::string driverInfo;

		int windowWidth, windowHeight;
		int viewportX, viewportY, viewportWidth, viewportHeight;

		uint32_t activeMatrixMode;
		uint32_t activeTextureStage; // 0x18
		uint32_t maxTextureUnits; // 0x28
		// We're still using the GL fixed function pipeline,
		// so we shouldn't have a high number of texture units.
		TextureStageData textureStageData[MAX_TEXTURE_UNITS]; // 0x2c?

		bool normalArrayEnabled, colorArrayEnabled;          // 0x81, 0x82
		bool ambientMaterialEnabled, diffuseMaterialEnabled; // 0xa8, 0xa9
		float colorMultiplierR, colorMultiplierG, colorMultiplierB, colorMultiplierA; // 0x98, 0x9c, 0xa0, 0xa4

		uint32_t interleavedFormat;     // 0x84
		int32_t interleavedStride;      // 0x88
		void const* interleavedPointer; // 0x8c

		// We're not expecting to use a lot of buffer regions simultaneously, so we'll use an
		// 8-bit mask to indicate which regions are allocated and free.
		uint8_t bufferRegionFlags;
		void* bufferRegionHandles[MAX_BUFFER_REGIONS];

	private:
		struct {
			// OpenGL
			bool bgraColor;
			bool stencilBuffer;
			bool multitexture;
			bool textureEnvCombine;
			bool fogCoord;
			bool textureCompression;
			bool nvTextureEnvCombine4;
			bool debugOutput;
			bool noError;

			// WGL
			bool bufferRegion;
			bool createContext;
			bool createContextNoError;
			bool createContextProfile;
			bool multisample;
			bool pixelFormat;
			bool swapControl;
		} supportedExtensions;

	private:
		void* windowHandle;
		void* deviceContext;
		void* glContext;

	private:
		void SetLastError(DriverError err);
		void SetLightingParameters();
		void ApplyTextureStages();
		void DestroyOpenGLContext();
		int FindFreeBufferRegionIndex(void);
		int InitializeVideoModeVector(void);

	public:
		cGDriver();
		virtual ~cGDriver() override;

		// We're taking the GZCLSID of the original GL driver and overriding
		// it by presenting a higher version number to the GZCOM.
		static const uint32_t kSCGLGDriverGZCLSID = 0xc4554841;

		static bool FactoryFunctionPtr2(uint32_t riid, void** ppvObj) {
			cGDriver* pDriver = new cGDriver();
			bool bSucceeded = pDriver->QueryInterface(riid, ppvObj);

			if (!bSucceeded || *ppvObj == NULL) {
				bSucceeded = false;

				delete pDriver;
				pDriver = NULL;
			}

			return bSucceeded;
		}

	public:
		virtual bool QueryInterface(uint32_t riid, void** ppvObj) override;
		virtual uint32_t AddRef(void) override;
		virtual uint32_t Release(void) override;
		virtual bool FinalRelease(void) override;

	public:
		virtual void DrawArrays(uint32_t gdPrimType, int32_t, int32_t) override;
		virtual void DrawElements(uint32_t gdPrimType, int32_t count, uint32_t gdType, void const* indices) override;
		virtual void InterleavedArrays(uint32_t gdVertexFormat, int32_t, void const*) override;

		virtual uint32_t MakeVertexFormat(uint32_t, intptr_t gdElementTypePtr) override;
		virtual uint32_t MakeVertexFormat(uint32_t gdVertexFormat) override;
		virtual uint32_t VertexFormatStride(uint32_t gdVertexFormat) override;
		virtual uint32_t VertexFormatElementOffset(uint32_t gdVertexFormat, uint32_t gdElementType, uint32_t) override;
		virtual uint32_t VertexFormatNumElements(uint32_t gdVertexFormat, uint32_t gdElementType) override;

		virtual void Clear(uint32_t) override;
		virtual void ClearColor(float, float, float, float) override;
		virtual void ClearDepth(double) override;
		virtual void ClearStencil(int32_t) override;

		virtual void ColorMask(bool) override;
		virtual void DepthFunc(uint32_t gdTestFunc) override;
		virtual void DepthMask(bool) override;

		virtual void StencilFunc(uint32_t gdTestFunc, int32_t, uint32_t) override;
		virtual void StencilMask(uint32_t) override;
		virtual void StencilOp(uint32_t gdStencilOp, uint32_t gdStencilOp2, uint32_t gdStencilOp3) override;

		virtual void BlendFunc(uint32_t gdBlendFunc, uint32_t gdBlend) override;
		virtual void AlphaFunc(uint32_t gdTestFunc, float) override;
		virtual void ShadeModel(uint32_t gdShade) override;

		virtual void BindTexture(uint32_t gdTextureTarget, uint32_t) override;
		virtual void TexImage2D(uint32_t gdTextureTarget, int32_t, int32_t gdInternalTexFormat, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*) override;
		virtual void PixelStore(uint32_t gdParameter, int32_t) override;

		virtual void TexEnv(uint32_t gdTextureEnvTarget, uint32_t gdTextureEnvParamType, int32_t gdTextureEnvModeParam) override;
		virtual void TexEnv(uint32_t gdTextureEnvTarget, uint32_t gdTextureEnvParamType, float const*) override;
		virtual void TexParameter(uint32_t gdTextureTarget, uint32_t gdTextureParamType, int32_t gdTextureParam) override;

		virtual void Fog(uint32_t gdFogParamType, uint32_t gdFogParam) override;
		virtual void Fog(uint32_t gdFogParamType, float const*) override;

		virtual void ColorMultiplier(float r, float g, float b) override;
		virtual void AlphaMultiplier(float a) override;
		virtual void EnableVertexColors(bool, bool) override;

		virtual void GenTextures(int32_t, uint32_t*) override;
		virtual void DeleteTextures(int32_t, uint32_t const*) override;
		virtual bool IsTexture(uint32_t) override;
		virtual void PrioritizeTextures(int32_t, uint32_t const*, float const*) override;
		virtual bool AreTexturesResident(int32_t, uint32_t const*, bool*) override;

		virtual void MatrixMode(uint32_t gdMatrixTarget) override;
		virtual void LoadMatrix(float const*) override;
		virtual void LoadIdentity(void) override;

		virtual void Flush(void) override;
		virtual void Enable(uint32_t gdDriverState) override;
		virtual void Disable(uint32_t gdDriverState) override;
		virtual bool IsEnabled(uint32_t gdDriverState) override;

		virtual void GetBoolean(uint32_t gdParameter, bool*) override;
		virtual void GetInteger(uint32_t gdParameter, int32_t*) override;
		virtual void GetFloat(uint32_t gdParameter, float*) override;
		virtual uint32_t GetError(void) override;

		virtual void TexStage(uint32_t) override;
		virtual void TexStageCoord(uint32_t gdTexCoordSource) override;
		virtual void TexStageMatrix(float const*, uint32_t, uint32_t, uint32_t gdTexMatFlags) override;
		virtual void TexStageCombine(eGDTextureStageCombineScaleParamType gdParamType, eGDTextureStageCombineScaleParam gdParam) override;
		virtual void TexStageCombine(eGDTextureStageCombineOperandType gdParamType, eGDBlend gdBlend) override;
		virtual void TexStageCombine(eGDTextureStageCombineSourceParamType gdParamType, eGDTextureStageCombineSourceParam gdParam) override;
		virtual void TexStageCombine(eGDTextureStageCombineParamType gdParamType, eGDTextureStageCombineModeParam gdParam) override;

		virtual void SetTexture(uint32_t, uint32_t) override;
		virtual intptr_t GetTexture(uint32_t) override;
		virtual intptr_t CreateTexture(uint32_t gdInternalTexFormat, uint32_t, uint32_t, uint32_t, uint32_t gdTexHintFlags) override;
		virtual void LoadTextureLevel(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, uint32_t, void const*) override;
		virtual void SetCombiner(cGDCombiner const& combiner, uint32_t) override;

		virtual uint32_t CountVideoModes(void) const override;
		virtual void GetVideoModeInfo(uint32_t dwIndex, sGDMode& gdMode) override;
		virtual void GetVideoModeInfo(sGDMode& gdMode) override;
		virtual void SetVideoMode(int32_t newModeIndex, void*, bool, bool) override;

		virtual void PolygonOffset(int32_t) override;

		virtual void BitBlt(int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*) override;
		virtual void StretchBlt(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*) override;
		virtual void BitBltAlpha(int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) override;
		virtual void StretchBltAlpha(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) override;
		virtual void BitBltAlphaModulate(int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) override;
		virtual void StretchBltAlphaModulate(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, uint32_t gdTexFormat, uint32_t gdType, void const*, bool, void const*, uint32_t) override;

		virtual void SetViewport(void) override;
		virtual void SetViewport(int32_t x, int32_t y, int32_t width, int32_t height) override;
		virtual void GetViewport(int32_t dimensions[4]) override;

		virtual char const* GetDriverInfo(void) const override;
		virtual uint32_t GetGZCLSID(void) const override;

		virtual bool Init(void) override;
		virtual bool Shutdown(void) override;
		virtual bool IsDeviceReady(void) override;
		virtual bool Punt(uint32_t, void*) override;

	public:
		virtual char const* GetVertexBufferName(uint32_t gdVertexFormat) override;
		virtual uint32_t VertexBufferType(uint32_t) override;
		virtual uint32_t MaxVertices(uint32_t) override;
		virtual uint32_t GetVertices(int32_t, bool) override;
		virtual uint32_t ContinueVertices(uint32_t, uint32_t) override;
		virtual void ReleaseVertices(uint32_t) override;
		virtual void DrawPrims(uint32_t, uint32_t gdPrimType, void*, uint32_t) override;
		virtual void DrawPrimsIndexed(uint32_t, uint32_t gdPrimType, uint32_t, uint16_t*, void*, uint32_t) override;
		virtual void Reset(void) override;

	public:
		virtual bool BufferRegionEnabled(void) override;
		virtual uint32_t NewBufferRegion(int32_t gdBufferRegionType) override;
		virtual bool DeleteBufferRegion(int32_t bufferRegion) override;
		virtual bool ReadBufferRegion(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) override;
		virtual bool DrawBufferRegion(uint32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) override;
		virtual bool IsBufferRegion(uint32_t bufferRegion) override;
		virtual bool CanDoPartialRegionWrites(void) override;
		virtual bool CanDoOffsetReads(void) override;

		virtual bool DeleteAllBufferRegions(void) override;
		virtual cIGZBuffer* CopyColorBuffer(int32_t, int32_t, int32_t, int32_t, cIGZBuffer*) override;

	public:
		virtual void EnableLighting(bool) override;
		virtual void EnableLight(uint32_t, bool) override;
		virtual void LightModelAmbient(float, float, float, float) override;
		virtual void LightColor(uint32_t, uint32_t, float const*) override;
		virtual void LightColor(uint32_t, float const*, float const*, float const*) override;
		virtual void LightPosition(uint32_t, float const*) override;
		virtual void LightDirection(uint32_t, float const*) override;
		virtual void MaterialColor(uint32_t, float const*) override;
		virtual void MaterialColor(float const*, float const*, float const*, float const*, float) override;
	};
}