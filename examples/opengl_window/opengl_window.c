
#include "pal/pal_opengl.h"
#include "pal/pal_video.h"

#include "glad/glad.h"

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

int main(int argc, char** argv) {

    const char* versionString = palGetVersionString();
    palLog("PAL version - %s", versionString);

    PalResult result;
    PalWindow* window = nullptr;
    PalEventDriver* eventDriver = nullptr;
    PalGLContext* context = nullptr;

    Int32 count = 0;
    PalGLPixelFormat* formats = nullptr;

    Uint32 vao, vbo, ibo;
    Uint32 vertexShader, fragmentShader, program;

    // load the opengl ICD
    result = palLoadGLICD();
    if (result != PAL_RESULT_SUCCESS) {
        const char* resultString = palResultToString(result);
        palLog("PAL error - %s", resultString);
        return false;
    }

    // event driver create info
    PalEventDriverCreateInfo eventDriverCreateInfo;
    eventDriverCreateInfo.allocator = nullptr;
    eventDriverCreateInfo.callback = nullptr;
    eventDriverCreateInfo.queue = nullptr; // use default
    eventDriverCreateInfo.userData = nullptr; // not using callback mode

    // create an event driver to recieve video system events
    result = palCreateEventDriver(&eventDriverCreateInfo, &eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // you can create the window using SDL, GLFW, PAL or the OS
    // if PAL video won't be used, then the video system initialization can be skipped
    result = palInitVideo(nullptr, eventDriver);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // window create info
    PalWindowCreateInfo windowCreateInfo = {};
    windowCreateInfo.display = nullptr; // use primary display
    windowCreateInfo.width = 640;
    windowCreateInfo.height = 480;
    windowCreateInfo.title = "Window";
    windowCreateInfo.flags = PAL_WINDOW_CENTER | PAL_WINDOW_DEFAULT;

    // create the window with the create info
    result = palCreateWindow(&windowCreateInfo, &window);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // get the GL driver info
    PalGLInfo glDriverInfo = palGetGLInfo();

    // enumerate through the available formats and choose one
    void* windowHandle = palGetWindowHandle(window);
    result = palEnumerateGLPixelFormats(windowHandle, &count, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    palLog("GL Pixel Format count - %d", count);
    formats = palAllocate(nullptr, sizeof(PalGLPixelFormat) * count, 0);

    if (!formats) {
        palLog("Out of memory");
        goto Failed;
    }

    result = palEnumerateGLPixelFormats(windowHandle, &count, formats);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // find the closest formats from the enumerated formats with a desired format
    PalGLPixelFormat desiredFormat = {};
    desiredFormat.redBits = 8;
    desiredFormat.greenBits = 8;
    desiredFormat.blueBits = 8;
    desiredFormat.alphaBits = 8;
    desiredFormat.alphaBits = 8;
    desiredFormat.depthBits = 24;
    desiredFormat.stencilBits = 8;
    desiredFormat.samples = 4;

    desiredFormat.stereo = false;
    desiredFormat.sRGB = true;
    desiredFormat.doubleBuffer = true;

    // get the closest pixel format thats matches your desired format
    PalGLPixelFormat* closest = palGetClosestGLPixelFormat(
        formats,
        count,
        &desiredFormat
    );

    // gl context create info
    PalGLContextCreateInfo createInfo = {};
    createInfo.allocator = nullptr;
    createInfo.share = nullptr; // set to share context
    createInfo.debug = true;
    createInfo.format = closest; // nullptr to use default
    createInfo.major = glDriverInfo.versionMajor;
    createInfo.minor = glDriverInfo.versionMinor;
    createInfo.noError = false;
    createInfo.release = PAL_GL_RELEASE_BEHAVIOR_NONE; // this might not be supported. check with extensions
    createInfo.reset = PAL_GL_CONTEXT_RESET_NONE; // set if robustness will be true
    createInfo.robustness = false;
    createInfo.windowHandle = palGetWindowHandle(window); // the window handle (HWND for win32 as void*)

    if (glDriverInfo.extensions & PAL_GL_EXTENSION_CREATE_CONTEXT) {
        // create a forward compatible context if supported
        createInfo.forward = true;
    }

    if (glDriverInfo.extensions & PAL_GL_EXTENSION_CONTEXT_PROFILE) {
        // create a context with core profile if supported
        createInfo.profile = PAL_GL_PROFILE_CORE;
    }

    // create the context
    result = palCreateGLContext(&createInfo, &context);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // make the context current
    result = palMakeContextCurrent(context);
    if (result != PAL_RESULT_SUCCESS) {
        goto Failed;
    }

    // we listen to window resize and window close events 
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_CLOSE, PAL_DISPATCH_POLL);
    palSetEventDispatchMode(eventDriver, PAL_EVENT_WINDOW_RESIZE, PAL_DISPATCH_POLL);

    // you should initlaized your loadder to load opengl functions (glad, glew etc)
    Int32 status = gladLoadGL();
    if (!status) {
        palLog("Failed to initialize Glad!");
        goto Failed;
    }

    // set the clear color
    glClearColor(.2f, .2f, .2f, 1.0f);

    // create a triangle using GL
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    };

    Uint32 indices[3] = { 0, 1, 2 };

    // create vertex array
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // create vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create index buffer
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set vertex attributes
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);

    // color
    // the size of elements (position is 3 elemets) to bypass before we get to color
    Uint64 offset = 3 * sizeof(float); 
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)offset);
    
    // create shaders
    // vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // fragment shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // create shader program
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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
                    PalResizeEventInfo info = palGetResizeEventInfo(&event);
                    glViewport(0, 0, info.width, info.height);
                    break;
                }
            }
        }

        // draw the triangle
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        result = palSwapBuffers(context);
        if (result != PAL_RESULT_SUCCESS) {
            goto Failed;
        }
    }

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    palDestroyGLContext(context);
    palDestroyWindow(window);
    palShutdownVideo();
    palDestroyEventDriver(eventDriver);

    palFree(nullptr, formats);
    return 0;

    // a goto for failure. No need to duplicate that result check
    Failed:
        if (vbo) {
            glDeleteBuffers(1, &vbo);
        }

        if (ibo) {
            glDeleteBuffers(1, &ibo);
        }

        if (vao) {
            glDeleteVertexArrays(1, &vao);
        }

        if (program) {
            glDeleteProgram(program);
        }

        if (context) {
            palDestroyGLContext(context);
        }

        if (window) {
            palDestroyWindow(window);
        }

        palShutdownVideo();
        if (eventDriver) {
            palDestroyEventDriver(eventDriver);
        }

        if (formats) {
            palFree(nullptr, formats);
        }
        
        const char* resultString = palResultToString(result);
        palLog("PAL Error - %s", resultString);
        return -1;
}