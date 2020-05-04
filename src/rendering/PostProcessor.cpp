#include "../Module/ModuleManager.h"
#include "../MaterialLibrary/MaterialLibraryModule.h"
#include "PostProcessor.h"

Exp::PostProcessor::PostProcessor(RenderingModule* renderingModule)
{
	renderer = renderingModule;
	if (MaterialLibraryModule* materialLibrary = ModuleManager::Get().GetModule<MaterialLibraryModule>("MaterialLibrary"))
	{
		postProcessShader = materialLibrary->GetShader("postprocess");
	}
}

Exp::PostProcessor::~PostProcessor()
{

}

void Exp::PostProcessor::UpdateRenderSize(unsigned int width, unsigned int height)
{
}

void Exp::PostProcessor::ProcessPreLighting(RenderTarget* gBuffer, Camera* camera)
{
}

void Exp::PostProcessor::ProcessPostLighting(RenderTarget* gBuffer, RenderTarget* output, Camera* camera)
{
}

void Exp::PostProcessor::Blit(Texture* source)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, renderer->renderSize.x, renderer->renderSize.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	source->Bind(0);

	// set settings 
	postProcessShader->Use();
	postProcessShader->SetBool("SSAO", SSAO);
	postProcessShader->SetBool("Sepia", sepia);
	postProcessShader->SetBool("Vignette", vignette);
	postProcessShader->SetBool("Bloom", bloom);
	// exposure
	postProcessShader->SetFloat("Exposure", exposure);

	// gamma
	postProcessShader->SetFloat("Gamma", gamma);

	// motion blur
	/*postProcessShader->SetBool("MotionBlur", motionBlur);
	postProcessShader->SetFloat("MotionScale", ImGui::GetIO().Framerate / FPSTarget * 0.8);
	postProcessShader->SetInt("MotionSamples", 16);*/

	renderer->RenderMesh(renderer->ndcPlane.get());
}

void Exp::PostProcessor::DisplayDebug()
{
	if (ImGui::CollapsingHeader("PostProcess"))
	{
		//ImGui::Checkbox("Bloom", &bloom);
		//ImGui::Checkbox("TXAA", &TXAA);
		//ImGui::Checkbox("Motionblur", &motionBlur);
		ImGui::Checkbox("Vignette", &vignette);
		ImGui::Checkbox("SSAO", &SSAO);
		ImGui::Checkbox("Sepia", &sepia);
		ImGui::Separator();
		ImGui::SliderFloat("Exposure", &exposure, 0.1f, 10.0f);
		ImGui::SliderFloat("Gamma", &gamma, 0.1f, 10.0f);
	}
}
