#pragma once

#include "Texture.h"
#include "Shader.h"
#include "RenderTarget.h"
#include "RenderingModule.h"
#include "../Camera/Camera.h"

namespace Exp
{

	class PostProcessor
	{
        friend class RenderingModule;
    public:
        // resulting post-processor intermediate outputs
        Texture* SSAOOutput = nullptr;

        // toggles
        bool sepia = false;
        bool vignette = false;
        bool bloom = false;
        bool SSAO = false;
        bool TXAA = false;
        bool motionBlur = false;

        // exposure
        float exposure = 1.0f;

        // gamma
        float gamma = 2.2f;

        // ssao
        int SSAOKernelSize = 32;

        // motion-blur
        float FPSTarget = 60.0f;
    private:
        // global post-process state
        Shader*  postProcessShader = nullptr;
        RenderTarget* RTOutput = nullptr;

        // ssao
        RenderTarget* SSAORenderTarget = nullptr;
        Shader* SSAOShader = nullptr;
        Shader* SSAOBlurShader = nullptr;
        Texture* SSAONoise = nullptr;
    private:
        RenderingModule* renderer;

    public:
        PostProcessor(RenderingModule * rendereringModule);
        ~PostProcessor();

        // updates all render targets to match new render size
        void UpdateRenderSize(unsigned int width, unsigned int height);

        // process stages
        void ProcessPreLighting(RenderTarget* gBuffer, Camera* camera);
        void ProcessPostLighting(RenderTarget* gBuffer, RenderTarget* output, Camera* camera);

        // blit all combined post-processing steps to default framebuffer
        void Blit(Texture* source);

        // Debug
        void DisplayDebug();
    };
}