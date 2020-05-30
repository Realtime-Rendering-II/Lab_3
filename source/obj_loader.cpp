#include "../include/obj_loader.h"

//this obj loader is very basic and only creates one mesh
namespace ObjLoader {
    Mesh *load_obj(std::string const &obj_path) {
        std::ifstream ifs(obj_path);
        if (!ifs.is_open()) {
            std::cout << "obj could not be loaded\n";
        }
        std::string current_line;

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texture_coordinates;
        std::vector<Vertex> vertices;

        while (std::getline(ifs, current_line)) {
            std::vector<std::string> lineParts;
            std::stringstream iss(current_line);
            std::string word;

            while (iss >> word) {
                lineParts.push_back(word);
            }

            if (lineParts.size() > 1) {
                if (lineParts[0] == "v") {
                    positions.push_back(glm::vec3{stof(lineParts[1]), -stof(lineParts[3]), stof(lineParts[2])});
                }

                if (lineParts[0] == "vt") {
                    texture_coordinates.push_back(glm::vec2{stof(lineParts[1]), stof(lineParts[2])});
                }

                if (lineParts[0] == "vn") {
                    normals.push_back(glm::vec3{stof(lineParts[1]), -stof(lineParts[3]), stof(lineParts[2])});
                }

                if (lineParts[0] == "f") {
                    for (int i = 1; i < 4; ++i) {
                        std::string pos;
                        std::string norm;
                        std::string tex_coord;
                        int slash_count = 0;
                        for (char c : lineParts[i]) {
                            if (c == '/') {
                                slash_count++;
                            } else {
                                if (slash_count == 0)
                                    pos += c;
                                else if (slash_count == 1)
                                    tex_coord += c;
                                else if (slash_count == 2)
                                    norm += c;
                            }
                        }

                        if(norm != "" && tex_coord != "") {
                            vertices.push_back(Vertex{positions[stof(pos) - 1], normals[stof(norm) - 1],
                                                      texture_coordinates[stof(tex_coord) - 1]});
                        }else if(tex_coord != ""){
                            vertices.push_back(Vertex{positions[stof(pos) - 1], glm::vec3{},
                                                      texture_coordinates[stof(tex_coord) - 1]});
                        } else if (norm != ""){
                            vertices.push_back(Vertex{positions[stof(pos) - 1], normals[stof(norm) - 1],
                                                      glm::vec2{}});
                        } else {
                            vertices.push_back(Vertex{positions[stof(pos) - 1], glm::vec3{},
                                                      glm::vec2{}});
                        }
                    }

                    if (lineParts.size() == 5) {
                        for (int i = 2; i < 5; ++i) {
                            std::string pos;
                            std::string norm;
                            std::string tex_coord;
                            int slash_count = 0;
                            int j = i;
                            if(i == 2)
                                j = 1;
                            for (char c : lineParts[j]) {
                                if (c == '/') {
                                    slash_count++;
                                } else {
                                    if (slash_count == 0)
                                        pos += c;
                                    else if (slash_count == 1)
                                        tex_coord += c;
                                    else if (slash_count == 2)
                                        norm += c;
                                }
                            }

                            if(norm != "" && tex_coord != "") {
                                vertices.push_back(Vertex{positions[stof(pos) - 1], normals[stof(norm) - 1],
                                                          texture_coordinates[stof(tex_coord) - 1]});
                            }else if(tex_coord != ""){
                                vertices.push_back(Vertex{positions[stof(pos) - 1], glm::vec3{},
                                                          texture_coordinates[stof(tex_coord) - 1]});
                            } else if (norm != ""){
                                vertices.push_back(Vertex{positions[stof(pos) - 1], normals[stof(norm) - 1],
                                                          glm::vec2{}});
                            } else {
                                vertices.push_back(Vertex{positions[stof(pos) - 1], glm::vec3{},
                                                          glm::vec2{}});
                            }
                        }
                    }
                }
            }
        }

        return new Mesh{vertices};
    }
}
