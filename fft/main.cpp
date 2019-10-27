#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <complex>
#include <chrono>

constexpr int count = 1024*512; //fft size
constexpr bool countFPS = true;
constexpr bool cpuFFT = false;

bool CheckCompile(GLuint shader) {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char info[1024];
        glGetShaderInfoLog(shader, sizeof(info), nullptr, info);
        std::cout << info << std::endl;
    }
    return success;
}

bool CheckLink(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char info[1024];
        glGetProgramInfoLog(program, sizeof(info), nullptr, info);
        std::cout << info << std::endl;
    }
    return success;
}

void fft(std::vector<std::complex<float>> &x);
void fft_inplace(std::vector<std::complex<float>> &x);
void fft_outplace(std::vector<std::complex<float>> &x);

void precompute_trig(int N);
extern std::vector<std::complex<float>> coef;
extern std::vector<uint32_t> indices;
extern std::string fft_source;

const std::string vertexSource =
    "#version 430\n"
    "layout(location = 0) in vec2 vposition;\n"
    "layout(location = 1) in float time_pos;\n"
    "void main() {\n"
    "   gl_Position = vec4(time_pos, vposition[0], 1.0, 1.0);\n"
    "}\n";

const std::string fragmentSource =
    "#version 330\n"
    "layout(location = 0) out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

int main() {
    constexpr int width = 640;
    constexpr int height = 480;

    if(glfwInit() == GL_FALSE) {
        std::cout << "glfwInit() failed" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow *window;
    if((window = glfwCreateWindow(width, height, "fft", 0, 0)) == 0) {
        std::cout << "glfwCreateWindow() failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "gladLoadGLLoader() failed" << std::endl;
        return -1;
    }

    GLint sizes[3];
    GLint inv_cnt;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &sizes[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &sizes[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &sizes[2]);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &inv_cnt);
    std::cout << "workgroup sizes: " << sizes[0] << " " << sizes[1] << " " << sizes[2] << std::endl;
    std::cout << "max inv count" << inv_cnt << std::endl;

    GLuint shader_program, vertex_shader, fragment_shader;
    const char *source;
    int length;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    source = vertexSource.c_str();
    length = vertexSource.size();
    glShaderSource(vertex_shader, 1, &source, &length);
    glCompileShader(vertex_shader);
    if(!CheckCompile(vertex_shader)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = fragmentSource.c_str();
    length = fragmentSource.size();
    glShaderSource(fragment_shader, 1, &source, &length);
    glCompileShader(fragment_shader);
    if(!CheckCompile(fragment_shader)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    CheckLink(shader_program);

    GLuint fft_program, fft_shader;
    fft_shader = glCreateShader(GL_COMPUTE_SHADER);
    source = fft_source.c_str();
    length = fft_source.size();
    glShaderSource(fft_shader, 1, &source, &length);
    glCompileShader(fft_shader);
    if(!CheckCompile(fft_shader)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    fft_program = glCreateProgram();
    glAttachShader(fft_program, fft_shader);
    glLinkProgram(fft_program);
    CheckLink(fft_program);

    double freq = count/4;
    std::vector<std::complex<float>> positionData;
    positionData.resize(count);
    std::vector<float> time_data;
    time_data.resize(count);
    for (int i = 0; i < count; i++)
        time_data[i] = (i<=count/2 ? 0.0f : -1.8f) + 1.8f*i/count;
    precompute_trig(count);
    const double pi = acos(-1);

    GLuint vao, positions_vbo, time_vbo, trig_vbo, tmp_vbo, ind_vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &positions_vbo);

    glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(std::complex<float>), positionData.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

    glGenBuffers(1, &time_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, time_vbo);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(float), time_data.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0);

    glGenBuffers(1, &trig_vbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, trig_vbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, count*sizeof(float), coef.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ind_vbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ind_vbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, count*sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &tmp_vbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, tmp_vbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, count*sizeof(std::complex<float>), 0, GL_DYNAMIC_DRAW);

    const GLuint ssbos[] = {positions_vbo, trig_vbo, tmp_vbo, ind_vbo};
    glBindBuffersBase(GL_SHADER_STORAGE_BUFFER, 0, 4, ssbos);
    glUseProgram(fft_program);
    glUniform1i(0, count);
    glUseProgram(shader_program);

    glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);

    auto t1 = std::chrono::high_resolution_clock::now();
    float frame_counter = 0.0f;
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_DOWN))
            freq -= (double)count/1000.f;
        else if(glfwGetKey(window, GLFW_KEY_UP))
            freq += (double)count/1000.f;

        for (int i = 0; i < count; i++) {
            double w = i*2*pi*freq/count;
            positionData[i] = {cosf(w), sinf(w)};//{sinf(w), 0.0f/*cosf(w)*/}; //{cosf(w), sinf(w)};
        }

        if (cpuFFT) {
            fft_outplace(positionData);
            for (int i = 0; i < count; i++)
                positionData[i].real(std::abs(positionData[i])/count);
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(std::complex<float>), positionData.data());
        if (!cpuFFT) {
            glUseProgram(fft_program);
            glDispatchCompute(1, 1, 1);
            glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, count);

        if(glGetError() != GL_NO_ERROR)
            break;

        glfwSwapBuffers(window);
        if (countFPS) {
            frame_counter++;
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> dt = t2-t1;
            if (dt.count() >= 1.0f) {
                t1 = t2;
                float fps = frame_counter/ dt.count();
                printf("FPS: %.1f\n", fps);
                frame_counter = 0.0f;
            }
        }

    }
    //cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &positions_vbo);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glDeleteProgram(shader_program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
