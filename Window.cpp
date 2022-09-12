#include "Window.h"
#include <glm/gtc/type_ptr.hpp>
#include "skybox.h"
#include "Sphere.h"
#include "geometry.h"
#include "transform.h"
#include "astronaut.h"
#include "plane.h"
#include "bound.h"

// Window Properties
int window::width;
int window::height;
const char* window::window_title = "GLFW Starter Project";

// Objects to Render
geometry* window::lobby;
std::vector<astronaut*> window::astronauts;
astronaut* window::player_astronaut;
// planes
std::vector<plane> window::boundaries;
// bounds
std::vector<bound*> window::bounds;
// Camera Matrices
// Projection matrix:
glm::mat4 window::projection;

// View Matrix:
glm::vec3 window::eye_pos(0, 0, 20); // Camera position.
glm::vec3 window::look_at_point(0, 0, 0); // The point we are looking at.
glm::vec3 window::up_vector(0, 1, 0); // The up direction of the camera.
glm::mat4 window::view = lookAt(eye_pos, look_at_point, up_vector);

// Shader Program ID
GLuint window::scene_program;
GLuint window::astronaut_program;

double window::curr_x;
double window::curr_y;

double window::prev_x;
double window::prev_y;

glm::vec3 window::last_point;
float window::rate;
state window::movement;
state window::current_rotation = state::rotate_model;
bool window::oscillate_off;
bool window::rotate_off;
bool window::move_off;

bool window::initialize_program() {
    // Create a shader program with a vertex shader and a fragment shader.
    scene_program = LoadShaders("shaders/scene_shader.vert", "shaders/scene_shader.frag");
    astronaut_program = LoadShaders("shaders/astronaut_shader.vert", "shaders/astronaut_shader.frag");
    // Check the shader program.
    if (!scene_program || !astronaut_program) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }
    return true;
}

bool window::initialize_objects() {
    auto num_astronauts = 9;
    auto num_boundaries = 6;
    auto colors = std::vector<glm::vec3>({
        {static_cast<float>(0x13) / 255.0f,static_cast<float>(0x2e) / 255.0f,static_cast<float>(0xd1) / 255.0f},
        {static_cast<float>(0x39) / 255.0f,static_cast<float>(0xfe) / 255.0f,static_cast<float>(0xdd) / 255.0f},
        {static_cast<float>(0x13) / 255.0f,static_cast<float>(0x80) / 255.0f,static_cast<float>(0x2c) / 255.0f},
        {static_cast<float>(0x4e) / 255.0f,static_cast<float>(0xee) / 255.0f,static_cast<float>(0x38) / 255.0f},
        {static_cast<float>(0xec) / 255.0f,static_cast<float>(0x54) / 255.0f,static_cast<float>(0xbb) / 255.0f},
        {static_cast<float>(0x6c) / 255.0f,static_cast<float>(0x2f) / 255.0f,static_cast<float>(0xbc) / 255.0f},
        {static_cast<float>(0xc5) / 255.0f,static_cast<float>(0x12) / 255.0f,static_cast<float>(0x11) / 255.0f},
        {static_cast<float>(0xd6) / 255.0f,static_cast<float>(0xdf) / 255.0f,static_cast<float>(0xf1) / 255.0f},
        {static_cast<float>(0xf6) / 255.0f,static_cast<float>(0xf6) / 255.0f,static_cast<float>(0x57) / 255.0f}
        });

    boundaries = std::vector<plane>({
        {glm::vec3(0, 0, 1), glm::vec3(0, 0, -4)},
        {glm::vec3(0, 0, -1), glm::vec3(0, 0, 15)},
        {glm::vec3(1, 0, 0), glm::vec3(-16, 0, 0)},
        {glm::vec3(-1, 0, 0), glm::vec3(16, 0, 0)}
        });
    const auto light_position = glm::vec3(0, 0, 10);
    auto sun_light = lighting(glm::vec3(.9f, .05875f, .05875f), glm::vec3(.61424f, .04136f, .04136f),
        glm::vec3(.727811f, .626959f, .626959f), glm::vec3(light_position),
        glm::vec3(1.0f, 0.09f, 0.032f));
    auto base_material = materials(glm::vec3(.329412f, .223529f, .027451f), glm::vec3(.780392f, .568627f, .113725f),
        glm::vec3(.992157f, .941176f, .807843f), 28.0f);
    lobby = new geometry("./objects/amongus_lobby.obj", base_material, sun_light, glm::vec3(0), glm::vec3(1));
    for (auto i = 0; i < num_astronauts; i++) {
        astronauts.push_back(new astronaut("./objects/amongus_astro_still.obj", sun_light, colors[i],
            glm::vec3((i * 2) - 8, .25, 5), glm::vec3(1)));
    }
    player_astronaut = new astronaut("./objects/amongus_astro_still.obj", sun_light,
        { static_cast<float>(0xf1) / 255.0f,static_cast<float>(0x7d) / 255.0f,static_cast<float>(0x0c) / 255.0f },
        glm::vec3(0, .25, 7), glm::vec3(1));
    bounds = std::vector<bound*>(astronauts.begin(), astronauts.end());
    return true;
}

