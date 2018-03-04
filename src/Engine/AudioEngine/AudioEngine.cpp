//
// Created by Sebastian on 12/23/2017.
//
// Modified by Brian on 03/03/2018
// Based on implementation by Cody Claborn, at: https://codyclaborn.me/tutorial/making-a-basic-fmod-audio-engine-in-c.html
//

#include "AudioEngine.h"

Implementation::Implementation() {
    mpStudioSystem = NULL;
    AudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
    AudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

    mpSystem = NULL;
    AudioEngine::ErrorCheck(mpStudioSystem->getLowLevelSystem(&mpSystem));
}

Implementation::~Implementation() {
    AudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
    AudioEngine::ErrorCheck(mpStudioSystem->release());
}

void Implementation::update() {
    std::vector<ChannelMap::iterator> pStoppedChannels;
    for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it) {
        bool bIsPlaying=false;
        it->second->isPlaying(&bIsPlaying);
        if (!bIsPlaying) {
            pStoppedChannels.push_back(it);
        }
    }
    for (auto& it : pStoppedChannels)
    {
        mChannels.erase(it);
    }
    AudioEngine::ErrorCheck(mpStudioSystem->update());
}

Implementation* sgpImplementation = nullptr;

void AudioEngine::init() {
    sgpImplementation = new Implementation;
}

void AudioEngine::update() {
    sgpImplementation->update();
}

void AudioEngine::loadSound(const std::string& strSoundName, bool b3d, bool bLooping, bool bStream)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt != sgpImplementation->mSounds.end())
        return;

    FMOD_MODE eMode = FMOD_DEFAULT;
    eMode |= b3d ? FMOD_3D : FMOD_2D;
    eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

    FMOD::Sound* pSound = nullptr;
    AudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(strSoundName.c_str(), eMode, nullptr, &pSound));
    if (pSound) {
        sgpImplementation->mSounds[strSoundName] = pSound;
    }

}

void AudioEngine::loadMusic(const std::string& strMusicName) {
    loadSound(strMusicName, false, true, true);
    // assign a channel for the music, if one hasn't been defined already.
    if (musicChannel == -1) {
        musicChannel = sgpImplementation->mnNextChannelId++;
    }
    auto tFoundIt = sgpImplementation->mSounds.find(strMusicName);
    FMOD::Channel* pChannel = nullptr;
    AudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
}

void AudioEngine::unloadSound(const std::string& strSoundName)
{
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
        return;

    AudioEngine::ErrorCheck(tFoundIt->second->release());
    sgpImplementation->mSounds.erase(tFoundIt);
}

int AudioEngine::playSounds(const std::string& strSoundName, const glm::vec3& vPosition, float fVolumedB)
{
    int nChannelId = sgpImplementation->mnNextChannelId++;
    auto tFoundIt = sgpImplementation->mSounds.find(strSoundName);
    if (tFoundIt == sgpImplementation->mSounds.end())
    {
        loadSound(strSoundName);
        tFoundIt = sgpImplementation->mSounds.find(strSoundName);
        if (tFoundIt == sgpImplementation->mSounds.end())
        {
            return nChannelId;
        }
    }
    FMOD::Channel* pChannel = nullptr;
    AudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
    if (pChannel)
    {
        FMOD_MODE currMode;
        tFoundIt->second->getMode(&currMode);
        if (currMode & FMOD_3D) {
            FMOD_VECTOR position = vectorToFmod(vPosition);
            AudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
        }
        AudioEngine::ErrorCheck(pChannel->setVolume(dbToVolume(fVolumedB)));
        AudioEngine::ErrorCheck(pChannel->setPaused(false));
        sgpImplementation->mChannels[nChannelId] = pChannel;
    }
    return nChannelId;
}

