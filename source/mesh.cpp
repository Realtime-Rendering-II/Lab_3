#include "../include/mesh.h"

#include <utility>

Mesh::Mesh(std::vector<Vertex> vertices) : vertices(std::move(vertices)) {
    setup_mesh();
    model_matrix = glm::mat4(1.0f);
}

void Mesh::draw(GLuint shader) {
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &model_matrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    if(textures.size() == 1) {
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glUniform1i(glGetUniformLocation(shader, "custom_texture_1"), 0);
    } else if(textures.size() == 2){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glUniform1i(glGetUniformLocation(shader, "mirror_view"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        glUniform1i(glGetUniformLocation(shader, "normal_view"), 1);
    }
    glBindVertexArray(VAO);

    GLintptr vertex_position_offset = 0 * sizeof(float);
    GLintptr vertex_normal_offset = 3 * sizeof(float);
    GLintptr vertex_texcoord_offset = 6 * sizeof(float);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) vertex_position_offset);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) vertex_normal_offset);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *) vertex_texcoord_offset);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void Mesh::setup_mesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::add_texture(GLuint tex) {
    if (textures.size() == 2) {
        textures.clear();
        textures.push_back(tex);
    } else
        textures.push_back(tex);
}

glm::mat4 Mesh::get_model_matrix() {
    return model_matrix;
}

void Mesh::set_model_matrix(glm::mat4 new_model) {
    model_matrix = new_model;
}


