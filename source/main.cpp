#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdexcept>
#include <iostream>

#include "../include/obj_loader.h"
#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/texture.h"
#include "../include/light.h"

int width = 800, height = 600;
float last_x, last_y, delta_time = 0.0f, last_frame_time = 0.0f;;
bool rotation = false, wireframe = false, first_mouse = false;

Camera *camera = new Camera{glm::vec3{0, 1, 4}};
GLFWwindow *window;

Mesh *create_plane();

glm::mat4 compute_shadow_matrix(glm::vec3 light_position, glm::vec3 plane_normal, float d);

glm::mat4 compute_reflection_matrix(glm::vec3 light_position, glm::vec3 plane_normal, glm::vec3 plane_position);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void mouse_callback(GLFWwindow *window, double x, double y);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void upload_lights_and_position(GLuint shader, Light *light);

void uploadMatrices(GLuint shader);

void initialize();

int main(int argc, const char *argv[]) {

    initialize();

    // the plane that will be created is aligned with the xz plane and has (0,1,0) as normal and 0 as value for d
    Mesh *plane = create_plane();
    glm::vec3 plane_position{0.0f, 0.0f, 0.0f};
    glm::vec3 plane_normal{0.0f, 1.0f, 0.0f};
    float d = 0;

    Mesh *earth = ObjLoader::load_obj("../models_and_textures/models/sphere.obj");
    earth->set_model_matrix(glm::translate(glm::mat4{1.0f}, glm::vec3{0, 1, 0}));
    Texture *earth_texture = new Texture{"../models_and_textures/textures/earthmap1k.jpg"};
    Texture *wood_texture = new Texture{"../models_and_textures/textures/wood.jpg"};
    earth->add_texture(earth_texture->get_texture());
    plane->add_texture(wood_texture->get_texture());

    Light *sun = new Light{
            {2.0, 3.0, 0.0f},
            {255.0f / 255.0f, 255.0f / 255.0f, 240.0f / 255.0f},
            {255.0f / 255.0f, 255.0f / 255.0f, 160.0f / 255.0f}, 1};


    Shader sun_shader = Shader{"../shader/sun_shader.vert", "../shader/sun_shader.frag"};
    Shader normal_shader = Shader{"../shader/textured.vert", "../shader/textured.frag"};
    Shader transparent_shader = Shader{"../shader/transparent_shader.vert", "../shader/transparent_shader.frag"};
    Shader shadow_shader = Shader{"../shader/shadow_shader.vert", "../shader/shadow_shader.frag"};

    glm::mat4 earth_model_matrix, light_model_matrix, shadow_projection_matrix;

    // specifies the size of the light
    glPointSize(10);

    // the main loop where the object are drawn
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        // calculate the time it took to comoute the new frame to move the camera in a smoother way depending on the framerate
        delta_time = currentFrame - last_frame_time;
        last_frame_time = currentFrame;
        glfwGetFramebufferSize(window, &width, &height);

        // get the model matrix for the earth and the light so that they can be modified and reset aferwards
        earth_model_matrix = earth->get_model_matrix();
        light_model_matrix = sun->get_model_matrix();

        //upload the view and projection matrix for the shaders that will be used later on
        uploadMatrices(normal_shader.get_program());
        uploadMatrices(transparent_shader.get_program());
        uploadMatrices(sun_shader.get_program());
        uploadMatrices(shadow_shader.get_program());

        // specify the background color
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        // clear color, depth and stencil buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // enable the depth test
        glEnable(GL_DEPTH_TEST);
        // enable stencil test to draw the reflected object only where the reflector is
        glEnable(GL_STENCIL_TEST);

        // Note: if you press the key 0 the light will start to rotate. This might help you to verify your shadows
        // TODO 1: draw the relective plane in the stencil buffer (do not draw in the color or depth buffer yet)
        // Note: glColorMask might help you to avoid drawing in the colorbuffer

        // TODO 3: compute and apply the the reflection matrix to the earth and the sun

        // TODO 4: draw the "mirrored" light source using the sun_shader and the "mirrored" earth using the normal_shader

        // TODO 5: reset the model matrices for the sun and the earth to the "unmirrored" matrices

        // clear the buffer bit to draw the shadow and the plane
        glClear(GL_DEPTH_BUFFER_BIT);

        // TODO 7: compute and apply the shadow projection matrix
        // Note: you can change d to d - 0.001 or you glPolygonOffset to avoid z fighting

        // TODO 8: draw the "shadow" of the earth using the shadow_shader

        // disbale the stencil test to draw the rest of the scene everywhere
        glDisable(GL_STENCIL_TEST);
        // clear the depth bit so that the plane is not drawn over the reflected sphere
        glClear(GL_STENCIL_BUFFER_BIT);

        upload_lights_and_position(normal_shader.get_program(), sun);
        upload_lights_and_position(transparent_shader.get_program(), sun);

        // TODO 9: find a way to draw the plane in a semitransparent way using the transparent_shader
        plane->draw(transparent_shader.get_program());

        // TODO 10: make sure the model matrices are reset to their unmirrored and unshadowed version

        // now we will draw the rest of our scene
        // draw the normal earth
        earth->draw(normal_shader.get_program());
        // draw a point for the sun position
        sun->draw(sun_shader.get_program());


        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // unbinding shader program
    glUseProgram(NULL);
    // window is no longer required
    glfwDestroyWindow(window);
    // finish glfw
    glfwTerminate();
    return 0;
}

