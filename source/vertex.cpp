#include "../include/vertex.h"

std::ostream &operator<<(std::ostream &out, const Vertex &vert) {
    out << "Position: ";
    out << vert.position.x << " ," << vert.position.y << " ," << vert.position.z << " ";
    out << "Normal: ";
    out << vert.normal.x << " ," << vert.normal.y << " ," << vert.normal.z << " ";
    out << "Texture Coordinates: ";
    out << vert.texture_coordinates.x << " ," << vert.texture_coordinates.y << "\n";
    return out;
};

Vertex::Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex_coord) : position(pos), normal(norm), texture_coordinates(tex_coord){
};
