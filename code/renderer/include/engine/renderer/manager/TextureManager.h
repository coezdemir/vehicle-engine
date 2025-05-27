//
// Created by Cem on 15.07.2022.
//
#pragma once
#include <filesystem>
#include <glad/glad.h>
#include <engine/renderer/Shader.h>
#include <map>

#define TEXTURES_HOME "../../assets/textures/"

namespace engine::renderer
{
    enum TextureType
    {
        ttDIFFUSE = (1 << 0),
        ttSPECULAR = (1 << 1),
        ttNORMAL = (1 << 2),
        ttEMISSIVE = (1 << 3),
        ttOTHER = (1 << 4)
    };

    /// \brief Represents an OpenGL texture.
    struct Texture
    {
    public:
        unsigned int id;
        std::filesystem::path path;
        TextureType type;

        Texture()
            : id(-1),
              type(ttOTHER)
        {

        }

        [[nodiscard]] std::string_view typeToString() const
        {
            switch (type)
            {
                case ttDIFFUSE:
                    return "texture_diffuse";
                case ttSPECULAR:
                    return "texture_specular";
                case ttNORMAL:
                    return "texture_normal";
                case ttEMISSIVE:
                    return "texture_emissive";
                 default:
                    return "";
            }
        }
    };

    namespace manager
    {
        /// \brief Manages all textures.
        ///
        /// Also provided helper function bindTexture() to easily bind textures.
        class TextureManager
        {
        public:
            static void init();

            static void shutdown();

            [[nodiscard]] static unsigned int loadTexture(std::string_view path);

            /// \brief Bind texture to use it in the shader.
            /// \param shader Shader that wants to use the texture.
            /// \param textureName Name of the sampler in the shader.
            /// \param textureID ID of the texture.
            /// \param resetBoundTexture Should reset the bound texture units and begin at GL_TEXTURE0
            static void bindTexture(const Shader* shader, std::string_view textureName, unsigned int textureID,
                                    bool resetBoundTexture = true);

            /// \brief Call this when you stopped using the texture to guarantee that the memory is freed when no one uses that texture.
            ///
            /// Otherwise the texture will be freed at the end of the runtime.
            /// \param id Texture ID
            static void lostUser(unsigned int id);

        private:
            [[nodiscard]] static unsigned int loadTextureInternal(std::string_view path);

            struct TextureCompare
            {
                bool operator() (const Texture& lhs, const Texture& rhs) const
                {
                    return &lhs.id < &rhs.id;
                }
            };

            static std::map<Texture, unsigned int, TextureCompare> textures;

            static unsigned int currentlyBound;

            static std::filesystem::path texturePath;
        };
    }
}
