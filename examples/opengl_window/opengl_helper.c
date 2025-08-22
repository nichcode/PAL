
#include "opengl_helper.h"

static const char* vertexShaderSource = {
    "#version 330 core\n"

    "layout(location = 0) in vec3 aPosition;\n"
    "layout(location = 1) in vec4 aColor;\n"

    "layout(location = 1) out vec4 vColor;\n"
    
    "void main() {\n" 
        "vColor = aColor;\n"
        "gl_Position = vec4(aPosition, 1.0);\n"
    "}\n"
};

static const char* fragmentShaderSource = {

    "#version 330 core\n"

    "layout(location = 1) in vec4 vColor;\n"

    "layout(location = 0) out vec4 color;\n"
    
    "void main() {\n" 
        "color = vColor;\n"
    "}\n"
};

bool initOpengl(
    PalEventDriver** outEventDriver,
    PalWindow** outWindow) {

    PalResult result;
    result = palLoadGLICD();
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return false;
    }

    PalEventDriver* eventDriver = nullptr;
    PalEventDriverCreateInfo eventDriverCreateInfo;
    eventDriverCreateInfo.allocator = nullptr;
    eventDriverCreateInfo.callback = nullptr;
    eventDriverCreateInfo.queue = nullptr; // use default
    eventDriverCreateInfo.userData = nullptr; // not using callback mode
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return false;
    }

    // we listen for window button click to close the application
    palSetEventDispatchMode(
        eventDriver, 
        PAL_EVENT_WINDOW_CLOSE, 
        PAL_DISPATCH_POLL
    );

    // we listen for window resized to resize the opengl context
    palSetEventDispatchMode(
        eventDriver, 
        PAL_EVENT_WINDOW_RESIZE, 
        PAL_DISPATCH_POLL
    );

    // you can create the window using SDL, GLFW, PAL or the OS
    // if PAL video won't be used, then the video system initialization can be skipped
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        // this can made into a goto to decrease this result checks
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return false;
    }

    PalWindow* window = nullptr;
    PalWindowCreateInfo windowCreateInfo;
    windowCreateInfo.display = nullptr; // use primary display
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.title = "GL Window";
    windowCreateInfo.flags = PAL_WINDOW_CENTER | PAL_WINDOW_DEFAULT;
    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return false;
    }

    *outWindow = window;
    *outEventDriver = eventDriver;
    return true;
}

bool createContext(
    PalWindow* window,
    PalGLPixelFormat* format,
    PalGLContext** outContext) {

    PalResult result;
    PalGLContext* context = nullptr;
    PalGLInfo info = palGetGLInfo();

    PalGLContextCreateInfo createInfo;
    createInfo.allocator = nullptr;
    createInfo.debug = true;
    createInfo.format = format; // nullptr to use default
    createInfo.major = info.versionMajor;
    createInfo.minor = info.versionMinor;
    createInfo.noError = false;
    createInfo.release = PAL_GL_RELEASE_BEHAVIOR_NONE;
    createInfo.reset = PAL_GL_CONTEXT_RESET_NONE; // set if robustness will be true
    createInfo.robustness = false;
    createInfo.windowHandle = palGetWindowHandle(window);

    if (info.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        createInfo.forward = true;
    }

    if (info.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        createInfo.profile = PAL_GL_PROFILE_CORE;
    }

    result = palCreateGLContext(&createInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return false;
    }

    // make the context current
    result = palMakeContextCurrent(context);
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return false;
    }

    // you should initlaized your loadder to load opengl functions (glad, glew etc)
    Int32 status = gladLoadGL();
    if (!status) {
        palLog("Failed to initialize Glad!");
        return false;
    }

    *outContext = context;
    return true;
}

void shutdownOpengl(
    PalEventDriver* eventDriver,
    PalWindow* window,
    PalGLContext* context) {

    palDestroyGLContext(context);
    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);
}

void createTriangle(
    Uint32* vao, 
    Uint32* vbo, 
    Uint32* ibo, 
    Uint32* program) {
    
    glViewport(0, 0, 640, 480);
    Uint32 glVao, glVbo, glIbo, vertexShader, fragmentShader, glProgram;

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    Uint32 indices[3] = { 0, 1, 2 };

    // create vertex array
    glGenVertexArrays(1, &glVao);
    glBindVertexArray(glVao);

    // create vertex buffer
    glGenBuffers(1, &glVbo);
    glBindBuffer(GL_ARRAY_BUFFER, glVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create index buffer
    glGenBuffers(1, &glIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set vertex attributes
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);

    // color
    Uint64 offset = 3 * sizeof(float); // the size of elements (position is 3 elemets) to bypass before we get to color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)offset);
    
    // create shaders
    Int32 status = GL_FALSE;
    Int32 max_length = 0;
    char errorMsg[512] = {};

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &max_length);

    if (status != GL_TRUE) {
        glGetShaderInfoLog(vertexShader, 512, &max_length, errorMsg);
        palLog(errorMsg);
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &max_length);

    if (status != GL_TRUE) {
        glGetShaderInfoLog(fragmentShader, 512, &max_length, errorMsg);
        palLog(errorMsg);
    }

    glProgram = glCreateProgram();
    glAttachShader(glProgram, vertexShader);
    glAttachShader(glProgram, fragmentShader);
    glLinkProgram(glProgram);

    glValidateProgram(glProgram);
    glGetProgramiv(glProgram, GL_LINK_STATUS, &status);
    glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &max_length);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(fragmentShader, 512, &max_length, errorMsg);
        palLog(errorMsg);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    *vao = glVao;
    *vbo = glVbo;
    *ibo = glIbo;
    *program = glProgram;
}

void drawTriangle(
    Uint32 vao, 
    Uint32 program) {

    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
}

void destroyTriangle(
    Uint32* vao, 
    Uint32* vbo, 
    Uint32* ibo, 
    Uint32* program) {

    glDeleteBuffers(1, vbo);
    glDeleteBuffers(1, ibo);
    glDeleteVertexArrays(1, vao);
    glDeleteProgram(*program);
}