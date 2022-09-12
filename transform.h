#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_
#include "node.h"
#include <list>
#include <vector>

class transform :
    public node {
protected:
    glm::mat4 model_matrix_;
    std::vector<node*> node_list_;
public:
    transform(glm::mat4 a_matrix);
    ~transform() override;
    void swap_transform(glm::mat4 a_matrix);
    void add_node(node* a_node);
    void draw(glm::mat4 current_matrix, const GLuint shader) override;
    void update() override;
};
#endif