void window::clean_up() {
    delete lobby;
    for (auto* an_astronaut : astronauts) {
        delete an_astronaut;
        an_astronaut = nullptr;
    }
    delete player_astronaut;
    player_astronaut = nullptr;
    // Delete the shader program.
    glDeleteProgram(scene_program);
    glDeleteProgram(astronaut_program);
}

GLFWwindow* window::create_window(int width, int height) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);

    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

#ifndef __APPLE__

    if (glewInit()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return nullptr;
    }
#endif

    glfwSwapInterval(0);

    resize_callback(window, width, height);

    return window;
}

void window::resize_callback(GLFWwindow* window, int width, int height) {
#ifdef __APPLE__
    glfwGetFramebufferSize(window, &width, &height);
#endif
    window::width = width;
    window::height = height;
    glViewport(0, 0, width, height);

    projection = glm::perspective(glm::radians(60.0),
        static_cast<double>(width) / static_cast<double>(height), 1.0, 1000.0);
}

void window::idle_callback() {
}

void window::display_callback(GLFWwindow* window) {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the objects
    //sphere_triangle->draw(view, projection, shader_program);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    const auto a_model = projection * view;
    //glDisable(GL_CULL_FACE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    lobby->draw(glm::mat4(1), scene_program);
    // Gets events, including input such as keyboard and mouse or window resizing
    for (auto* an_astronaut : astronauts) {
        an_astronaut->draw(glm::mat4(1), astronaut_program);
    }
    player_astronaut->draw(glm::mat4(1), astronaut_program);
    update_window();
    glfwPollEvents();

    // Swap buffers.
    glfwSwapBuffers(window);
}

void window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Check for a key press.
    const auto magnitude = 5;
    const auto angle = .15f;
    const auto movement_rate = 1.0f;
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_LEFT:
            rotate_eye_h(angle);
            break;
        case GLFW_KEY_RIGHT:
            rotate_eye_h(-angle);
            break;
        case GLFW_KEY_UP:
            rotate_eye_v(-angle);
            break;
        case GLFW_KEY_DOWN:
            rotate_eye_v(angle);
            break;
        case GLFW_KEY_A:
            move_player(glm::vec3(-movement_rate, 0, 0));
            break;
        case GLFW_KEY_D:
            move_player(glm::vec3(movement_rate, 0, 0));
            break;
        case GLFW_KEY_W:
            move_player(glm::vec3(0, 0, -movement_rate));
            break;
        case GLFW_KEY_S:
            move_player(glm::vec3(0, 0, movement_rate));
            break;
        case GLFW_KEY_R:
            move_y(magnitude);
            break;
        case GLFW_KEY_F:
            move_y(-magnitude);
            break;
        case GLFW_KEY_Q:
            look_at_point = glm::vec3(0);
            view = lookAt(eye_pos, look_at_point, up_vector);
            break;
        case GLFW_KEY_1:
            animate_rotation();
            break;
        case GLFW_KEY_2:
            animate_oscillate();
            break;
        case GLFW_KEY_3:
            //spin_animate();
            break;
        default:
            break;
        }
    }
}

void window::mouse_button_callback(GLFWwindow* window, const int button, const int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // get current position
        last_point = track_ball_mapping(curr_x, curr_y);
        glMatrixMode(GL_MODELVIEW);
        movement = current_rotation;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        movement = state::none;
    }
}

