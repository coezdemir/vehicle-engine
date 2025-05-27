//
// Created by CemOe on 23.01.2022.
//
#pragma once
#include <engine/components/CComponent.h>
#include <engine/components/ComponentPool.h>

namespace engine::audio
{
    class Audio;

    namespace components
    {
        /// \brief Can be used to create and play audio.
        struct CAudio final : public engine::components::CComponent
        {
        public:
            friend Audio;

            struct ConstructionInfo : public engine::components::ConstructionInfo
            {
                std::string_view pathToWav;
                float volume = 1.f;
            };

            /// \brief Intended for internal usage only!
            /// \param id The id of the newly constructed component.
            /// \param owner The id of the entity that owns this component.
            explicit CAudio(engine::components::id_t id = engine::components::invalid_id,
                            entities::id_t owner = entities::invalid_id);

            /// \brief Creates the audio object and stores a reference.
            /// \param constructionInfo
            void init(const engine::components::ConstructionInfo* constructionInfo) override;

            /// \brief Plays the audio that it holds the reference to.
            void play();

        private:
            std::string_view pathToWav;

            Audio* audioSystem;

            unsigned int soundHandle;

            float volume;
        };

        using audio_container_t = typename engine::components::component_pool_t<CAudio>;
        using audio_handle_t = typename audio_container_t::handle_t;
        using audio_event_handle_t = typename audio_container_t::event_t::handle_t;
    }
}
