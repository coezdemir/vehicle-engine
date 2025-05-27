//
// Created by CemOe on 15.08.2021.
//
#pragma once
#include <engine/components/CComponent.h>
#include <filesystem>
#include <map>
#include "Shader.h"
#include "engine/renderer/manager/TextureManager.h"

namespace engine::renderer
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    struct Material
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
        bool bHasDiffuseMap;
        bool bHasSpecularMap;
        bool bHasNormalMap;
        bool bHasEmissiveMap;

        Material()
        {
            ambient = glm::vec3(1.f);
            diffuse = glm::vec3(1.f);
            specular = glm::vec3(1.f);
            shininess = 1.f;
            bHasDiffuseMap = false;
            bHasSpecularMap = false;
            bHasNormalMap = false;
        }

        bool hasTextures()
        {
            return bHasDiffuseMap || bHasSpecularMap;
        }
    };

    namespace manager
    {
        class ModelLoader;
        class ModelManager;
    }

    class Mesh
    {
    public:
        Mesh();

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int triangleCount,
             std::vector<Texture> textures, Material material);

        ~Mesh();

        // explicit move constructor
        Mesh(Mesh&& other) noexcept
        {
            std::swap(vertices, other.vertices);
            std::swap(indices, other.indices);
            std::swap(textures, other.textures);
            std::swap(material, other.material);
            std::swap(triangleCount, other.triangleCount);
            std::swap(VAO, other.VAO);
            std::swap(VBO, other.VBO);
            std::swap(EBO, other.EBO);
        }

        Mesh& operator=(Mesh&& other) noexcept
        {
            if(this != &other)
            {
                std::swap(vertices, other.vertices);
                std::swap(indices, other.indices);
                std::swap(textures, other.textures);
                std::swap(material, other.material);
                std::swap(triangleCount, other.triangleCount);
                std::swap(VAO, other.VAO);
                std::swap(VBO, other.VBO);
                std::swap(EBO, other.EBO);
                return *this;
            }
            return *this;
        }

        virtual void draw(const Shader* shader) const;

        [[nodiscard]] std::vector<glm::vec3> getVertices() const;

        [[nodiscard]] const std::vector<unsigned int>& getIndices() const;

        [[nodiscard]] unsigned int getTriangleCount() const;

        virtual void setIsParticleMesh(bool bIsParticleMesh);

    private:
        void setupMesh();

        std::vector<Vertex> vertices;

        std::vector<unsigned int> indices;

        std::vector<Texture> textures;

        Material material;

        unsigned int triangleCount;

        unsigned int VAO, VBO, EBO;

    protected:
        bool bIsParticleMesh;
    };

    class Model : public Mesh
    {
        friend manager::ModelLoader;
        friend manager::ModelManager;
    public:
        Model();

        // explicit move constructor
        Model(Model&& other) noexcept
        {
            Mesh(std::move(other));
            path = other.path;
            std::swap(meshes, other.meshes);

            other.meshes.clear();
            other.path = "";
        }

        Model& operator=(Model&& other) noexcept
        {
            if(this != &other)
            {
                Mesh::operator=(std::move(other));
                path = other.path;
                std::swap(meshes, other.meshes);

                other.meshes.clear();
                other.path = "";
                return *this;
            }
            return *this;
        }

        void draw(const Shader* shader) const override;

        [[nodiscard]] std::vector<glm::vec3> getVertices(int index) const;

        [[nodiscard]] const std::vector<unsigned int>& getIndices(int index) const;

        [[nodiscard]] unsigned int getNumberOfMeshes() const;

        void getAllVertices(std::vector<std::vector<glm::vec3>>& allVertices) const;

        void getAllIndices(std::vector<std::vector<unsigned int>>& allIndices) const;

        void getAllTriangleCounts(std::vector<unsigned int>& allTriangleCounts) const;

        void setIsParticleMesh(bool bIsParticleMesh) override;

        const std::filesystem::path& getPath() const;

    private:
        std::vector<Mesh> meshes;

        std::filesystem::path path;
    };
}
