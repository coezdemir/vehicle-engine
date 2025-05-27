//
// Created by Cem on 16.07.2022.
//
#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/mesh.h>
#include <assimp/types.h>
#include <engine/renderer/Mesh.h>

#define MODELS_HOME "../../assets/models/"

namespace engine::renderer::manager
{
    /// \brief Holds pointer to the mesh.
    struct ModelHandle
    {
        friend class ModelManager;

    public:
        ModelHandle() : model(nullptr)
        {

        }

        const Model* operator->() const
        {
            return model;
        }

        [[nodiscard]] bool isValid()
        {
            return model != nullptr;
        }

    private:
        explicit ModelHandle(Model* model)
                : model(model)
        {

        }

        Model* model;
    };

    /// \brief Loads 3D models in obj format with the help of assimp.
    class ModelLoader
    {
        friend class ModelManager;

    private:
        ModelLoader() : currentModel(nullptr)
        {

        }

        /// Starts loading process of a model.
        /// \param path Path to the model.
        /// \param model Out for model.
        /// \param bIsParticleMesh Will this model be used for a particle system.
        void loadModel(const std::filesystem::path& path, Model* model, bool bIsParticleMesh)
        {
            // Create empty model
            currentModel = model;
            currentModel->path = path;

            loadMeshes(path);

            currentModel->setIsParticleMesh(bIsParticleMesh);
        }

        /// Starts the data processing from the obj.
        /// \param path Path to the model.
        void loadMeshes(const std::filesystem::path& path)
        {
            currentPath = path;

            const aiScene* scene = importer.ReadFile(currentPath.string(),
                                                     aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
                                                     aiProcess_CalcTangentSpace);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
                return;
            }

            // Load the model
            processNode(scene->mRootNode, scene);
        }

        /// Processes nodes.
        /// \param node Current node.
        /// \param scene The assimp scene.
        void processNode(aiNode* node, const aiScene* scene)
        {
            // process all the node's meshes (if any)
            for(int i = 0; i < node->mNumMeshes; ++i)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                currentModel->meshes.push_back(processMesh(mesh, scene));
            }

            // then do the same for each of its children
            for(int i = 0; i < node->mNumChildren; ++i)
            {
                processNode(node->mChildren[i], scene);
            }
        }

        /// Process the mesh data with its materials.
        /// \param mesh Assimp mesh to process.
        /// \param scene Assimp scene where the mesh resides
        /// \return Constructed mesh.
        [[nodiscard]] Mesh processMesh(aiMesh* mesh, const aiScene* scene)
        {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<Texture> textures;

            // process vertex positions, normals and texture coordinates
            for(int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex{};
                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.position = vector;

                if (mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.normal = vector;
                }

                // Does the mesh contain texture coordinates?
                if(mesh->mTextureCoords[0])
                {
                    glm::vec2 vector2;
                    vector2.x = mesh->mTextureCoords[0][i].x;
                    vector2.y = mesh->mTextureCoords[0][i].y;
                    vertex.textCoords = vector2;
                }
                else
                    vertex.textCoords = glm::vec2(0.0f);

                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.bitangent = vector;

                vertices.push_back(vertex);
            }

            // process indices
            for(int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                for( int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            // process material
            Material mat = Material();
            if(mesh->mMaterialIndex >= 0)
            {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

                std::vector<Texture> diffuseMaps;
                loadMaterialTextures(diffuseMaps, material, aiTextureType_DIFFUSE, ttDIFFUSE);
                mat.bHasDiffuseMap = diffuseMaps.size();
                textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

                std::vector<Texture> specularMaps;
                loadMaterialTextures(specularMaps, material, aiTextureType_SPECULAR, ttSPECULAR);
                mat.bHasSpecularMap = specularMaps.size();
                textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

                std::vector<Texture> normalMaps;
                loadMaterialTextures(normalMaps, material, aiTextureType_HEIGHT, ttNORMAL);
                mat.bHasNormalMap = normalMaps.size();
                textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

                std::vector<Texture> emissionMaps;
                loadMaterialTextures(emissionMaps, material, aiTextureType_EMISSIVE, ttEMISSIVE);
                mat.bHasEmissiveMap = emissionMaps.size();
                textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

                aiColor3D color(0.f, 0.f, 0.f);
                material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                mat.diffuse = aiColor3DToGlm(color) * float(!mat.bHasDiffuseMap);

                material->Get(AI_MATKEY_COLOR_AMBIENT, color);
                mat.ambient = aiColor3DToGlm(color) * float(!mat.bHasDiffuseMap);

                material->Get(AI_MATKEY_COLOR_SPECULAR, color);
                mat.specular = aiColor3DToGlm(color) * float(!mat.bHasSpecularMap);

                material->Get(AI_MATKEY_SHININESS, mat.shininess);
            }

            return {vertices, indices, mesh->mNumFaces, textures, mat};
        }

        /// Loads the textures the material references.
        /// \param outTextures Vector that is going to be filled with the loaded textures.
        /// \param mat Pointer to the material.
        /// \param type Texture type.
        /// \param typeName Internal texture type.
        void loadMaterialTextures(std::vector<Texture>& outTextures, aiMaterial* mat, aiTextureType type, TextureType typeName)
        {
            for(unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
            {
                aiString str;
                mat->GetTexture(type, i, &str);

                std::string currPathString = currentPath.string();
                auto pos = currPathString.find("models/");
                currPathString.erase(0, pos + 7);
                currPathString = currPathString.substr(0, currPathString.find_last_of("/"));
                currPathString.append("/");
                currPathString.append(str.data);

                Texture texture;
                texture.id = manager::TextureManager::loadTexture(currPathString);
                texture.type = typeName;
                texture.path = currPathString;
                outTextures.push_back(texture);
            }
        }

        /// Translates assimp color to glm::vec3.
        /// \param color Asssimp color to translate.
        /// \return Color in glm::vec3.
        glm::vec3 aiColor3DToGlm(const aiColor3D& color)
        {
            return {color.r, color.g, color.b};
        }

        Model* currentModel;

        std::filesystem::path currentPath;

        Assimp::Importer importer;
    };

    /// \brief Manages all models.
    ///
    /// If the user counter of a model falls to zero the model is destroyed.
    class ModelManager
    {
    public:
        /// \brief Start the loading process with the help of the model loader.
        /// \param path Path to the model.
        /// \param bIsParticleMesh Is this model going to be used in a particle system.
        /// \return Handle fro accessing a const pointer to the model.
        [[nodiscard]] static ModelHandle loadModel(const std::filesystem::path& path, bool bIsParticleMesh = false);

        /// \brief Call this when you copied the model handle to tell the model manager to increment the ref counter.
        /// \param handle Model handle
        static void addUser(ModelHandle handle);

        /// \brief Call this when you stopped using the model to guarantee that the memory is freed when no one uses that model.
        /// \param handle Model handle
        static void lostUser(ModelHandle handle);

    private:
        /// \brief Check if the model was already loaded.
        ///
        /// If the model was already loaded return a handle.
        /// \param path Path to the model.
        /// \return Valid handle if model was loaded already.
        [[nodiscard]] static ModelHandle checkForModel(const std::filesystem::path& path);

        struct ModelCompare
        {
            bool operator() (const Model* lhs, const Model* rhs) const
            {
                return lhs < rhs;
            }
        };

        static std::map<Model*, int, ModelCompare> models;

        static ModelLoader loader;
    };
};