void AudioEngine::setChannel3dPosition(int nChannelId, const glm::vec3& vPosition)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    FMOD_VECTOR position = vectorToFmod(vPosition);
    AudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void AudioEngine::setChannelVolume(int nChannelId, float fVolumedB)
{
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return;

    AudioEngine::ErrorCheck(tFoundIt->second->setVolume(dbToVolume(fVolumedB)));
}

bool AudioEngine::isPlaying(int nChannelId) const {
    auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
    if (tFoundIt == sgpImplementation->mChannels.end())
        return false;

    bool bIsPlaying = false;
    tFoundIt->second->isPlaying(&bIsPlaying);
    if (!bIsPlaying) {
        return true;
    }
    else{
    return false;
    }
}

void AudioEngine::loadBank(const std::string& strBankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
    auto tFoundIt = sgpImplementation->mBanks.find(strBankName);
    if (tFoundIt != sgpImplementation->mBanks.end()) {
        return;
    }
    FMOD::Studio::Bank* pBank;
    AudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(strBankName.c_str(), flags, &pBank));
    if (pBank) {
        sgpImplementation->mBanks[strBankName] = pBank;
    }
}

void AudioEngine::loadEvent(const std::string& strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit != sgpImplementation->mEvents.end())
        return;
    FMOD::Studio::EventDescription* pEventDescription = NULL;
    AudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(strEventName.c_str(), &pEventDescription));
    if (pEventDescription) {
        FMOD::Studio::EventInstance* pEventInstance = NULL;
        AudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
        if (pEventInstance) {
            sgpImplementation->mEvents[strEventName] = pEventInstance;
        }
    }
}

void AudioEngine::playEvent(const std::string &strEventName) {
    auto tFoundit = sgpImplementation->mEvents.find(strEventName);
    if (tFoundit == sgpImplementation->mEvents.end()) {
        loadEvent(strEventName);
        tFoundit = sgpImplementation->mEvents.find(strEventName);
        if (tFoundit == sgpImplementation->mEvents.end())
            return;
    }
    tFoundit->second->start();
}

void AudioEngine::stopEvent(const std::string &strEventName, bool bImmediate) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

    FMOD_STUDIO_STOP_MODE eMode;
    eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
    AudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

bool AudioEngine::isEventPlaying(const std::string &strEventName) const {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return false;

    FMOD_STUDIO_PLAYBACK_STATE* state = NULL;
    if (tFoundIt->second->getPlaybackState(state) == FMOD_STUDIO_PLAYBACK_PLAYING) {
        return true;
    }
    return false;
}



void AudioEngine::getEventParameter(const std::string &strEventName, const std::string &strParameterName, float* parameter) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

    FMOD::Studio::ParameterInstance* pParameter = NULL;
    AudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
    AudioEngine::ErrorCheck(pParameter->getValue(parameter));
}

void AudioEngine::setEventParameter(const std::string &strEventName, const std::string &strParameterName, float fValue) {
    auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
    if (tFoundIt == sgpImplementation->mEvents.end())
        return;

    FMOD::Studio::ParameterInstance* pParameter = NULL;
    AudioEngine::ErrorCheck(tFoundIt->second->getParameter(strParameterName.c_str(), &pParameter));
    AudioEngine::ErrorCheck(pParameter->setValue(fValue));
}

FMOD_VECTOR AudioEngine::vectorToFmod(const glm::vec3& vPosition) {
    FMOD_VECTOR fVec;
    fVec.x = vPosition.x;
    fVec.y = vPosition.y;
    fVec.z = vPosition.z;
    return fVec;
}

float AudioEngine::dbToVolume(float dB)
{
    return powf(10.0f, 0.05f * dB);
}

float AudioEngine::volumeTodb(float volume)
{
    return 20.0f * log10f(volume);
}

int AudioEngine::ErrorCheck(FMOD_RESULT result) {
    if (result != FMOD_OK) {
        std::cout << "FMOD ERROR " << result << std::endl;
        return 1;
    }
    // cout << "FMOD all good" << endl;
    return 0;
}

void AudioEngine::shutdown() {
    delete sgpImplementation;
}