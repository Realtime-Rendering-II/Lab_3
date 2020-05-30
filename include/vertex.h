#ifndef LAB_2_VERTEX_H
#define LAB_2_VERTEX_H

#include <glm/glm.hpp>
#include <iostream>

class Vertex{
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;

    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex_coord);
    friend std::ostream& operator << (std::ostream &out, Vertex const& vert);
};

#endif //LAB_2_VERTEX_H
