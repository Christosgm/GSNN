#pragma once

#include <vector>	
#include <string>
#include <iostream>

#include "../utils/Cube.h"
#include "../utils/Quad.h"
#include "../utils/Options.h"
#include "../utils/Shader.h"
#include "../utils/Texture.h"
#include "../utils/FileWriter.h"
#include "../utils/FrameBuffer.h"
#include "../utils/SSBO.h"
#include "SplatData.h"

class PointCloud
{
private:
	unsigned int numberOfInstances;

	GLuint _positionSSBO;
	GLuint _scaleSSBO;
	GLuint _colorSSBO;
	GLuint _quatSSBO;
	GLuint _alphaSSBO;
	GLuint _cov3DSSBO;

	// #ifdef EXTRACT
		//1st Pass
		GLuint depth_avg_image;
		GLuint minimum_depth_image;
		GLuint maximum_depth_image;
		GLuint prod_1_ai;
		GLuint sum_aici;
		GLuint sum_ai;

		//subsequent passes
		GLuint sum_aici_k;
		GLuint sum_ai_k;
		GLuint prod_1_ai_k;
	// #endif

	#ifdef DEPTH_PEELING
		FrameBuffer* fb;
		GLuint previousDepthTexture;
	#endif 

	Cube* cube;
	Quad* quad;

	int WIDTH;
	int HEIGHT;

	ShaderManager* sm;

	std::string imageName;
	int imageIndex;

	void init(SplatData* splatData)
	{	
		_positionSSBO = generatePointsSSBO(splatData->getPositions());
		_scaleSSBO = generatePointsSSBO(splatData->getScales());
		_colorSSBO = generatePointsSSBO(splatData->getColors());
		_quatSSBO = generatePointsSSBO(splatData->getRotations());
		_alphaSSBO = generatePointsSSBO(splatData->getOpacities());
		_cov3DSSBO = generatePointsSSBO(splatData->getCovariances3D());

		cube = new Cube();
		quad = new Quad();

		initFrameBuffers();
		
		#ifdef EXTRACT
			initFeatureTextures();
		#endif
	}
public:
	PointCloud(SplatData* splatData, ShaderManager* sm, int width, int height)
	{
		this->sm = sm;
		numberOfInstances = splatData->getNumberOfGaussians();
		WIDTH = width;
		HEIGHT = height;
		init(splatData);
	}

	~PointCloud()
	{
		deleteBuffers();
		#ifdef EXTRACT
			deleteFeatureTextures();
		#endif
		#ifdef DEPTH_PEELING
			fb->~FrameBuffer();
		#endif
	}

	void deleteBuffers()
	{
		GLuint buffers[6] = {_positionSSBO, _scaleSSBO, _colorSSBO, _quatSSBO, _alphaSSBO, _cov3DSSBO}; 
		glDeleteBuffers(6, buffers);
	}

	#ifdef DEPTH_PEELING
		void initFrameBuffers()
		{
			fb = new FrameBuffer(WIDTH, HEIGHT);
			initTexture(&previousDepthTexture, 0, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 1.0);
		}
	#endif

	#ifdef EXTRACT
		void initFeatureTextures()
		{
			initTexture(&sum_aici, 0, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 0.0);
			initTexture(&sum_ai, 1, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 0.0);
			initTexture(&prod_1_ai, 2, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 1.0);
			initTexture(&depth_avg_image, 3, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 0.0);
			initTexture(&minimum_depth_image, 4, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 1.0);
			initTexture(&maximum_depth_image, 5, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 0.0);

			initTexture(&sum_aici_k, 0, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 0.0);
			initTexture(&sum_ai_k, 1, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 0.0);
			initTexture(&prod_1_ai_k, 2, WIDTH, HEIGHT, GL_TEXTURE_2D, GL_RGBA32F, 1.0);
		}

		void deleteFeatureTextures()
		{
			GLuint textures[9] = {
								  sum_aici, 
								  sum_ai, 
								  prod_1_ai, 
								  depth_avg_image,
								  minimum_depth_image,
								  maximum_depth_image,
								  sum_aici_k,
								  sum_ai_k,
								  prod_1_ai_k 
								 };
			glDeleteTextures(6, textures);
		}
	#endif

	void setImageName(std::string imageName)
	{
		this->imageName = imageName;
	}

	void setIndex(int index)
	{
		this->imageIndex = index;
	}

