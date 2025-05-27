// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#include <memory>
#include <filesystem>
#include <iostream>
#include <vector>
#include <functional>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/callbacks/Event.h>

struct GLFWwindow;

namespace engine::context
{
    namespace fs = std::filesystem;

    class Context
    {
    public:
        /* Events */
        using event_void_t = typename callbacks::Event<Context>;
        using key_input_event_t = typename callbacks::Event<Context, const GLFWwindow*, const int, const int, const int, const int>;
        using controller_input_event_t = typename callbacks::Event<Context, const int, const GLFWgamepadstate&>;
        using mouse_move_event_t = typename callbacks::Event<Context, const double, const double>;
        using mouse_input_event_t = typename callbacks::Event<Context, const int, const int, const int>;
        using mouse_scroll_event_t = typename callbacks::Event<Context, const double, const double>;
        using mouse_catch_event_t = typename callbacks::Event<Context, GLFWwindow*, const bool>;

        friend GLFWwindow;

        explicit Context(fs::path assetRoot = "./assets", int width = 1920.0f, int height = 1080.0f,
                         const char* title = "GameLab III");

        ~Context();

        void run(const std::function<void(Context*)>& updateCallback);

        [[nodiscard]] GLFWwindow* getWindow() const;

        [[nodiscard]] float getDeltaTime() const;

        [[nodiscard]] unsigned int getWidth() const;

        [[nodiscard]] unsigned int getHeight() const;

        [[nodiscard]] float getAspect() const;

    private:
        void initialize();

        void beginFrame();

        void endFrame();

        void tick(const std::function<void(Context*)>& updateCallback);

        void draw();

        void fireKeyInputEvent(const GLFWwindow* glfWwindow, int key, int scancode, int action, int mods);

        void fireMouseMoveEvent(double xPos, double yPos);

        void fireMouseInputEvent(int button, int action, int mods);

        void fireMouseScrollEvent(double xOffset, double yOffset);

        void fireMouseCatchEvent(GLFWwindow* glfWwindow, bool bEntered);

        void processControllerInput();

    public:
        event_void_t event_newFrame;

        event_void_t event_endFrame;

        event_void_t event_beginPlay;

        event_void_t event_draw;

        key_input_event_t event_keyInput;

        controller_input_event_t event_controllerInput;

        mouse_move_event_t event_mouseMove;

        mouse_input_event_t event_mouseInput;

        mouse_scroll_event_t event_mouseScroll;

        mouse_catch_event_t event_mouseCatch;

    private:
        std::filesystem::path assetRoot;

        GLFWwindow* glfwWindow;

        float deltaTime;

        float lastTime;

        unsigned int windowHeight;

        unsigned int windowWidth;
    };
}
