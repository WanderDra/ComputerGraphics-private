#include "Main.h"
#define LINE_LEN 9

#define CONSTANT_SHADING 0
#define GOURAND_SHADING 1
#define PHONG_SHADING 2


using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::vec3 calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3);

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

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


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

    string ls_path = path + "\\LightingShader.fs";
    string lss_path = path + "\\LightSourceShader.fs";

    //Create Shaders
    //Shader ourShader(vspath.c_str(), fspath.c_str());
    Shader lightingShader(vspath.c_str(), ls_path.c_str());
    Shader lightSourceShader(vspath.c_str(), lss_path.c_str());
    //Shader lightSourceShader(vspath.c_str(), lss_path.c_str());

    //Load FileManager
    FileManager fm;

    ////Input object data
    //Import model (in project directory)////////////////////
    string model = "better-ball.d.txt";
    /////////////////////////////////////////////////////////

    string modelpath = path + "\\" + model;

    float* triangles = fm.load(modelpath.c_str(), false);
    float size = fm.getTriangleNum() * 9 * 3;
    float* vertices = new float[size];

    int flag = 0;
    int normal_flag = 0;
    int tri_count = 0;
    int group_flag = 0;
    glm::vec3 normal;
    for (int i = 0; i < size; i++) {
        if (normal_flag == 3 * LINE_LEN) {
            normal_flag = 0;
        }
        if (normal_flag == 0) {
            glm::vec3 vec[3];
            vec[0] = glm::vec3(triangles[tri_count], triangles[tri_count + 1], triangles[tri_count + 2]);
            vec[1] = glm::vec3(triangles[tri_count + 3], triangles[tri_count + 4], triangles[tri_count + 5]);
            vec[2] = glm::vec3(triangles[tri_count + 6], triangles[tri_count + 7], triangles[tri_count + 8]);
            normal = calTriNormal(vec[0], vec[1], vec[2]);
        }
        if (flag == LINE_LEN) {
            flag = 0;
        }
        if (group_flag == 3) {
            group_flag = 0;
        }

        if (flag < 3) {
            vertices[i] = triangles[tri_count];
            tri_count++;
            //cout << vertices[i] << endl;
        }
        else if(flag < 6){
            vertices[i] = 0.0f;
        }
        else if (flag < 9) {
            vertices[i] = normal[group_flag];
            group_flag++;
        }
        flag++;
        normal_flag++;
    } 



    std::srand((unsigned)time(NULL));
    //add random color
    for (int i = 0; i < size; i += 27) {
        float r = rand() / double(RAND_MAX);
        float g = rand() / double(RAND_MAX);
        float b = rand() / double(RAND_MAX);
        for (int j = i; j < i + 27; j += 9) {
            vertices[j + 3] = r;
            vertices[j + 4] = g;
            vertices[j + 5] = b;
        }
    }

    //DEBUG///////////////////////////////////
    for (int i = 0; i < size; i += 9) {
        for (int j = i; j < i + 9; j++) {
            cout << vertices[j] << " ";
        }
        cout << endl;

    }
    /////////////////////////////////////////

    float cube[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };
    
    

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

    ////Pointer to attr in GLSL////////////////////
    // x, y, z, r, g, b
    //Vertex data interpretor
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);   //index = 0, 3 vertics, size = 3 * float
    glEnableVertexAttribArray(0);
    //Color data interpretor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //Normal data interpretor
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    unsigned int lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    //Vertex data interpretor
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);   //index = 0, 3 vertics, size = 3 * float
    glEnableVertexAttribArray(0);

    ////Create EBO buffer
    //unsigned int EBO;
    //glGenBuffers(1, &EBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ////Display mode///////////////////////////////
    //Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Clear Z-Buffer before rendering
        glClear(GL_DEPTH_BUFFER_BIT);

        //Calculate time    //////////////////////////////////////////////////////////////////////////////////////
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

        //Import shader ////////////////////////////////////////////////////////////////////////////////////////
        //ourShader.use();
        //ourShader.setFloat("someUniform", 1.0f);

        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("model", model);
        lightingShader.setVec3("lightPos", lightPos);

        //lightSourceShader.use();
        //lightSourceShader.setMat4("projection", projection);
        //lightSourceShader.setMat4("view", view);
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, lightPos);
        //model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        //lightSourceShader.setMat4("model", model);
        


        //Transfer locations to shader
        //int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        //int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        //glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        //int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        //glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            //Draw
            //Use program object
        //glUseProgram(ourShader.ID);
        glUseProgram(lightingShader.ID);
        //glBindVertexArray(VAO);
        glBindVertexArray(VAO);
        //glDrawElements(GL_TRIANGLES, 108, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, size);

        //glUseProgram(lightSourceShader.ID);
        //glBindVertexArray(lightVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);


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


glm::vec3 calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3)
{
    float temp1[3], temp2[3], normal[3];
    float length = 0.0f;
    temp1[0] = ver2[0] - ver1[0];
    temp1[1] = ver2[1] - ver1[1];
    temp1[2] = ver2[2] - ver1[2];
    temp2[0] = ver3[0] - ver2[0];
    temp2[1] = ver3[1] - ver2[1];
    temp2[2] = ver3[2] - ver2[2];
    // Calculate normal
    normal[0] = temp1[1] * temp2[2] - temp1[2] * temp2[1];
    normal[1] = -(temp1[0] * temp2[2] - temp1[2] * temp2[0]);
    normal[2] = temp1[0] * temp2[1] - temp1[1] * temp2[0];
    // Unit
    length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    if (length == 0.0f) { length = 1.0f; }
    normal[0] /= length;
    normal[1] /= length;
    normal[2] /= length;
    glm::vec3 e_normal(normal[0], normal[1], normal[2]);
    cout << e_normal[0] << " " << e_normal[1] << " " << e_normal[2] << endl;
    return e_normal;
}

