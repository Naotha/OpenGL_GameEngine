#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const unsigned short windowWidth = 800;
const unsigned short windowHeight = 600;

/* Vertices */
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

/* Shaders */
const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 " gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   " FragColor = vec4(0.4f, 0.4f, 0.4f, 1.0f);\n"
                                   "}\0";

void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* Set Viewport */
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, frambuffer_size_callback); // Change viewport when window is resized

    /* Create VBO */    
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    
    /* Initialize Shaders */
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    /* Initialize Shader Program */
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Delete linked Shader Objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /* Create VAO */
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // Copy vertices array to VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* User inputs */
        processInput(window);
        
        /* Rendering */
        glClearColor(0.1f,0.4f,0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw Objects */
        // Use Shader Program
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Poll for and process events */
        glfwPollEvents();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}