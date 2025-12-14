

/*
*
*
*
*
*
*
*
هذا الكود تم تطبيقه من كلاسات وشادرز المحاضرة الخامسة
تم فقط استخدام والتعديل على شادرز
L5.vs & L5-Model.fs
(وتم اضافة 3 موديلات للشمس والارض والقمر (جميعها نفس الموديل مع تغيير الحجم والصورة فقط



*/













#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

using namespace glm;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

Camera camera(vec3(0.0f, 5.0f, 40.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float simTime = 0.0f;
float timeSpeed = 1.0f;

bool solarPaused = false;
bool lunarPaused = false;
bool pauseMotion = false;
bool solarEclipseMode = false;
bool lunarEclipseMode = false;

vec3 lightPos(0.0f, 0.0f, 0.0f);

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


//ميثود لمعرفة اذا كانت الاجرام عللى استقامة  واحد وما اذا كان خسوفا او كسوفا
bool isMiddle(vec3 start, vec3 middle, vec3 end)
{
    vec3 dir = end - start;
    float proj = dot(middle - start, normalize(dir));
    float total = length(dir);
    return proj > 0.0f && proj < total && length(cross(normalize(middle - start), normalize(dir))) < 0.0001f;
}

int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lecture 5", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/L5-Model.fs");

    glEnable(GL_DEPTH_TEST);

    Model sunModel("./models/sun/planet.obj");
    Model earthModel("./models/earth/planet.obj");
    Model moonModel("./models/moon/planet.obj");

    bool BPressed = false;

    while (!glfwWindowShouldClose(window))
    {
        allShader.use();

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        mat4 earthTrans = mat4(1.0f);
        float width = 30.0f;
        float height = 18.0f;

        //تم استخدام الساين والكوساين لجعل المدار بيضوي
        float orbitX = cos(simTime * 0.3f) * width;
        float orbitZ = sin(simTime * 0.3f) * height;

        earthTrans = translate(earthTrans, vec3(orbitX, 0.0f, orbitZ));
        earthTrans = rotate(earthTrans, simTime * 2.0f, vec3(0.0f, 1.0f, 0.0f));
        earthTrans = scale(earthTrans, vec3(0.3f));
        vec3 earthPos = vec3(earthTrans[3]);

        mat4 moonTrans = earthTrans;
        float moonOrbitRadius = 15.0f;
        moonTrans = rotate(moonTrans, simTime * 3.0f, vec3(0.0f, 1.0f, 0.0f));
        moonTrans = translate(moonTrans, vec3(moonOrbitRadius, 0.0f, 0.0f));
        moonTrans = scale(moonTrans, vec3(0.5f));
        vec3 moonPos = vec3(moonTrans[3]);

        //ميثودز للتحقق من حدوث كسوف او خسوف عند الضغط على حرف الجي أو الاتش
        if (solarEclipseMode) {
            solarPaused = isMiddle(lightPos, moonPos, earthPos);
            pauseMotion = solarPaused;
            timeSpeed = solarPaused ? 0.0f : 5.0f;
        }

        if (lunarEclipseMode) {
            lunarPaused = isMiddle(lightPos, earthPos, moonPos);
            pauseMotion = lunarPaused;
            timeSpeed = lunarPaused ? 0.0f : 5.0f;
        }

        if (!solarEclipseMode && !lunarEclipseMode) {
            pauseMotion = false;
            timeSpeed = 1.0f;
        }

        if (!pauseMotion)
            simTime += deltaTime * timeSpeed;


        glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        allShader.setMat4("projection", projection);

        mat4 view = camera.GetViewMatrix();
        allShader.setMat4("view", view);

        allShader.setVec3("lightPos", lightPos);
        allShader.setVec3("viewPos", camera.Position);
        allShader.setVec3("objectColor", vec3(3.0f));

        allShader.setBool("isSun", true);
        allShader.setMat4("model", mat4(1.0f));
        sunModel.Draw(allShader);

        allShader.setBool("isSun", false);
        allShader.setVec3("moonPos", moonPos);
        allShader.setVec3("moonColor", vec3(0.15f));
        allShader.setMat4("model", earthTrans);
        earthModel.Draw(allShader);

        allShader.setMat4("model", moonTrans);
        moonModel.Draw(allShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    float speed = deltaTime * 10.0f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, speed);

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        timeSpeed = 3.0f;
        solarEclipseMode = true;
        lunarEclipseMode = false;
        pauseMotion = false;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        timeSpeed = 3.0f;
        lunarEclipseMode = true;
        solarEclipseMode = false;
        pauseMotion = false;
    }

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        timeSpeed = 1.0f;
        pauseMotion = false;
        solarEclipseMode = false;
        lunarEclipseMode = false;
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
