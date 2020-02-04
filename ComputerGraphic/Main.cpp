#include "Main.h"



using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 10.0f, 0.0f);

//Speed
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Center of screen
float lastX = 400, lastY = 300;
float yaw = -90, pitch = 0;

bool firstMouse;

//Fov
float fov = 45.0f;


int main() {

    //Initialize GLFW setting
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    //Initialize window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Lab", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Set rendering size
    glViewport(0, 0, 800, 600);

    //Set window size when resize happening
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Get project path
    char buffer[256];
    _getcwd(buffer, 256);
    string path = buffer;
    string vspath = path + "\\Shader.vs";
    string fspath = path + "\\Shader.fs";

    //Create Shaders
    Shader ourShader(vspath.c_str(), fspath.c_str());
    
    //Load FileManager
    FileManager fm;

    ////Input object data
    //Import model (in project directory)////////////////////
    string model = "biplane.d.txt";
    /////////////////////////////////////////////////////////

    string modelpath = path + "\\" + model;

    float *triangles = fm.load(modelpath.c_str());
    float size = fm.getTriangleNum() * 3 * 3;
    float *vertices = new float[size];
    for (int i = 0; i < size; i++) {
        vertices[i] = triangles[i];
        //cout << vertices[i] << endl;
    } 

    //unsigned int indices[] = {  // note that we start from 0!
    //0, 1, 3,   // first triangle
    //1, 2, 3    // second triangle
    //};

    //Create VAO to save VBOs
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    //Create VBO buffer
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);

    ////Create EBO buffer
    //unsigned int EBO;
    //glGenBuffers(1, &EBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Vertex data interpretor
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);   //index = 0, 3 vertics, size = 3 * float
    glEnableVertexAttribArray(0);

    ////Display mode///////////////////////////////
    //Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //glPolygonMode(GL_FRONT, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    //Enable Z-Buffer
    glEnable(GL_DEPTH_TEST);

    //Remove back face
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    ////Callback functions/////////////////////////
    //Input mode
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //Mouse callback
    glfwSetCursorPosCallback(window, mouse_callback);
    //Scroll callback
    glfwSetScrollCallback(window, scroll_callback);

    ///////////////////////////////////////////////
    //Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        //input
        processInput(window);

        //Rendering commands
        //Clear color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Clear Z-Buffer before rendering
        glClear(GL_DEPTH_BUFFER_BIT);

        //Import shader
        ourShader.use();
        ourShader.setFloat("someUniform", 1.0f);

        //Calculate time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //model, camera(view), projection locations
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 2.0f));
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        //glm::mat4 view = glm::mat4(1.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f)); //x, y, distance

        const float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 200.0f);


        //Transfer locations to shader
        int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            //Draw
            //Use program object
        glUseProgram(ourShader.ID);
        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 108, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, size);
        glBindVertexArray(0);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}

//Return window size callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//Input process
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    const float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    if (xoffset > 150 || yoffset > 150) {
        xoffset = 0;
        yoffset = 0;
    }
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov > 1.0f && fov < 45.0f)
        fov -= yoffset;
    else if (fov <= 1.0f)
        fov = 1.0f;
    else if (fov >= 45.0f)
        fov = 45.0f;
}
