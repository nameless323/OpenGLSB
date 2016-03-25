#include <sb6.h>
#include "vmath.h"
#include "ShaderProgram.h"
#include <object.h>
#include <sb6ktx.h>

using vmath::mat4;

enum
{
	MAX_SCENE_WIDTH = 2048,
	MAX_SCENE_HEIGHT = 2048,
	SPHERE_COUNT = 32
};

class Bloom : public sb6::application
{
public:
	Bloom() : _exposure(1.0), _mode(0), _paused(false), _bloomfactor(1.0f), _showBloom(true), _showScene(true), _bloomThreshMax(1.2), _bloomThreshMin(0.8f), _showPrefilter(false)
	{
	}

	void LoadShaders()
	{
		_shaderRender.CreateProgram();
		_shaderRender.AttachShader("Shaders/Bloom/BloomScene.vert");
		_shaderRender.AttachShader("Shaders/Bloom/BloomScene.frag");
		_shaderRender.Link();
		SceneUniforms.BloomThreshMin = glGetUniformLocation(_shaderRender.GetHandler(), "bloom_thresh_min");
		SceneUniforms.BloomThreshMax = glGetUniformLocation(_shaderRender.GetHandler(), "bloom_thresh_max");

		_shaderFilter.CreateProgram();
		_shaderFilter.AttachShader("Shaders/Bloom/BloomFilter.vert");
		_shaderFilter.AttachShader("Shaders/Bloom/BloomFilter.frag");
		_shaderFilter.Link();

		_shaderResolve.CreateProgram();
		_shaderResolve.AttachShader("Shaders/Bloom/BloomResolve.vert");
		_shaderResolve.AttachShader("Shaders/Bloom/BloomResolve.frag");
		_shaderResolve.Link();
		ResolveUniforms.Exposure = glGetUniformLocation(_shaderResolve.GetHandler(), "exposure");
		ResolveUniforms.BloomFactor = glGetUniformLocation(_shaderResolve.GetHandler(), "bloom_factor");
		ResolveUniforms.SceneFactor = glGetUniformLocation(_shaderResolve.GetHandler(), "scene_factor");
	}

	void startup()
	{
		glGenVertexArrays(1, &_fsQuadVAO);
		glBindVertexArray(_fsQuadVAO);
		LoadShaders();

		glGenFramebuffers(1, &_renderFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);

		glGenTextures(1, &_sceneTex);
		glBindTexture(GL_TEXTURE_2D, _sceneTex);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _sceneTex, 0);

