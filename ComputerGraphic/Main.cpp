#include "Main.h"
#include <stb/stb_image.h>
#include <map>
#define LINE_LEN 9

#define CONSTANT_SHADING 0
#define GOURAND_SHADING 1
#define PHONG_SHADING 2
#define PHONG_SHADING_SHADOW 3

#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}


using namespace std;

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
void renderScene(const Shader& shader);
void renderCube();
void renderQuad();
glm::vec3 calTriNormal(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

// Depth map
unsigned int depthMapFBO;

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
glm::vec3 lightPos(-2.0f, 10.0f, -1.0f);

//Shading mode
int shading_mode = 0;

// meshes
unsigned int planeVAO;


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

    string phong_shader_shadow_vs = path + "\\PhongShaderShadow.vs";
    string phong_shader_shadow_fs = path + "\\PhongShaderShadow.fs";

    string constant_shader_vs = path + "\\ConstantShader.vs";
    string constant_shader_fs = path + "\\ConstantShader.fs";

    string skybox_shader_vs = path + "\\SkyboxShader.vs";
    string skybox_shader_fs = path + "\\SkyboxShader.fs";

    string simple_depth_vs = path + "\\simpleDepthShader.vs";
    string simple_depth_fs = path + "\\simpleDepthShader.fs";


    //Create Shaders
    //Shader ourShader(vspath.c_str(), fspath.c_str());
    Shader lightingShader(vspath.c_str(), ls_path.c_str());
    Shader lightSourceShader(vspath.c_str(), lss_path.c_str());
    Shader gouraudShader(gouraud_shader_vs.c_str(), gouraud_shader_fs.c_str());
    Shader phongShader(phong_shader_vs.c_str(), phong_shader_fs.c_str());
    Shader constantShader(constant_shader_vs.c_str(), constant_shader_fs.c_str());
    Shader skyboxShader(skybox_shader_vs.c_str(), skybox_shader_fs.c_str());
    Shader shadowPhongShader(phong_shader_shadow_vs.c_str(), phong_shader_shadow_fs.c_str());
    Shader simpleDepthShader(simple_depth_vs.c_str(), simple_depth_fs.c_str());
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

    // Model Manager
    ModelManager model_manager;

    ////Input object data
    //Import model (in project directory)////////////////////
    string model = "better-ball.d.txt";
    string model_cow = "cow.d.txt";
    //string floor = "bishop.d.txt";
    //string model = "cow.d.txt";

    /////////////////////////////////////////////////////////

    string modelpath = path + "\\" + model;
    model_cow = path + "\\" + model_cow;
    //floor = path + "\\" + floor;


    // Load model
    Model model_1 = fm.load(modelpath.c_str(), true, true);
    glm::vec3 model_1_pos = glm::vec3(-6.0f, 1.0f, 0.0f);
    model_1.loadTexture(1, (path + "\\" + "Moon.jpg").c_str());
    model_manager.addModel(&model_1, model_1_pos);
    Model model_2 = fm.load(modelpath.c_str(), true, true);
    glm::vec3 model_2_pos = glm::vec3(8.0f, 1.1f, -4.0f);
    model_2.loadTexture(2, (path + "\\" + "timg.png").c_str());
    model_manager.addModel(&model_2, model_2_pos);
    Model model_cow_m = fm.load(model_cow.c_str(), true, true);
    glm::vec3 model_cow_pos = glm::vec3(0.5f, 3.0f, -0.3f);
    model_cow_m.loadTexture(3, (path + "\\" + "cow.png").c_str());
    model_manager.addModel(&model_cow_m, model_cow_pos);
    //Model floor_m = fm.load(floor.c_str(), true, true);
    //floor_m.loadTexture(4, (path + "\\" + "timg.png").c_str());
    //model_manager.addModel(&floor_m, glm::vec3(0.0f, 0.0f, -3.0f));

     // load textures
    // -------------
    unsigned int floorTexture = loadTexture((path + "\\floor.png").c_str());

    // Shadow////////////////////////////////////////////////

    float planeVertices[] = {
        // positions            // normals         // texcoords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
    };
    // plane VAO
    unsigned int planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

// configure depth map FBO
// -----------------------
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ///////////////////////////////////////////////////////
    shadowPhongShader.use();
    shadowPhongShader.setInt("ourTexture", 0);
    shadowPhongShader.setInt("shadowMap", 1);


    //// Set model 1 color
    //std::srand((unsigned)time(NULL));
    ////add random color
    //list<glm::vec4> color_map;
    //for (int i = 0; i < model_1.getVerticesAmount(); i++) {
    //    glm::vec4 color;
    //    color.r = rand() / double(RAND_MAX);
    //    color.g = rand() / double(RAND_MAX);
    //    color.b = rand() / double(RAND_MAX);
    //    color.a = 1.0f;
    //    color_map.insert(color_map.end(), color);
    //}
    //model_1.setColor(color_map);


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
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    // Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Shading mode
    shading_mode = PHONG_SHADING_SHADOW;


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

        //lightPos = glm::vec3(cameraPos);

        //change light position over time
        lightPos.x = 5 + sin(glfwGetTime()) * 10.0f;
        lightPos.z = 5 + cos(glfwGetTime()) * 10.0f;
        lightPos.y = 10.0;

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

        /* sort the transparent windows before rendering
         ---------------------------------------------*/
        std::map<float, ModelAttributes> sorted;
        for (unsigned int i = 0; i < model_manager.getAmount(); i++)
        {
            vector<ModelAttributes>* ma = model_manager.getModelAttributes();
            float distance = glm::length(camera.Position - ma->at(i).location);
            sorted[distance] = ma->at(i);
        }



         //render depth of scene to texture (from light's perspective)
         //--------------------------------------------------------------
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        float near_plane = 0.0f, far_plane = 50.5f;
        //lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 0.5, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

        //// reset viewport
        //glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        // floor
        model = glm::mat4(1.0f);
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // floor
        //model = glm::mat4(1.0f);
        //simpleDepthShader.setMat4("model", model);
        //glBindVertexArray(planeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        //// cubes
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
        //model = glm::scale(model, glm::vec3(0.5f));
        //simpleDepthShader.setMat4("model", model);
        //renderCube();
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
        //model = glm::scale(model, glm::vec3(0.5f));
        //simpleDepthShader.setMat4("model", model);
        //renderCube();
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
        //model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
        //model = glm::scale(model, glm::vec3(0.25));
        //simpleDepthShader.setMat4("model", model);
        //renderCube();
        model_1.createShadow(simpleDepthShader, model_1_pos);
        model_2.createShadow(simpleDepthShader, model_2_pos);
        model_cow_m.createShadow(simpleDepthShader, model_cow_pos);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Skybox
        glm::mat4 view_temp = view;
        view = camera.GetViewMatrix();
        skyboxShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skybox.show();
        view = view_temp;

        for (std::map<float, ModelAttributes>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            switch (shading_mode)
            {
                // Constant shading
            case CONSTANT_SHADING:
                constantShader.use();
                constantShader.setVec3("objectColor", 0.87f, 0.87f, 0.87f);
                constantShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                constantShader.setMat4("projection", projection);
                constantShader.setMat4("view", view);
                model = glm::mat4(1.0f);
                model = glm::translate(model, it->second.location);
                constantShader.setMat4("model", model);
                constantShader.setVec3("lightPos", lightPos);
                constantShader.setVec3("viewPos", cameraPos);
                it->second.model->show(constantShader);
                break;
                // Gouraud shading
            case GOURAND_SHADING:
                gouraudShader.use();
                gouraudShader.setVec3("objectColor", 0.87f, 0.87f, 0.87f);
                gouraudShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                gouraudShader.setMat4("projection", projection);
                gouraudShader.setVec3("viewPos", cameraPos);
                gouraudShader.setMat4("view", view);
                model = glm::mat4(1.0f);
                model = glm::translate(model, it->second.location);
                gouraudShader.setMat4("model", model);
                gouraudShader.setVec3("lightPos", lightPos);
                it->second.model->show(gouraudShader);
                break;
                // Phong shading
            case PHONG_SHADING:
                phongShader.use();
                phongShader.setVec3("objectColor", 0.87f, 0.87f, 0.87f);
                phongShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                phongShader.setMat4("projection", projection);
                phongShader.setMat4("view", view);
                model = glm::mat4(1.0f);
                model = glm::translate(model, it->second.location);
                phongShader.setMat4("model", model);
                phongShader.setVec3("lightPos", lightPos);
                it->second.model->show(phongShader);
                break;
            case PHONG_SHADING_SHADOW:
                shadowPhongShader.use();
                shadowPhongShader.setInt("ourTexture", 0);
                shadowPhongShader.setInt("shadowMap", 1);
                shadowPhongShader.setVec3("objectColor", 0.87f, 0.87f, 0.87f);
                shadowPhongShader.setVec3("lightColor", sin(glfwGetTime())/2, cos(glfwGetTime())/2, (1 - sin(glfwGetTime()))/2);
                shadowPhongShader.setMat4("projection", projection);
                shadowPhongShader.setMat4("view", view);
                shadowPhongShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
                shadowPhongShader.setVec3("viewPos", camera.Position);
                model = glm::mat4(1.0f);
                model = glm::translate(model, it->second.location);
                shadowPhongShader.setMat4("model", model);
                shadowPhongShader.setVec3("LightPos", lightPos);
                /////PROBLEM/////////////////////////////////////////////
                //glActiveTexture(GL_TEXTURE1);
                //glBindTexture(GL_TEXTURE_2D, depthMap);
                //////////////////////////////////////////////////
                it->second.model->show(shadowPhongShader, depthMap);

                break;
            default:
                break;
            }
        }
         //--------------------------------------------------------------

        shadowPhongShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        shadowPhongShader.setMat4("projection", projection);
        shadowPhongShader.setMat4("view", view);
        // set light uniforms
        shadowPhongShader.setVec3("viewPos", camera.Position);
        shadowPhongShader.setVec3("lightPos", lightPos);
        shadowPhongShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, depthMap);
            // floor
        model = glm::mat4(1.0f);
        shadowPhongShader.setMat4("model", model);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    vector<ModelAttributes> *ma = model_manager.getModelAttributes();
    for (vector<ModelAttributes>::iterator it = ma->begin(); it != ma->end(); ++it){
        it->model->clear();
    }
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

// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
             1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
             1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
             1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
             1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
             1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
             1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
             1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
             1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
             1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderModel(Model model, Shader shader) {
    model.show(shader);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void renderScene(const Shader& shader)
{
    // floor
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.25));
    shader.setMat4("model", model);
    renderCube();
}