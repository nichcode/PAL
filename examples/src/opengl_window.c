
#include "pal/pal_video.h"
#include "pal/pal_opengl.h"

#include "glad/glad.h"

Uint32 g_Vao, g_Vbo, g_Ibo;
Uint32 g_VertexShader, g_FragmentShader, g_Program;

const char* vertexShaderSource = {
    "#version 330 core\n"

    "layout(location = 0) in vec3 aPosition;\n"
    "layout(location = 1) in vec4 aColor;\n"

    "layout(location = 1) out vec4 vColor;\n"
    
    "void main() {\n" 
        "vColor = aColor;\n"
        "gl_Position = vec4(aPosition, 1.0);\n"
    "}\n"
};

const char* fragmentShaderSource = {

    "#version 330 core\n"

    "layout(location = 1) in vec4 vColor;\n"

    "layout(location = 0) out vec4 color;\n"
    
    "void main() {\n" 
        "color = vColor;\n"
    "}\n"
};

void createTriangle();
void createShaders();
void drawTriangle();
void destroyTriangle();

int main(int argc, char** argv) {

    const char* versionString = palGetVersionString();
    palLog("PAL version - %s", versionString);

    PalResult result;
    result = palLoadGLICD();
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    // create an event driver to recieve events
    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo;
    eventDriverCreateInfo.allocator = nullptr;
    eventDriverCreateInfo.callback = nullptr;
    eventDriverCreateInfo.queue = nullptr; // use default
    eventDriverCreateInfo.userData = nullptr; // not using callback mode
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    // we listen for window button click to close the application
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);

    // we listen for window resized to resize the opengl context
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_RESIZE, PAL_DISPATCH_POLL);

    // you can create the window using SDL, GLFW, PAL or the OS
    // if PAL video won't be used, then the video system initialization can be skipped
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo windowCreateInfo;
    windowCreateInfo.display = nullptr; // use primary display
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.title = "Pal GL Context Test";
    windowCreateInfo.flags = PAL_WINDOW_CENTER | PAL_WINDOW_DEFAULT;

    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    // get the native window handle
    void* nativeWindow = palGetWindowHandle(window);

    Int32 count = 0;
    result = palEnumerateGLPixelFormats(nativeWindow, &count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    palLog("GL Pixel Format count - %d", count);

    PalGLPixelFormat* formats = palAllocate(
        nullptr,
        sizeof(PalGLPixelFormat) * count,
        0
    );

    if (!formats) {
        return -1;
    }

    result = palEnumerateGLPixelFormats(nativeWindow, &count, formats);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    PalGLPixelFormat desired = {};
    desired.redBits = 8;
    desired.greenBits = 8;
    desired.blueBits = 8;
    desired.alphaBits = 8;
    desired.alphaBits = 8;
    desired.depthBits = 24;
    desired.stencilBits = 8;
    desired.samples = 4;

    desired.stereo = false;
    desired.sRGB = true;
    desired.doubleBuffer = true;

    // get the closest pixel format
    PalGLPixelFormat* closest = palGetClosestGLPixelFormat(
        formats,
        count,
        &desired
    );

    if (!closest) {
        desired.samples = 1;
        desired.sRGB = false;
    }

    // get the closest pixel format with the new desired format
    closest = palGetClosestGLPixelFormat(
        formats,
        count,
        &desired
    );

    if (!closest) {
        palLog("Failed to get closest pixel format");
        return -1;
    }

    // create opengl context
    PalGLContext* context = nullptr;
    PalGLInfo info = palGetGLInfo();

    PalGLContextCreateInfo contextCreateInfo;
    contextCreateInfo.allocator = nullptr;
    contextCreateInfo.debug = true;
    contextCreateInfo.format = closest;
    contextCreateInfo.major = info.versionMajor;
    contextCreateInfo.minor = info.versionMinor;
    contextCreateInfo.noError = false;
    contextCreateInfo.release = PAL_GL_RELEASE_BEHAVIOR_NONE;
    contextCreateInfo.reset = PAL_GL_CONTEXT_RESET_NONE; // set if robustness will be true
    contextCreateInfo.robustness = false;
    contextCreateInfo.windowHandle = nativeWindow;

    if (info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        contextCreateInfo.forward = true;
    }

    if (info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        contextCreateInfo.profile = PAL_GL_PROFILE_CORE;
    }

    palCreateGLContext(&contextCreateInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    // make the context current
    palMakeContextCurrent(context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return -1;
    }

    // set vsync
    palSetGLContextVsync(context, true);

    // you should initlaized your loadder to load opengl functions (glad, glew etc)
    Int32 status = gladLoadGL();
    if (!status) {
        palLog("Failed to initialize Glad!");
        return -1;
    }

    // create the vao, vbo and ibo of the triangle
    createTriangle();
    createShaders();

    // set the clear color
    glClearColor(.2f, .2f, .2f, 1.0f);

    bool running = true;
    while (running) {
        palUpdateVideo();

        PalEvent event;
        while (palPollEvent(eventDriver, &event)) {
            switch (event.type) {
                case PAL_EVENT_WINDOW_CLOSE: {
                    running = false;
                    break;
                }

                case PAL_EVENT_WINDOW_RESIZE: {
                    Uint32 width, height;
                    palUnpackUint32(event.data, &width, &height);
                    glViewport(0, 0, width, height);
                    break;
                }
            }
        }

        // draw the triangle
        glClear(GL_COLOR_BUFFER_BIT);
        drawTriangle();

        palSwapBuffers(context);
        if (result != PAL_RESULT_SUCCESS) {
            const char* resultString = palResultToString(result);
            palLog("PAL error - %s", resultString);
            return -1;
        }
    }

    destroyTriangle();
    palDestroyGLContext(context);
    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);

    palFree(nullptr, formats);
    return 0;
}