		glGenTextures(1, &_brightpassTex);
		glBindTexture(GL_TEXTURE_2D, _brightpassTex);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _brightpassTex, 0);

		glGenTextures(1, &_depthTex);
		glBindTexture(GL_TEXTURE_2D, _depthTex);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTex, 0);

		static const GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
		glDrawBuffers(2, buffers);

		glGenFramebuffers(2, _filterFBO);
		glGenTextures(2, _filterTex);
		static const GLenum bufferFilter[] = {GL_COLOR_ATTACHMENT0};
		for (int i = 0; i < 2; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _filterFBO[i]);
			glBindTexture(GL_TEXTURE_2D, _filterTex[i]);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, i ? MAX_SCENE_WIDTH : MAX_SCENE_HEIGHT, i ? MAX_SCENE_HEIGHT : MAX_SCENE_WIDTH);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _filterTex[i], 0);

			glDrawBuffers(1, bufferFilter);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		_object.load("media/objects/sphere.sbm");

		glGenBuffers(1, &_uboTransform);
		glBindBuffer(GL_UNIFORM_BUFFER, _uboTransform);
		glBufferData(GL_UNIFORM_BUFFER, (2 + SPHERE_COUNT) * sizeof(vmath::mat4), nullptr, GL_DYNAMIC_DRAW);

		struct material
		{
			vmath::vec3 diffuseColor;
			unsigned int : 32;
			vmath::vec3 specularColor;
			float specularPower;
			vmath::vec3 ambientColor;
			unsigned int : 32;
		};

		glGenBuffers(1, &_uboMaterial);
		glBindBuffer(GL_UNIFORM_BUFFER, _uboMaterial);
		glBufferData(GL_UNIFORM_BUFFER, SPHERE_COUNT * sizeof(material), nullptr, GL_STATIC_DRAW);

		material* m = (material*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, SPHERE_COUNT * sizeof(material), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		float ambient = 0.002f;
		for (int i = 0; i < SPHERE_COUNT; i++)
		{
			float fi = 3.14159267f * (float)i / 8.0;
			m[i].diffuseColor = vmath::vec3(sinf(fi) * 0.5f + 0.5f, sinf(fi + 1.345f) * 0.5f + 0.5f, sinf(fi + 2.567f) * 0.5f + 0.5);
			m[i].specularColor = vmath::vec3(2.8f, 2.8f, 2.9f);
			m[i].specularPower = 30.0f;
			m[i].ambientColor = vmath::vec3(ambient * 0.025f);
			ambient *= 1.5f;
		}
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	void onResize(int w, int h)
	{
		application::onResize(w, h);
	}

	void render(double currentTime)
	{
		_shaderRender.Use();

		static const GLfloat back[] = {0.0f, 0.0f, 0.0f, 1.0f};
		static const GLfloat one = 1.0f;
		static double lastTime = 0.0f;
		static double totalTime = 0.0f;

		if (!_paused)
			totalTime += (currentTime - lastTime);
		lastTime = currentTime;
		float t = (float)totalTime;

        glViewport(0, 0, info.windowWidth, info.windowHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, _renderFBO);
		glClearBufferfv(GL_COLOR, 0, back);
		glClearBufferfv(GL_COLOR, 1, back);
		glClearBufferfv(GL_DEPTH, 0, &one);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		_shaderRender.Use();

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, _uboTransform);
		struct transformsT
		{
			mat4 proj;
			mat4 view;
			mat4 model[SPHERE_COUNT];
		}* transforms = (transformsT*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(transformsT), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		transforms->proj = vmath::perspective(50.0f, (float)info.windowWidth / (float)info.windowHeight, 1.0f, 1000.0f);
		transforms->view = vmath::translate(0.0f, 0.0f, -20.0f);

		for (int i = 0; i < SPHERE_COUNT; i++)
		{
			float fi = 3.141592f * (float)i / 16;
			float r = (i & 2) ? 0.6f : 1.5f;
			transforms->model[i] = vmath::translate(cosf(t + fi) * 5.0f * r, sinf(t + fi * 4.0f) * 4.0f, sinf(t + fi) * 5.0f * r);
		}

		glUnmapBuffer(GL_UNIFORM_BUFFER);
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uboMaterial);

		glUniform1f(SceneUniforms.BloomThreshMin, _bloomThreshMin);
		glUniform1f(SceneUniforms.BloomThreshMax, _bloomThreshMax);
		_object.render(SPHERE_COUNT);
		glDisable(GL_DEPTH_TEST);
		_shaderFilter.Use();

		glBindVertexArray(_fsQuadVAO);
		glBindFramebuffer(GL_FRAMEBUFFER, _filterFBO[0]);
		glBindTexture(GL_TEXTURE_2D, _brightpassTex);
		glViewport(0, 0, info.windowHeight, info.windowWidth);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindFramebuffer(GL_FRAMEBUFFER, _filterFBO[1]);
		glBindTexture(GL_TEXTURE_2D, _filterTex[0]);
		glViewport(0, 0, info.windowWidth, info.windowHeight);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		_shaderResolve.Use();
		glUniform1f(ResolveUniforms.Exposure, _exposure);

		if (_showPrefilter)
		{
			glUniform1f(ResolveUniforms.BloomFactor, 0.0f);
			glUniform1f(ResolveUniforms.SceneFactor, 1.0f);
		}
		else
		{
			glUniform1f(ResolveUniforms.BloomFactor, _showBloom ? _bloomfactor : 0.0f);
			glUniform1f(ResolveUniforms.SceneFactor, _showScene ? 1.0f : 0.0f);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _filterTex[1]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _showPrefilter ? _brightpassTex : _sceneTex);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	void shutdown()
	{
		_shaderRender.DeleteProgram();
		_shaderFilter.DeleteProgram();
		_shaderResolve.DeleteProgram();
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &_fsQuadVAO);
		glDeleteTextures(1, &_brightpassTex);
		glDeleteTextures(1, &_depthTex);
		glDeleteTextures(2, _filterTex);
		glDeleteTextures(1, &_sceneTex);
		glDeleteBuffers(1, &_uboTransform);
		glDeleteBuffers(1, &_uboMaterial);
	}

	void onKey(int key, int action) override
	{
		if (action)
		{
			switch (key)
			{
			case '1':
			case '2':
			case '3':
				_mode = key - '1';
				break;
			case 'B':
				_showBloom = !_showBloom;
				break;
			case 'V':
				_showScene = !_showScene;
				break;
			case 'A':
				_bloomfactor += 0.1f;
				break;
			case 'Z':
				_bloomfactor -= 0.1f;
				break;
			case 'S':
				_bloomThreshMax += 0.1f;
				break;
			case 'X':
				_bloomThreshMin -= 0.1f;
				break;
			case 'D':
				_bloomThreshMax += 0.1f;
				break;
			case 'C':
				_bloomThreshMin -= 0.1f;
				break;
			case 'N':
				_showPrefilter = !_showPrefilter;
				break;
			case 'M':
				_mode = (_mode + 1) % 3;
				break;
			case 'P':
				_paused = !_paused;
				break;
			case GLFW_KEY_KP_ADD:
				_exposure *= 1.1f;
				break;
			case GLFW_KEY_KP_SUBTRACT:
				_exposure /= 1.1f;
				break;
			}

		}
	}

private:
	ShaderProgram _shaderRender;
	ShaderProgram _shaderFilter;
	ShaderProgram _shaderResolve;
	GLuint _fsQuadVAO;

	GLuint _srcTex;

	GLuint _renderFBO;
	GLuint _filterFBO[2];

	GLuint _sceneTex;
	GLuint _brightpassTex;
	GLuint _depthTex;
	GLuint _filterTex[2];

	float _exposure;
	int _mode;
	bool _paused;
	float _bloomfactor;
	bool _showBloom;
	bool _showScene;
	bool _showPrefilter;
	float _bloomThreshMin;
	float _bloomThreshMax;

	GLuint _uboTransform;
	GLuint _uboMaterial;

	struct
	{
		int BloomThreshMin;
		int BloomThreshMax;
	} SceneUniforms;

	struct
	{
		int Exposure;
		int BloomFactor;
		int SceneFactor;
	} ResolveUniforms;

	sb6::object _object;
};

//DECLARE_MAIN(Bloom);

