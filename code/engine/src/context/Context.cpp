// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#include <engine/context/Context.h>

using namespace engine::context;
const int SAMPLE_AMOUNT = 4;

Context::Context(fs::path assetRoot, const int width, const int height, const char* title)
        : assetRoot(std::move(assetRoot)), windowWidth(width), windowHeight(height), deltaTime(0), lastTime(0)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Make sure we get an OpenGL 4.6 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, SAMPLE_AMOUNT);

    // Create a windowed mode window and its OpenGL context
    glfwWindow = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), nullptr);
    if (!glfwWindow)
    {
        glfwTerminate();
        throw std::runtime_error("failed to create window");
    }

    glfwSetWindowUserPointer(glfwWindow,  this);

    auto key_input_callback = [](GLFWwindow* w, int k, int s, int a, int m)
    {
        static_cast<Context*>(glfwGetWindowUserPointer(w))->fireKeyInputEvent(w, k, s, a, m);
    };

    // Enable sticky keys to not miss key events
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetKeyCallback(glfwWindow, key_input_callback);

    auto mouse_move_callback = [](GLFWwindow* w, double xPos, double yPos)
    {
        static_cast<Context*>(glfwGetWindowUserPointer(w))->fireMouseMoveEvent(xPos, yPos);
    };

    auto mouse_input_callback = [](GLFWwindow* w,  int button, int action, int mods)
    {
        static_cast<Context*>(glfwGetWindowUserPointer(w))->fireMouseInputEvent(button, action, mods);
    };

    auto mouse_scroll_callback = [](GLFWwindow* w, double xOffset, double yOffset)
    {
        static_cast<Context*>(glfwGetWindowUserPointer(w))->fireMouseScrollEvent(xOffset, yOffset);
    };

    auto mouse_catch_callback = [](GLFWwindow* w, int entered)
    {
        static_cast<Context*>(glfwGetWindowUserPointer(w))->fireMouseCatchEvent(w, entered);
    };

    glfwSetInputMode(glfwWindow, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetCursorPosCallback(glfwWindow, mouse_move_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_input_callback);
    glfwSetScrollCallback(glfwWindow, mouse_scroll_callback);
    glfwSetCursorEnterCallback(glfwWindow, mouse_catch_callback);

    // Make the window's context current
    glfwMakeContextCurrent(glfwWindow);

    // Load GL Extensions
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    std::cout << "gpu: " << glGetString(GL_RENDERER) << std::endl
              << "opengl version: " << glGetString(GL_VERSION) << std::endl;


    // Open GL options
    {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_MULTISAMPLE);
    }

    if (glGetError() != GL_NO_ERROR)
    {
        throw std::runtime_error("gl error");
    }
}

Context::~Context()
{
    glfwTerminate();
}

void Context::initialize()
{
    // Frame time (initialize to sane values)
    glfwSetTime(1.0 / 60);
    lastTime = 0;
    deltaTime = 0;
}

void Context::beginFrame()
{
    event_newFrame.fire();
}

void Context::tick(const std::function<void(Context*)>& updateCallback)
{
    updateCallback(this);
}

void Context::endFrame()
{
    // Poll for and process events
    glfwPollEvents();

    processControllerInput();

    // Swap front and back buffers
    glfwSwapBuffers(glfwWindow);

    event_endFrame.fire();
}

void Context::draw()
{
    event_draw.fire();
}

void Context::run(const std::function<void(Context*)>& updateCallback)
{
    initialize();

    event_beginPlay.fire();

    // Update Loop
    while (!glfwWindowShouldClose(glfwWindow))
    {
        // Update frame time
        deltaTime = (float) (glfwGetTime() - lastTime);
        lastTime = (float) glfwGetTime();

        beginFrame();

        tick(updateCallback);

        draw();

        endFrame();
    }
}

GLFWwindow* Context::getWindow() const
{
    return glfwWindow;
}

float Context::getDeltaTime() const
{
    return deltaTime;
}

unsigned int Context::getWidth() const
{
    return windowWidth;
}

unsigned int Context::getHeight() const
{
    return windowHeight;
}

float Context::getAspect() const
{
    return ((float) windowWidth) / ((float) windowHeight);
}

void Context::fireKeyInputEvent(const GLFWwindow* glfWwindow, const int key, const int scancode, const int action, const int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(glfwWindow, true);
    
    //  TODO: Remove glfw window from the payload
    event_keyInput.fire(glfWwindow, key, scancode, action, mods);
}

void Context::fireMouseMoveEvent(const double xPos, const double yPos)
{
    event_mouseMove.fire(xPos, yPos);
}

void Context::fireMouseInputEvent(const int button, const int action, const int mods)
{
    event_mouseInput.fire(button, action, mods);
}


void Context::fireMouseScrollEvent(const double xOffset, const double yOffset)
{
    event_mouseScroll.fire(xOffset, yOffset);
}

void Context::fireMouseCatchEvent(GLFWwindow* glfWwindow, const bool bEntered)
{
    event_mouseCatch.fire(glfWwindow, bEntered);
}

void Context::processControllerInput()
{
    int controllerID;

    if(glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
    {
        controllerID = GLFW_JOYSTICK_1;

        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
            event_controllerInput.fire(controllerID, state);
    }

    if(glfwJoystickIsGamepad(GLFW_JOYSTICK_2) )
    {
        controllerID = GLFW_JOYSTICK_2;

        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_2, &state))
            event_controllerInput.fire(controllerID, state);
    }

    if(glfwJoystickIsGamepad(GLFW_JOYSTICK_3))
    {
        controllerID = GLFW_JOYSTICK_3;

        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_3, &state))
            event_controllerInput.fire(controllerID, state);
    }

    if(glfwJoystickIsGamepad(GLFW_JOYSTICK_4))
    {
        controllerID = GLFW_JOYSTICK_4;

        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_4, &state))
            event_controllerInput.fire(controllerID, state);
    }
}