void createTriangle() {

    glViewport(0, 0, 640, 480);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    Uint32 indices[3] = { 0, 1, 2 };

    // create vertex array
    glGenVertexArrays(1, &g_Vao);
    glBindVertexArray(g_Vao);

    // create vertex buffer
    glGenBuffers(1, &g_Vbo);
    glBindBuffer(GL_ARRAY_BUFFER, g_Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create index buffer
    glGenBuffers(1, &g_Ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_Ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set vertex attributes
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);

    // color
    Uint64 offset = 3 * sizeof(float); // the size of elements (position is 3 elemets) to bypass before we get to color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)offset);
}

void drawTriangle() {

    glUseProgram(g_Program);
    glBindVertexArray(g_Vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
}

void createShaders() {

    Int32 status = GL_FALSE;
    Int32 max_length = 0;
    char errorMsg[512] = {};

    g_VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(g_VertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(g_VertexShader);

    glGetShaderiv(g_VertexShader, GL_COMPILE_STATUS, &status);
    glGetShaderiv(g_VertexShader, GL_INFO_LOG_LENGTH, &max_length);

    if (status != GL_TRUE) {
        glGetShaderInfoLog(g_VertexShader, 512, &max_length, errorMsg);
        palLog(errorMsg);
    }

    g_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_FragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(g_FragmentShader);

    glGetShaderiv(g_FragmentShader, GL_COMPILE_STATUS, &status);
    glGetShaderiv(g_FragmentShader, GL_INFO_LOG_LENGTH, &max_length);

    if (status != GL_TRUE) {
        glGetShaderInfoLog(g_FragmentShader, 512, &max_length, errorMsg);
        palLog(errorMsg);
    }

    g_Program = glCreateProgram();
    glAttachShader(g_Program, g_VertexShader);
    glAttachShader(g_Program, g_FragmentShader);
    glLinkProgram(g_Program);

    glValidateProgram(g_Program);
    glGetProgramiv(g_Program, GL_LINK_STATUS, &status);
    glGetProgramiv(g_Program, GL_INFO_LOG_LENGTH, &max_length);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(g_FragmentShader, 512, &max_length, errorMsg);
        palLog(errorMsg);
    }

    glDeleteShader(g_VertexShader);
    glDeleteShader(g_FragmentShader);
}

void destroyTriangle() {
    
    glDeleteBuffers(1, &g_Vbo);
    glDeleteBuffers(1, &g_Ibo);
    glDeleteVertexArrays(1, &g_Vao);
    glDeleteProgram(g_Program);
}