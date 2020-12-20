#include <iostream>
#include <ngf/Graphics/Shader.h>
#include "GlDebug.h"

namespace ngf {

namespace {
GLuint compileShader(Shader::Type type, const char *source) {
  auto id = glCreateShader(type == Shader::Type::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);

  // compile
  glShaderSource(id, 1, &source, nullptr);
  glCompileShader(id);
  GLint compileStatus = GL_FALSE;
  GL_CHECK(glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus));

  // if compilation failed
  if (compileStatus == GL_FALSE) {
    GLint infoLogLength = 0;
    GL_CHECK(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength));

    assert(infoLogLength > 0);
    std::vector<char> infoLog(infoLogLength);
    GL_CHECK(glGetShaderInfoLog(id, infoLogLength, nullptr, infoLog.data()));

    const auto typeText = type == Shader::Type::Vertex ? "Vertex" : "Fragment";
    std::cerr << "Error while compiling " << typeText << "shader:\n" << infoLog.data() << "\n";
  }
  return id;
}

GLuint compile(const char *vertexShaderCode, const char *fragmentShaderCode) {
  assert(vertexShaderCode != nullptr || fragmentShaderCode != nullptr);

  GLuint program;
  GL_CHECK(program = glCreateProgram());

  if (vertexShaderCode != nullptr) {
    auto id = compileShader(Shader::Type::Vertex, vertexShaderCode);
    GL_CHECK(glAttachShader(program, id));
    GL_CHECK(glDeleteShader(id)); // the shader is still here because it is attached to the program
  }

  if (fragmentShaderCode != nullptr) {
    auto id = compileShader(Shader::Type::Fragment, fragmentShaderCode);
    GL_CHECK(glAttachShader(program, id));
    GL_CHECK(glDeleteShader(id)); // the shader is still here because it is attached to the program
  }

  GL_CHECK(glLinkProgram(program));

  GLint linkStatus = GL_FALSE;
  GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));

  if (linkStatus == GL_FALSE) {
    GLint infoLogLength;
    GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength));

    assert(infoLogLength > 0);
    std::vector<char> infoLog(infoLogLength);
    GL_CHECK(glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.data()));

    std::cerr << "Error while linking program:\n" << infoLog.data() << "\n";
    throw std::runtime_error("Error while linking program");
  }

  return program;
}
}

struct Shader::Guard {
  explicit Guard(const Shader &shader)
      : m_previousProgram(0), m_currentProgram(shader.m_program) {
    GLint prev = 0;
    GL_CHECK(glGetIntegerv(GL_CURRENT_PROGRAM, &prev));
    m_previousProgram = prev;

    if (m_previousProgram != m_currentProgram) {
      GL_CHECK(glUseProgram(m_currentProgram));
    }
  }

  ~Guard() {
    if (m_previousProgram != m_currentProgram) {
      GL_CHECK(glUseProgram(m_previousProgram));
    }
  }

  GLuint m_previousProgram;
  GLuint m_currentProgram;
};

Shader::Shader(const char *vertexShader, const char *fragmentShader) {
  if (vertexShader == nullptr && fragmentShader == nullptr) {
    return;
  }

  m_program = compile(vertexShader, fragmentShader);
}

void Shader::load(const char *vertexShader, const char *fragmentShader) {
  if (vertexShader == nullptr && fragmentShader == nullptr) {
    return;
  }

  m_program = compile(vertexShader, fragmentShader);
}

Shader::~Shader() {
  if (m_program != 0) {
    GL_CHECK(glDeleteProgram(m_program));
  }
}

void Shader::setUniform(std::string_view name, int value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform1i(loc, value));
}

void Shader::setUniform(std::string_view name, float value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform1f(loc, value));
}

void Shader::setUniformArray(std::string_view name, const float *vectorArray, std::size_t length) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform1fv(loc, static_cast<GLsizei>(length), vectorArray));
}

void Shader::setUniform(std::string_view name, glm::vec2 value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform2f(loc, value.x, value.y));
}

void Shader::setUniformArray(std::string_view name, const glm::vec2 *vectorArray, std::size_t length) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform2fv(loc, static_cast<GLsizei>(length), glm::value_ptr(vectorArray[0])));
}

void Shader::setUniform(std::string_view name, glm::vec3 value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform3f(loc, value.x, value.y, value.z));
}

void Shader::setUniformArray(std::string_view name, const glm::vec3 *vectorArray, std::size_t length) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform3fv(loc, static_cast<GLsizei>(length), glm::value_ptr(vectorArray[0])));
}

void Shader::setUniform(std::string_view name, glm::vec4 value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform4f(loc, value.x, value.y, value.z, value.w));
}

void Shader::setAttribute(std::string_view name, const glm::vec2 &value) const {
  Guard guard(*this);
  auto loc = getAttributeLocation(name);
  GL_CHECK(glVertexAttrib2f(loc, value.x, value.y));
}

void Shader::setUniform(std::string_view name, const glm::mat4 &value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value)));
}

void Shader::setUniform(std::string_view name, const glm::mat3 &value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value)));
}

void Shader::setUniform(std::string_view name, const Texture &tex) {
  Guard guard(*this);
  int loc = getUniformLocation(name);
  if (loc == -1)
    return;
  m_textures[loc] = &tex;
}

void Shader::setUniform3(std::string_view name, const ngf::Color &value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform3f(loc, value.r, value.g, value.b));
}

void Shader::setUniformArray3(std::string_view name, const ngf::Color *vectorArray, std::size_t length) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  std::vector<float> array(length * 3);
  for (size_t i = 0; i < length; i++) {
    array[3 * i] = vectorArray[i].r;
    array[3 * i + 1] = vectorArray[i].g;
    array[3 * i + 2] = vectorArray[i].b;
  }
  GL_CHECK(glUniform3fv(loc, static_cast<GLsizei>(length), array.data()));
}

void Shader::setUniform4(std::string_view name, const ngf::Color &value) const {
  Guard guard(*this);
  auto loc = getUniformLocation(name);
  GL_CHECK(glUniform4f(loc, value.r, value.g, value.b, value.a));
}

void Shader::bind(const Shader *shader) {
  if (shader != nullptr && shader->m_program != 0) {
    GL_CHECK(glUseProgram(static_cast<GLuint>(shader->m_program)));

    // bind textures
    GLint index = 0;
    for (auto &item : shader->m_textures) {
      GL_CHECK(glActiveTexture(GL_TEXTURE0 + index));
      GL_CHECK(glUniform1i(item.first, index));
      Texture::bind(item.second);
      index++;
    }

  } else {
    GL_CHECK(glUseProgram(0));
  }
}

int Shader::getAttributeLocation(std::string_view name) const {
  GLint loc;
  GL_CHECK(loc = glGetAttribLocation(static_cast<GLuint>(m_program), name.data()));
  return loc;
}

int Shader::getUniformLocation(std::string_view name) const {
  GLint loc;
  GL_CHECK(loc = glGetUniformLocation(static_cast<GLuint>(m_program), name.data()));
  return loc;
}

}
