#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

struct Row
{
    std::map<std::string,std::string> values;
};

struct Section
{
    std::string id;
    std::string title;
    std::string type;

    int priority;

    std::vector<std::string> columns;
    std::vector<Row> rows;
};

struct MasterViewModel
{
    std::vector<Section> sections;
};

struct SceneNode
{
    std::string id;

    float x;
    float y;

    float width;
    float height;

    std::string label;
};

std::string loadFile(
    const std::string& path)
{
    std::ifstream file(path);

    if(!file.is_open())
    {
        throw std::runtime_error(
            "failed to open file: " +
            path
        );
    }

    std::stringstream ss;

    ss << file.rdbuf();

    return ss.str();
}

GLuint compileShader(
    GLenum type,
    const std::string& source)
{
    GLuint shader =
        glCreateShader(type);

    const char* src =
        source.c_str();

    glShaderSource(
        shader,
        1,
        &src,
        nullptr
    );

    glCompileShader(shader);

    GLint success;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if(!success)
    {
        char log[1024];

        glGetShaderInfoLog(
            shader,
            sizeof(log),
            nullptr,
            log
        );

        std::cerr
            << log
            << std::endl;
    }

    return shader;
}

GLuint createProgram(
    const std::string& vsPath,
    const std::string& fsPath)
{
    auto vs =
        loadFile(vsPath);

    auto fs =
        loadFile(fsPath);

    GLuint vertex =
        compileShader(
            GL_VERTEX_SHADER,
            vs
        );

    GLuint fragment =
        compileShader(
            GL_FRAGMENT_SHADER,
            fs
        );

    GLuint program =
        glCreateProgram();

    glAttachShader(
        program,
        vertex
    );

    glAttachShader(
        program,
        fragment
    );

    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

MasterViewModel loadViewModel(
    const std::string& path)
{
    MasterViewModel model;

    std::ifstream file(path);

    json j;

    file >> j;

    for(auto& sectionJson :
        j["sections"])
    {
        Section section;

        section.id =
            sectionJson["id"];

        section.title =
            sectionJson["title"];

        section.type =
            sectionJson["type"];

        section.priority =
            sectionJson["priority"];

        model.sections.push_back(
            section
        );
    }

    return model;
}

std::vector<SceneNode>
buildScene(
    const MasterViewModel& model)
{
    std::vector<SceneNode> nodes;

    float y = 20.0f;

    for(const auto& section :
        model.sections)
    {
        SceneNode node;

        node.id =
            section.id;

        node.label =
            section.title;

        node.x = 20.0f;
        node.y = y;

        node.width = 1200.0f;
        node.height = 120.0f;

        nodes.push_back(node);

        y += 140.0f;
    }

    return nodes;
}

bool initFreeType(
    const std::string& fontPath)
{
    FT_Library ft;

    if(
        FT_Init_FreeType(
            &ft
        )
    )
    {
        return false;
    }

    FT_Face face;

    if(
        FT_New_Face(
            ft,
            fontPath.c_str(),
            0,
            &face
        )
    )
    {
        return false;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return true;
}

int main()
{
    glfwInit();

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window =
        glfwCreateWindow(
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            "Fluid UI Demo",
            nullptr,
            nullptr
        );

    glfwMakeContextCurrent(
        window
    );

    gladLoadGLLoader(
        (GLADloadproc)
        glfwGetProcAddress
    );

    std::cout
        << "GL Version: "
        << glGetString(
            GL_VERSION
        )
        << std::endl;

    GLuint shader =
        createProgram(
            "vertex.glsl",
            "fragment.glsl"
        );

    if(
        !initFreeType(
            "fonts/FiraMono-Regular.ttf"
        )
    )
    {
        std::cerr
            << "Font load failed\n";
    }

    auto model =
        loadViewModel(
            "master_view_model.json"
        );

    auto scene =
        buildScene(model);

    float vertices[] =
    {
        0.0f,0.0f,
        1.0f,0.0f,
        1.0f,1.0f,
        0.0f,1.0f
    };

    unsigned int indices[] =
    {
        0,1,2,
        2,3,0
    };

    GLuint vao;
    GLuint vbo;
    GLuint ebo;

    glGenVertexArrays(
        1,
        &vao
    );

    glGenBuffers(
        1,
        &vbo
    );

    glGenBuffers(
        1,
        &ebo
    );

    glBindVertexArray(
        vao
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        vbo
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        ebo
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        2*sizeof(float),
        nullptr
    );

    glEnableVertexAttribArray(
        0
    );

    GLint posLoc =
        glGetUniformLocation(
            shader,
            "uPosition"
        );

    GLint sizeLoc =
        glGetUniformLocation(
            shader,
            "uSize"
        );

    GLint screenLoc =
        glGetUniformLocation(
            shader,
            "uScreenSize"
        );

    GLint colorLoc =
        glGetUniformLocation(
            shader,
            "uColor"
        );

    while(
        !glfwWindowShouldClose(
            window
        )
    )
    {
        glfwPollEvents();

        glClearColor(
            0.05f,
            0.06f,
            0.08f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT
        );

        glUseProgram(
            shader
        );

        glUniform2f(
            screenLoc,
            WINDOW_WIDTH,
            WINDOW_HEIGHT
        );

        glBindVertexArray(
            vao
        );

        for(
            const auto& node :
            scene
        )
        {
            glUniform2f(
                posLoc,
                node.x,
                node.y
            );

            glUniform2f(
                sizeLoc,
                node.width,
                node.height
            );

            glUniform3f(
                colorLoc,
                0.2f,
                0.4f,
                0.8f
            );

            glDrawElements(
                GL_TRIANGLES,
                6,
                GL_UNSIGNED_INT,
                nullptr
            );
        }

        glfwSwapBuffers(
            window
        );
    }

    glfwTerminate();

    return 0;
}
