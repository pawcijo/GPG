#include "Shader.h"
#include <vector>

#include <print>

// constructor reads and builds the shader
Shader::Shader(std::filesystem::path vertex_file_path, std::filesystem::path fragment_file_path, std::filesystem::path compute_file_path)
{
	// Create the shaders
	GLuint VertexShaderID;
	if (!vertex_file_path.empty())
	{
		VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	}
	GLuint FragmentShaderID;
	if (!fragment_file_path.empty())
	{
		FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	}
	GLuint ComputeShaderID;
	if (!compute_file_path.empty())
	{
		ComputeShaderID = glCreateShader(GL_COMPUTE_SHADER);
	}

	// Read the Vertex Shader code from the files
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path.c_str(), std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path.c_str(), std::ios::in);
	if (FragmentShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	// Read the Compute Shader code from the file
	std::string ComputeShaderCode;
	std::ifstream ComputeShaderStream(compute_file_path.c_str(), std::ios::in);
	if (ComputeShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << ComputeShaderStream.rdbuf();
		ComputeShaderCode = sstr.str();
		ComputeShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	if (!vertex_file_path.empty())
	{
		// Compile Vertex Shader

		std::println("Compiling shader : {}.", vertex_file_path.c_str());
		char const *VertexSourcePointer = VertexShaderCode.c_str();

		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}
	}

	if (!fragment_file_path.empty())
	{
		// Compile Fragment Shader
		std::println("Compiling shader : {}.", fragment_file_path.c_str());
		char const *FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}
	}

	if (!compute_file_path.empty())
	{
		// Compile Compute Shader
		std::println("Compiling shader : {}.", compute_file_path.c_str());
		char const *ComputeSourcePointer = ComputeShaderCode.c_str();
		glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, NULL);
		glCompileShader(ComputeShaderID);

		// Check Compute Shader
		glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0)
		{
			std::vector<char> ComputeShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(ComputeShaderID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]);
			std::println("Error Message : {}.", &ComputeShaderErrorMessage[0]);
		}
	}

	// Link the program
	std::println("Linking program ");

	GLuint ProgramID = glCreateProgram();
	if (!vertex_file_path.empty())
	{
		glAttachShader(ProgramID, VertexShaderID);
	}
	if (!fragment_file_path.empty())
	{
		glAttachShader(ProgramID, FragmentShaderID);
	}
	if (!compute_file_path.empty())
	{
		glAttachShader(ProgramID, ComputeShaderID);
	}
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::println("Error Message : {}.", &ProgramErrorMessage[0]);
	}

	if (!vertex_file_path.empty())
	{
		glDetachShader(ProgramID, VertexShaderID);
		glDeleteShader(VertexShaderID);
	}
	if (!fragment_file_path.empty())
	{
		glDetachShader(ProgramID, FragmentShaderID);
		glDeleteShader(FragmentShaderID);
	}
	if (!compute_file_path.empty())
	{
		glDetachShader(ProgramID, ComputeShaderID);
		glDeleteShader(ComputeShaderID);
	}

	shaderProgramID = ProgramID;
}

Shader::~Shader()
{
	std::println("Shader program removed.");
	glDeleteProgram(shaderProgramID);
}

// use/activate the shader
void Shader::use()
{
	glUseProgram(shaderProgramID);
}

unsigned Shader::GetUniformLocation(const char *pUniformName)
{

	unsigned Location = glGetUniformLocation(shaderProgramID, pUniformName);

	if (Location == INVALID_UNIFORM_LOCATION)
	{
		fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
	}

	return Location;
}

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
	glUniform2fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(shaderProgramID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
	glUniform3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(shaderProgramID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
	glUniform4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(shaderProgramID, name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
