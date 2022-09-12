#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "shader.h"
#include "Object.h"
#include "Cube.h"
#include "PointCloud.h"
#include "Triangle.h"


class geometry;
class sphere;
class skybox;
class transform;
class astronaut;
class plane;
class bound;

enum class state {
    none,
    rotate_model,
    rotate_light,
    rotate_both
};
const float rot_scale = 40 * atan(1);

static GLfloat object_x_form[4][4] = {
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{0.0, 0.0, 0.0, 1.0}
};

static const float zoom_scale = 0.008f;

class window {
public:

    // Window Properties
    static int width;
    static int height;
    static const char* window_title;

    // Objects to Render
    static geometry* lobby;
    static std::vector<astronaut*> astronauts;
    static astronaut* player_astronaut;
    // positions
    static std::vector<float> x_positions;
    static std::vector<float> y_positions;
    static std::vector<float> z_positions;

    // part 2
    static double curr_x;
    static double curr_y;

    static double prev_x;
    static double prev_y;

    static glm::vec3 last_point;

    static state movement;
    static state current_rotation;

    // Camera Matrices
    static glm::mat4 projection;
    static glm::mat4 view;
    static glm::vec3 eye_pos, look_at_point, up_vector;

    // Shader Program ID
    static GLuint scene_program;
    static GLuint astronaut_program;

    // Constructors and Destructors
    static bool initialize_program();
    static bool initialize_objects();
    static void clean_up();

    // Window functions
    static GLFWwindow* create_window(int width, int height);
    static void resize_callback(GLFWwindow* window, int width, int height);

    // Draw and Update functions
    static void idle_callback();
    static void display_callback(GLFWwindow*);

    // Callbacks
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    // Utility functions
    static glm::vec3 track_ball_mapping(double pos_x, double pos_y);
    static void print_matrix(const glm::mat4 a_matrix);
    static void print_vector(glm::vec3 a_vector);

    // camera movement
    static void rotate_left();
    static void move_x(const GLfloat magnitude);
    static void move_z(const GLfloat magnitude);
    static void move_y(const GLfloat magnitude);
    static void rotate_eye_h(const float angle);
    static void rotate_eye_v(const float angle);

    static void animate_oscillate();
    static void animate_rotation();
    static void animate_move();

    static void update_window();
    static float rate;
    static void get_rotation();
    static bool oscillate_off;
    static bool rotate_off;
    static bool move_off;

    // player movement
    static void move_player(glm::vec3 a_movement);

    // boundaries
    static std::vector<plane> boundaries;
    // bounds
    static std::vector<bound*> bounds;
};



#endif
