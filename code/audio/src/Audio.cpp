//
// Created by CemOe on 23.01.2022.
//

#include <engine/audio/Audio.h>
#include <engine/audio/components/CAudio.h>

engine::audio::Audio::Audio() : System()
{

}

void engine::audio::Audio::registerSystem(Engine* engine)
{
    handle_startup = engine->event_startup.subscribe([this](engine::Engine* e) {
        startup(e);
    });

    handle_shutdown = engine->event_shutdown.subscribe([this](engine::Engine* e) {
        shutdown(e);
    });

    handle_endFrame = engine->event_endFrame.subscribe([this]() {
       endFrame();
    });
}

void engine::audio::Audio::startup(Engine* engine)
{
    soloud.init();
    audioContainer = engine->createContainer<engine::audio::components::CAudio>();

    audioContainer->create_component.subscribe([this](auto audioHandle){
        audioHandle->audioSystem = this;
    });
}

void engine::audio::Audio::endFrame()
{
    for (auto entry : currActiveSounds)
    {
        if(!soloud.isValidVoiceHandle(entry.second))
            currActiveSounds.erase(entry.first);
    }
}

void engine::audio::Audio::shutdown(Engine* engine)
{
    for (int i = 0; i < audioSources.size(); ++i)
        delete audioSources[i];

    soloud.deinit();
}

engine::audio::Audio::soundObject_handle engine::audio::Audio::createSoundObject(std::string_view pathToWav)
{
    audioPath = AUDIO_HOME;
    audioPath += pathToWav;
    std::string audioPathString = audioPath.string();

    if(createdAudioSources.find(audioPath.string()) != createdAudioSources.end())
        return createdAudioSources[audioPathString];

    audioSources.push_back(new soundObject());
    engine::audio::Audio::soundObject_handle handle = audioSources.size() - 1;
    audioSources[handle]->load(audioPathString.c_str());
    createdAudioSources.insert({audioPathString, handle});
    return handle;
}

void engine::audio::Audio::play(engine::audio::Audio::soundObject_handle soundObjectHandle, float volume)
{
    if(soloud.isValidVoiceHandle(currActiveSounds[soundObjectHandle]))
        return;

    currActiveSounds.insert({soundObjectHandle, soloud.play(*audioSources[soundObjectHandle], volume)});
}

void engine::audio::Audio::setBackgroundMusic(std::string_view pathToWav, float volume)
{
    soundObject_handle background = createSoundObject(pathToWav);
    audioSources[background]->setLooping(true);
    soloud.playBackground(*audioSources[background]);
    soloud.setProtectVoice(background, true);
}

void engine::audio::Audio::stopAll()
{
    soloud.stopAll();
}
