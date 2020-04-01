#include "Main.h"
#define LINE_LEN 9

#define CONSTANT_SHADING 0
#define GOURAND_SHADING 1
#define PHONG_SHADING 2

#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}


using namespace std;

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::vec3 calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 10.0f, 0.0f);

//Speed
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Center of screen
//float lastX = 400, lastY = 300;
float yaw = -90, pitch = 0;

bool firstMouse;

//Fov
float fov = 45.0f;

// lighting
glm::vec3 lightPos(-0.8f, 3.0f, -4.0f);

//Shading mode
int shading_mode = 0;


int main() {

    //Initialize GLFW setting
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //Initialize window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab", NULL, NULL);
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
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

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

    string gouraud_shader_vs = path + "\\GouraudShader.vs";
    string gouraud_shader_fs = path + "\\GouraudShader.fs";

    string phong_shader_vs = path + "\\PhongShader.vs";
    string phong_shader_fs = path + "\\PhongShader.fs";

    string constant_shader_vs = path + "\\ConstantShader.vs";
    string constant_shader_fs = path + "\\ConstantShader.fs";

    string skybox_shader_vs = path + "\\SkyboxShader.vs";
    string skybox_shader_fs = path + "\\SkyboxShader.fs";


    //Create Shaders
    //Shader ourShader(vspath.c_str(), fspath.c_str());
    Shader lightingShader(vspath.c_str(), ls_path.c_str());
    Shader lightSourceShader(vspath.c_str(), lss_path.c_str());
    Shader gouraudShader(gouraud_shader_vs.c_str(), gouraud_shader_fs.c_str());
    Shader phongShader(phong_shader_vs.c_str(), phong_shader_fs.c_str());
    Shader constantShader(constant_shader_vs.c_str(), constant_shader_fs.c_str());
    Shader skyboxShader(skybox_shader_vs.c_str(), skybox_shader_fs.c_str());
    //Shader lightSourceShader(vspath.c_str(), lss_path.c_str());

    //Load FileManager
    FileManager fm;

    std::vector<std::string> faces{
        path + "\\skybox\\right.jpg",
        path + "\\skybox\\left.jpg",
        path + "\\skybox\\top.jpg",
        path + "\\skybox\\bottom.jpg",
        path + "\\skybox\\front.jpg",
        path + "\\skybox\\back.jpg"
    };
    
    Skybox skybox(faces, &skyboxShader);
    skybox.loadCubemap();

    ////Input object data
    //Import model (in project directory)////////////////////
    string model = "better-ball.d.txt";
    /////////////////////////////////////////////////////////

    string modelpath = path + "\\" + model;
 

    // Load model
    Model model_1 = fm.load(modelpath.c_str(), true, true);
    model_1.loadTexture(1, (path + "\\" + "Moon.jpg").c_str());

    // Set model 1 color
    std::srand((unsigned)time(NULL));
    //add random color
    list<glm::vec4> color_map;
    for (int i = 0; i < model_1.getVerticesAmount(); i++) {
        glm::vec4 color;
        color.r = rand() / double(RAND_MAX);
        color.g = rand() / double(RAND_MAX);
        color.b = rand() / double(RAND_MAX);
        color.a = 1.0f;
        color_map.insert(color_map.end(), color);
    }
    model_1.setColor(color_map);


    //float* vertices = model_1.getNoneEBOVertices(true);
    

    //DEBUG///////////////////////////////////
    //for (int i = 0; i < size; i++) {
    //    for (int j = i * 9; j < i * 9 + 9; j++) {
    //        cout << vertices[j] << " ";
    //    }
    //    cout << endl;

    //}

    //for (int i = 0; i < model_1.getPolygonsAmount(); i++) {
    //    for (int j = i * 3; j < i * 3 + 3; j++) {
    //        cout << indices[j] << " ";
    //    }
    //    cout << endl;
    //}
    ///////////////////////////////////////

    //-------------------------------------------------------------------------------------------------

    
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

    //Shading mode
    shading_mode = PHONG_SHADING;


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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Clear Z-Buffer before rendering
        glClear(GL_DEPTH_BUFFER_BIT);


        //Calculate time    //////////////////////////////////////////////////////////////////////////////////////
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        //Import shader ////////////////////////////////////////////////////////////////////////////////////////
        //ourShader.use();
        //ourShader.setFloat("someUniform", 1.0f);
        
        // Test Shader
        //lightingShader.use();
        //lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        //lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        //lightingShader.setMat4("projection", projection);
        //lightingShader.setMat4("view", view);
        //lightingShader.setMat4("model", model);
        //lightingShader.setVec3("lightPos", lightPos);
        
        switch (shading_mode)
        {
            // Constant shading
        case CONSTANT_SHADING:
            constantShader.use();
            constantShader.setVec3("objectColor", 0.87f, 0.87f, 0.87f);
            constantShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            constantShader.setMat4("projection", projection);
            constantShader.setMat4("view", view);
            constantShader.setMat4("model", model);
            constantShader.setVec3("lightPos", lightPos);
            constantShader.setVec3("viewPos", cameraPos);
            model_1.show(constantShader);
            break;
            // Gouraud shading
        case GOURAND_SHADING:
            gouraudShader.use();
            gouraudShader.setVec3("objectColor", 0.87f, 0.87f, 0.87f);
            gouraudShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            gouraudShader.setMat4("projection", projection);
            gouraudShader.setVec3("viewPos", cameraPos);
            gouraudShader.setMat4("view", view);
            gouraudShader.setMat4("model", model);
            gouraudShader.setVec3("lightPos", lightPos);
            model_1.show(gouraudShader);
            break;
            // Phong shading
        case PHONG_SHADING:
            phongShader.use();
            phongShader.setVec3("objectColor", 0.87f, 0.87f, 0.87f);
            phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
            phongShader.setMat4("projection", projection);
            phongShader.setMat4("view", view);
            phongShader.setMat4("model", model);
            phongShader.setVec3("lightPos", lightPos);
            model_1.show(phongShader);
            break;
        default:
            break;
        } 

        // Skybox
        view = camera.GetViewMatrix();
        skyboxShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skybox.show();

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    model_1.clear();
    skybox.clear();
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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
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
    //cout << e_normal[0] << " " << e_normal[1] << " " << e_normal[2] << endl;
    return e_normal;
}