void window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    // origin is top left
    // start
    auto pos_x = xpos;
    auto pos_y = ypos;
    curr_x = pos_x;
    curr_y = pos_y;
    glm::vec3 direction;
    float pixel_diff;
    float rot_angle, zoom_factor;
    glm::vec3 curr_point;
    glm::vec3 center_point = glm::vec3(0, 0, 0);
    if (movement == state::rotate_model) {
        curr_point = track_ball_mapping(pos_x, pos_y);
        direction = curr_point - last_point;
        auto velocity = glm::length(direction);
        /*if (velocity > .0001) {
            glm::mat4 model_matrix = lobby->get_model();
            glm::mat4 rotation;
            glm::vec3 rot_axis;
            glLoadIdentity(); //
            glMultMatrixf((GLfloat*)glm::value_ptr(model_matrix)); //
            rot_axis = glm::cross(last_point, curr_point);
            rot_angle = velocity * rot_scale;
            glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)glm::value_ptr(rotation));
            glLoadIdentity();
            glRotatef(rot_angle, rot_axis.x, rot_axis.y, rot_axis.z);
            glMultMatrixf((GLfloat*)glm::value_ptr(rotation));
            glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)glm::value_ptr(rotation)); //
            lobby->set_model(rotation);
            for(auto* astronaut : astronauts) {
                glLoadIdentity();
                model_matrix = astronaut->get_model();
                glRotatef(rot_angle, rot_axis.x, rot_axis.y, rot_axis.z);
                glMultMatrixf((GLfloat*) glm::value_ptr(model_matrix));
                glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*) glm::value_ptr(model_matrix));
                astronaut->set_model(model_matrix);
                glLoadIdentity();
            }
        }*/
        if (velocity > .0001) {
            glMatrixMode(GL_MODELVIEW);
            glm::mat4 rotation = glm::mat4(1);
            glm::vec3 rot_axis;
            auto the_length = glm::length(eye_pos);
            //eye_pos -= the_length;
            //auto old_eye = eye_pos;
            rot_axis = glm::cross(last_point, curr_point);
            rot_angle = velocity * rot_scale;
            glLoadIdentity();
            //glMultMatrixf((GLfloat*)glm::value_ptr(model_matrix));
            glRotatef(rot_angle, rot_axis.x, rot_axis.y, rot_axis.z);
            glMultMatrixf((GLfloat*)glm::value_ptr(rotation));
            glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)glm::value_ptr(rotation));
            glm::vec4 temp_eye = glm::vec4(eye_pos, 0) * rotation;
            eye_pos = temp_eye;
            //eye_pos += the_length;
            view = lookAt(eye_pos, look_at_point, up_vector);
        }
    }
    last_point = curr_point;
}

void window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto x_off = xoffset;
    auto y_off = yoffset;
    float zoom_factor = 1.0f + y_off * .125f;
    view = scale(view, glm::vec3(zoom_factor));
    /*glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::mat4 model_matrix = lobby->get_model();
    glMultMatrixf(value_ptr(model_matrix));
    glScalef(zoom_factor, zoom_factor, zoom_factor);
    glGetFloatv(GL_MODELVIEW_MATRIX, static_cast<GLfloat*>(value_ptr(model_matrix)));
    lobby->set_model(model_matrix);
    glLoadIdentity();
    for(auto* astronaut : astronauts) {
        glLoadIdentity();
        model_matrix = astronaut->get_model();
        glMultMatrixf(value_ptr(model_matrix));
        glScalef(zoom_factor, zoom_factor, zoom_factor);
        glGetFloatv(GL_MODELVIEW_MATRIX, static_cast<GLfloat*>(value_ptr(model_matrix)));
        astronaut->set_model(model_matrix);
        glLoadIdentity();
    }*/
}

void window::get_rotation() {
}