// this function is called when a key is pressed
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // if the escape key is pressed the window will close
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // if this key is pressed the cube should start rotating around the y-axis
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        if (!rotation)
            rotation = true;
        else
            rotation = false;
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        if (wireframe) {
            wireframe = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            wireframe = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
    }

    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera->ProcessKeyboard(FORWARD, delta_time * 50);
        }

    }

    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera->ProcessKeyboard(BACKWARD, delta_time * 50);
        }
    }
    //left and right
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera->ProcessKeyboard(RIGHT, delta_time * 50);
        }
    }

    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            camera->ProcessKeyboard(LEFT, delta_time * 50);
        }
    }
}

void mouse_callback(GLFWwindow *window, double x, double y) {
    if (first_mouse) {
        last_x = x;
        last_y = y;
        first_mouse = false;
    }

    float xoffset = x - last_x;
    float yoffset = last_y - y; // reversed since y-coordinates go from bottom to top

    glfwSetCursorPos(window, last_x, last_y);
    camera->ProcessMouseMovement(xoffset, yoffset);
}

// this function is called when the window is resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

// this function uploads the model, view and projection matrix to the shader if they are defined in the shader
void uploadMatrices(GLuint shader) {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), width / (float) height, 0.1f, 10000.0f);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &view[0][0]);
}

//this function uploads values to the shader for further computation
void upload_lights_and_position(GLuint shader, Light *light) {
    if (rotation) {
        glm::mat4 new_model_matrix = glm::rotate(light->get_model_matrix(), 0.0001f, glm::vec3{0, 1, 0});
        light->set_model_matrix(new_model_matrix);
    }

    glUseProgram(shader);
    int light_position_location = glGetUniformLocation(shader, "light_position");
    int light_diffuse_color_location = glGetUniformLocation(shader, "light_diffuse_color");
    int camera_position_location = glGetUniformLocation(shader, "camera_position");

    glm::vec3 position = light->get_computed_position();
    glUniform3f(light_position_location, position.x, position.y, position.z);
    glUniform3f(light_diffuse_color_location, light->diffuse_color.x, light->diffuse_color.y, light->diffuse_color.z);
    glUniform3f(camera_position_location, camera->Position.x, camera->Position.y, camera->Position.z);
}

void initialize() {
    // initialize the GLFW library to be able create a window
    if (!glfwInit()) {
        throw std::runtime_error("Couldn't init GLFW");
    }

    // set the opengl version
    int major = 3;
    int minor = 3;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // create the window
    window = glfwCreateWindow(width, height, "Lab 3", NULL, NULL);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Couldn't create a window");
    }

    // set the window to the current context so that it is used
    glfwMakeContextCurrent(window);
    // set the frameBufferSizeCallback so that the window adjusts if it is scaled
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // set the keyCallback function so that keyboard input can be used
    glfwSetKeyCallback(window, key_callback);
    // set the mouse callback so that mouse input can be used
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // try to initialise glew to be able to use opengl commands
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();

    if (err != GLEW_OK) {
        glfwTerminate();
        throw std::runtime_error(
                std::string("Could initialize GLEW, error = ") + (const char *) glewGetErrorString(err));
    }

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported " << version << std::endl;


    // opengl configuration
    glEnable(GL_DEPTH_TEST);        // enable depth-testing
    glDepthFunc(GL_LESS);           // depth-testing interprets a smaller value as "closer"
    glfwSwapInterval(false); // disables VSYNC
}

// TODO 6: implement the computation of the shadow matrix
glm::mat4 compute_shadow_matrix(glm::vec3 light_position, glm::vec3 plane_normal, float d) {
    glm::mat4 matrix{1.0f};
    return matrix;
}

// TODO 2: implement the computation of the reflection matrix
// relfection matrix from: https://community.arm.com/developer/tools-software/graphics/b/blog/posts/combined-reflections-stereo-reflections-in-vr used
glm::mat4 compute_reflection_matrix(glm::vec3 light_position, glm::vec3 plane_normal, glm::vec3 plane_position) {
    glm::mat4 matrix{1.0f};
    return matrix;
}

Mesh *create_plane() {
    std::vector<Vertex> vertices;
    vertices.emplace_back(glm::vec3{2.0f, 0.0f, -2.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 1.0f});
    vertices.emplace_back(glm::vec3{-2.0f, 0.0f, -2.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
    vertices.emplace_back(glm::vec3{-2.0f, 0.0f, 2.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 0.0f});
    vertices.emplace_back(glm::vec3{2.0f, 0.0f, -2.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{0.0f, 1.0f});
    vertices.emplace_back(glm::vec3{-2.0f, 0.0f, 2.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 0.0f});
    vertices.emplace_back(glm::vec3{2.0f, 0.0f, 2.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec2{1.0f, 1.0f});
    return new Mesh{vertices};
}