	void render()
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _positionSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _scaleSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _colorSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _quatSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, _alphaSSBO);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, _cov3DSSBO);

		glBindVertexArray(cube->getVAO());
		glDrawArraysInstanced(GL_TRIANGLES, 0, cube->getVerticesNumber(), numberOfInstances);
		glFinish();
	}

	void depthPeeling(int num_layers)
	{
		//Default framebuffer starting depth: 1.0, Default colorbuffer starting color [0, 0, 0, 1]  
		glClearDepth(1.0);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat* retrievedDepthBuffer; //to get current minimum depth
		fb->bind();
			//clear depth and color buffers 
			fb->clear(1.0, 0.0, 0.0, 0.0, 1.0);
			//NORMAL PASS
			sm->use();
				sm->sendUniforms();
				#ifdef EXTRACT
					bindTexture(sum_aici, 0, GL_RGBA32F);
					bindTexture(sum_ai, 1, GL_RGBA32F);
					bindTexture(prod_1_ai, 2, GL_RGBA32F);
					bindTexture(depth_avg_image, 3, GL_RGBA32F);
					bindTexture(minimum_depth_image, 4, GL_RGBA32F);
					bindTexture(maximum_depth_image, 5, GL_RGBA32F);
				#endif
				render();
			sm->unuse();
			retrievedDepthBuffer = retrieveDepthBuffer(fb->getDepthTex(), WIDTH, HEIGHT);
			//Fill depth image texture with minimum
			bindSamplerTexture(previousDepthTexture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, retrievedDepthBuffer);
		fb->unbind();
		renderToQuad();
		sm->change(DEPTH_PEEL);//change shader for depth peeling;
		
		//SUBSEQUENT PASSES
		for (int i = 0; i < num_layers; i++)
		{
			//clear depth buffer for new pass
			fb->bind();
				fb->clear(1.0, 0.0, 0.0, 0.0, 1.0);
				if (i == num_layers-1) sm->change(XTRACT_PEEL);
				else sm->change(DEPTH_PEEL);
				sm->use();
					sm->sendUniforms();
					if (i == num_layers - 1)
					{
						bindTexture(sum_aici_k, 0, GL_RGBA32F);
						bindTexture(sum_ai_k, 1, GL_RGBA32F);
						bindTexture(prod_1_ai_k, 2, GL_RGBA32F);
						bindSamplerTexture(previousDepthTexture);
						sm->getShader()->set1i(0, "previousDepthTexture");
					}
					else
					{
						//send texture for current minimum
						bindSamplerTexture(previousDepthTexture);
						sm->getShader()->set1i(0, "previousDepthTexture");
					}
					render();
				sm->unuse();
				retrievedDepthBuffer = retrieveDepthBuffer(fb->getDepthTex(), WIDTH, HEIGHT);
				//fill texture with current minimum from depth buffer
				bindSamplerTexture(previousDepthTexture);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, retrievedDepthBuffer);
			fb->unbind();
			if(i != num_layers-1 ) renderToQuad();
			#ifdef EXTRACT
				if(i == num_layers-2) saveScreen(std::string(FEATURES_PATH)+"blend_"+imageName, WIDTH, HEIGHT);
			#endif
			sm->change(DEPTH_PEEL);
		}
		sm->change(NORMAL); //back to normal;
		#ifdef EXTRACT
				std::vector<glm::vec4> sum_aici_buffer;
				std::vector<glm::vec4> sum_ai_buffer;
				std::vector<glm::vec4> prod_1_ai_buffer;
				std::vector<glm::vec4> depth_avg_image_buffer;
				std::vector<glm::vec4> minimum_depth_image_buffer;
				std::vector<glm::vec4> maximum_depth_image_buffer;

				std::vector<glm::vec4> sum_aici_k_buffer;
				std::vector<glm::vec4> sum_ai_k_buffer;
				std::vector<glm::vec4> prod_1_ai_k_buffer;

				sum_aici_buffer = retrieveFull(sum_aici, WIDTH, HEIGHT, 4);
				sum_ai_buffer = retrieveFull(sum_ai, WIDTH, HEIGHT, 4);
				prod_1_ai_buffer = retrieveFull(prod_1_ai, WIDTH, HEIGHT, 4);
				depth_avg_image_buffer = retrieveFull(depth_avg_image, WIDTH, HEIGHT, 4);
				minimum_depth_image_buffer = retrieveFull(minimum_depth_image, WIDTH, HEIGHT, 4);
				maximum_depth_image_buffer = retrieveFull(maximum_depth_image, WIDTH, HEIGHT, 4);

				sum_aici_k_buffer = retrieveFull(sum_aici_k, WIDTH, HEIGHT, 4);
				sum_ai_k_buffer = retrieveFull(sum_ai_k, WIDTH, HEIGHT, 4);
				prod_1_ai_k_buffer = retrieveFull(prod_1_ai_k, WIDTH, HEIGHT, 4);

				std::string name;
				int img_name = imageIndex + 1;
				std::string num = std::to_string(img_name);
				if (img_name < 10) name = "0000" + num;
				else if (img_name < 100) name = "000" + num;
				else name = "00" + num;

				saveFeatues(std::string(FEATURES_PATH)+"features_"+name, sum_aici_buffer,
																		 sum_aici_k_buffer, 
																		 sum_ai_buffer,
																		 sum_ai_k_buffer,
																		 prod_1_ai_buffer,
																		 prod_1_ai_k_buffer,
																		 depth_avg_image_buffer,
																		 minimum_depth_image_buffer,
																		 maximum_depth_image_buffer);
				saveScreen(std::string(FEATURES_PATH)+"blend_"+name, WIDTH, HEIGHT);
			#endif
	}

	void renderToQuad()
	{
		sm->change(QUAD);
		sm->use();
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);//Blending operator for front to back blending
			glBindVertexArray(quad->getVAO());
			glDisable(GL_DEPTH_TEST);
			glBindTexture(GL_TEXTURE_2D, fb->getColorTex());
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glFinish();
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		sm->unuse();
	}
};