void window::move_player(glm::vec3 a_movement) {
    auto the_model = player_astronaut->get_model();
    auto& a_radius = player_astronaut->radius;
    auto& a_center = player_astronaut->center_point;
    for (auto a_plane : boundaries) {
        auto shortest_distance = a_plane.shortest_distance(a_center);
        if (shortest_distance <= a_radius) {
            std::cout << "distance is " << a_plane.shortest_distance(a_center) << std::endl;
            std::cout << "radius is " << a_radius << std::endl;
            glm::vec3 epsilon_vector = a_movement * -.01f;
            a_movement = (a_movement * -(a_radius - shortest_distance)) + epsilon_vector;
            print_vector(a_movement);
            a_center += a_movement;
            the_model = translate(the_model, a_movement);
            player_astronaut->set_model(the_model);
            return;
        }
    }
    /*for(auto* a_bound : bounds) {
        auto other_radius = a_bound->radius;
        auto other_center = a_bound->center_point;
        if(glm::distance(a_center, other_center) < a_radius + other_radius) {
            std::cout << "distance is " << glm::distance(a_center, other_center) << std::endl;
            std::cout << "combined radius is " << a_radius + other_radius << std::endl;
            auto epsilon_vector = a_movement + a_radius + other_radius + .01f - glm::distance(a_center, other_center);
            a_movement = a_movement - epsilon_vector;
        }
    }*/
    a_center += a_movement;
    the_model = translate(the_model, a_movement);
    player_astronaut->set_model(the_model);
}

void window::update_window() {
}

glm::vec3 window::track_ball_mapping(const double pos_x, const double pos_y) {
    glm::vec3 v;
    float d;
    v.x = (2.0 * pos_x - width) / width;
    v.y = (height - 2.0 * pos_y) / height;
    v.z = 0.0;
    d = length(v);
    d = (d < 1.0) ? d : 1.0;
    v.z = sqrt(1.001 - d * d);
    v = normalize(v);
    return v;
}

void window::print_matrix(const glm::mat4 a_matrix) {
    const auto row_size = 4;
    const auto* array_form = value_ptr(a_matrix);
    for (auto j = 0; j < row_size; j++) {
        for (auto i = 0; i < row_size; i++) {
            std::clog << array_form[i + j * row_size] << " ";
        }
        std::clog << std::endl;
    }
}

void window::print_vector(const glm::vec3 a_vector) {
    std::clog << "vector: " << a_vector.x << ", " << a_vector.y << ", " << a_vector.z << std::endl;
}

// lookat changes
void window::rotate_left() {
    //std::clog << "look at point before rotation = ";
    //print_vector(look_at_point);
    //glm::vec3 window::look_at_point(0, 0, 0); // The point we are looking at.
    const auto x = look_at_point.x;
    const auto y = look_at_point.y;
    const auto z = look_at_point.z + 20;
    const auto angle = .15f;
    const auto sine = sin(angle);
    const auto cosine = cos(angle);
    look_at_point = glm::vec3(x * cosine + z * sine, y, -x * sine + z * cosine);
    std::clog << "look at point after rotation = ";
    //print_vector(look_at_point);
    view = lookAt(eye_pos, look_at_point, up_vector);
}

void window::move_x(const GLfloat magnitude) {
    eye_pos.x += magnitude;
    look_at_point.x += magnitude;
    view = lookAt(eye_pos, look_at_point, up_vector);
}

void window::move_z(const GLfloat magnitude) {
    eye_pos.z += magnitude;
    look_at_point.z += magnitude;
    view = lookAt(eye_pos, look_at_point, up_vector);
}

void window::move_y(const GLfloat magnitude) {
    eye_pos.y += magnitude;
    look_at_point.y += magnitude;
    view = lookAt(eye_pos, look_at_point, up_vector);
}


void window::rotate_eye_h(const float angle) {
    const auto x = look_at_point.x + eye_pos.x;
    const auto y = look_at_point.y + eye_pos.y;
    const auto z = look_at_point.z + eye_pos.z;
    const auto sine_theta = sin(angle);
    const auto cosine_theta = cos(angle);
    look_at_point = glm::vec3(x * cosine_theta + z * sine_theta, y, -x * sine_theta + z * cosine_theta);
    view = lookAt(eye_pos, look_at_point, up_vector);
}

void window::rotate_eye_v(const float angle) {
    const auto x = look_at_point.x + eye_pos.x;
    const auto y = look_at_point.y + eye_pos.y;
    const auto z = look_at_point.z + eye_pos.z;
    const auto sine_theta = sin(angle);
    const auto cosine_theta = cos(angle);
    eye_pos = glm::vec3(x, y * cosine_theta - z * sine_theta, y * sine_theta + z * cosine_theta);
    view = lookAt(eye_pos, look_at_point, up_vector);
}

void window::animate_oscillate() {

}

void window::animate_rotation() {

}

void window::animate_move() {
}
