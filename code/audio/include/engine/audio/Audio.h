//
// Created by CemOe on 23.01.2022.
//
#pragma once
#include <engine/system/System.h>
#include <engine/Engine.h>
#include <soloud.h>
#include <soloud_audiosource.h>
#include <soloud_wav.h>
#include <engine/audio/components/CAudio.h>

#define AUDIO_HOME "../../assets/audio/"

namespace engine::audio
{
    /// \brief Subsystem managing audio objects
    class Audio final : public engine::system::System
    {
        using soundObject = typename SoLoud::Wav;
        using soundObject_handle = unsigned int;

    public:
        /// \brief Intended for internal usage only!
        Audio();

        Audio(Audio&&) = delete;

        Audio(const Audio&) = delete;

        /// \brief Creates a sound object and returns a handle to it.
        /// \param pathToWav The path to wav (will be searched in assets/audio/).
        /// \return Handle to the sound object that can be used to play the sound object.
        [[nodiscard]] soundObject_handle createSoundObject(std::string_view pathToWav);

        /// \brief Play the given sound object referenced by the handle at the given value.
        /// \param soundObjectHandle Handle of the sound object to play.
        /// \param volume The volume at which the sound is going to be played.
        void play(soundObject_handle soundObjectHandle, float volume = 1.f);

        /// \brief Add a background sound to the scene.
        /// \param pathToWav The path to wav (will be searched in assets/audio/).
        /// \param volume The volume at which the background music is going to be played.
        void setBackgroundMusic(std::string_view pathToWav, float volume = -1.f);

        /// \brief Stops all sounds that are currently being played.
        void stopAll();

    private:
        /// \brief Registers the system to the engine.
        /// \param engine Pointer to the engine.
        void registerSystem(Engine* engine) override;

        /// \brief Initializes the subsystem.
        /// \param engine Pointer to the engine.
        void startup(Engine* engine);

        /// \brief Clean up at the end of the current frame.
        void endFrame();

        /// \brief Clean up at the end of the runtime.
        void shutdown(Engine* engine);

    private:
        engine::Engine::event_engineptr_t::handle_t handle_startup;

        engine::Engine::event_engineptr_t::handle_t handle_shutdown;

        engine::Engine::event_void_t::handle_t handle_endFrame;

        SoLoud::Soloud soloud;

        std::filesystem::path audioPath;

        std::vector<soundObject*> audioSources;

        std::map<soundObject_handle, SoLoud::handle> currActiveSounds;

        std::map<std::string_view, soundObject_handle> createdAudioSources;

        engine::audio::components::audio_container_t* audioContainer;
    };
}