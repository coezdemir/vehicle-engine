//
// Created by coezdemir on 10.11.2020.
//
#include <engine/renderer/Mesh.h>
#include <utility>
#include <vector>
#include <string>

///#####################################################################################################################
/// MESH IMPLEMENTATION START
///#####################################################################################################################

engine::renderer::Mesh::Mesh() : bIsParticleMesh(false), VAO(0), VBO(0), EBO(0)
{

}

engine::renderer::Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int triangleCount,
                             std::vector<Texture> textures,
                             Material material)
        : vertices(std::move(vertices)), indices(std::move(indices)), triangleCount(triangleCount),
          textures(std::move(textures)), material(material), bIsParticleMesh(false), VAO(0), VBO(0), EBO(0)
{
    setupMesh();
}

engine::renderer::Mesh::~Mesh()
{
    for (auto& texture : textures)
        engine::renderer::manager::TextureManager::lostUser(texture.id);

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void engine::renderer::Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    const GLuint attributeLocationVertexPosition = 0;
    glVertexAttribPointer(attributeLocationVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);

    // vertex normal
    glEnableVertexAttribArray(1);
    const GLuint attributeLocationNormalPosition = 1;
    glVertexAttribPointer(attributeLocationNormalPosition, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, Vertex::normal));

    // vertex texture coords
    glEnableVertexAttribArray(2);
    const GLuint attributeLocationTextCoordPosition = 2;
    glVertexAttribPointer(attributeLocationTextCoordPosition, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, Vertex::textCoords));

    // vertex tangent
    glEnableVertexAttribArray(3);
    const GLuint attributeLocationTangentPosition = 3;
    glVertexAttribPointer(attributeLocationTangentPosition, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, Vertex::tangent));

    // vertex bitangent
    glEnableVertexAttribArray(4);
    const GLuint attributeLocationBitangentPosition = 4;
    glVertexAttribPointer(attributeLocationBitangentPosition, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, Vertex::bitangent));

    glBindVertexArray(0);
}

void engine::renderer::Mesh::draw(const Shader* shader) const
{
    if (!bIsParticleMesh)
    {
        if (!material.bHasDiffuseMap)
        {
            shader->setVector("material.ambient", material.ambient);
            shader->setVector("material.diffuse", material.diffuse);
        }

        if (!material.bHasSpecularMap)
            shader->setVector("material.specular", material.specular);

        shader->setFloat("material.shininess", material.shininess);

        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int emissiveNr = 1;
        std::string textureName = "material.";
        std::string defaultName = textureName;
        for (unsigned int i = 0; i < textures.size(); ++i)
        {
            // activate proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            TextureType type = textures[i].type;
            if (type == ttDIFFUSE)
                number = std::to_string(diffuseNr++);
            else if (type == ttSPECULAR)
                number = std::to_string(specularNr++);
            else if (type == ttNORMAL)
                number = std::to_string(normalNr++);
            else if (type == ttEMISSIVE)
                number = std::to_string(emissiveNr++);

            textureName.append(textures[i].typeToString());
            textureName.append(number);

            manager::TextureManager::bindTexture(shader, textureName, textures[i].id, i == 0);

            textureName = defaultName;
        }
        shader->setBool("material.bHasDiffuseMap", material.bHasDiffuseMap);
        shader->setBool("material.bHasSpecularMap", material.bHasSpecularMap);
        shader->setBool("material.bHasNormalMap", material.bHasNormalMap);
        shader->setBool("material.bHasEmissiveMap", material.bHasEmissiveMap);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

std::vector<glm::vec3> engine::renderer::Mesh::getVertices() const
{
    std::vector<glm::vec3> positions;
    for (int i = 0; i < vertices.size(); ++i)
    {
        positions.emplace_back(vertices[i].position);
    }

    return positions;
}

const std::vector<unsigned int>& engine::renderer::Mesh::getIndices() const
{
    return indices;
}

unsigned int engine::renderer::Mesh::getTriangleCount() const
{
    return triangleCount;
}

void engine::renderer::Mesh::setIsParticleMesh(bool bValue)
{
    bIsParticleMesh = bValue;
}

///#####################################################################################################################
/// MODEL IMPLEMENTATION START
///#####################################################################################################################

engine::renderer::Model::Model() : Mesh()
{

}

std::vector<glm::vec3> engine::renderer::Model::getVertices(int index) const
{
    return meshes[index].getVertices();
}

const std::vector<unsigned int>& engine::renderer::Model::getIndices(int index) const
{
    return meshes[index].getIndices();
}

unsigned int engine::renderer::Model::getNumberOfMeshes() const
{
    return meshes.size();
}

void engine::renderer::Model::getAllVertices(std::vector<std::vector<glm::vec3>>& allVertices) const
{
    for (auto& mesh: meshes)
        allVertices.push_back(mesh.getVertices());
}

void engine::renderer::Model::getAllIndices(std::vector<std::vector<unsigned int>>& allIndices) const
{
    for (auto& mesh: meshes)
        allIndices.push_back(mesh.getIndices());
}

void engine::renderer::Model::getAllTriangleCounts(std::vector<unsigned int>& allTriangleCounts) const
{
    for (auto& mesh: meshes)
        allTriangleCounts.push_back(mesh.getTriangleCount());
}

void engine::renderer::Model::draw(const engine::renderer::Shader* shader) const
{
    for (auto& mesh: meshes)
        mesh.draw(shader);
}

void engine::renderer::Model::setIsParticleMesh(bool bIsParticleMesh)
{
    for (int i = 0; i < meshes.size(); ++i)
        meshes[i].setIsParticleMesh(bIsParticleMesh);
}

const std::filesystem::path& engine::renderer::Model::getPath() const
{
    return path;
}

///#####################################################################################################################
/// PARTICLE MODEL IMPLEMENTATION START
///#####################################################################################################################