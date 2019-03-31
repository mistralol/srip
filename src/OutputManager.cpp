
#include "main.h"

OutputManager::OutputManager() :
    m_currentfile("unknown")
{

}

OutputManager::~OutputManager() {

}

void OutputManager::Start() {
    ScopedLock lock(&m_mutex);
    for(auto it : m_pipes) {
        if (it->DoRestartOnNewFile())
            it->Start();
    }
}

void OutputManager::PushBuffer(GstCaps *caps, GstBuffer *buffer) {
    ScopedLock lock(&m_mutex);
    for(auto it : m_pipes) {
        if (it->IsRunning())
            it->PushBuffer(caps, buffer);
    }
}

void OutputManager::SetFilename(const std::string &Album, const std::string &Artist, const std::string &Song) {
    ScopedLock lock(&m_mutex);
    std::string filename = Artist + " - " + Song;
    if (m_currentfile == filename)
        return;

    for(auto it : m_pipes) {
        if (it->IsRunning()) {
            if (it->DoRestartOnNewFile()) {
                it->Stop();
            }
        }
    }

    LogInfo("New Song Name: %s", filename.c_str());
    m_currentfile = filename;

    if (!Album.empty()) {
        for(auto it : m_pipes) {
            if (it->IsRunning() == false) {
                it->SetFileName(filename);
                it->Start();
            }
        }
    }
}

void OutputManager::PipelineAdd(std::shared_ptr<IOutputPipeline> pipeline) {
    ScopedLock lock(&m_mutex);
    m_pipes.push_back(pipeline);
    if (pipeline->DoRestartOnNewFile() == false) {
        pipeline->Start();
    }
}

void OutputManager::PipelineRemoveAll() {
    ScopedLock lock(&m_mutex);
    for(auto it : m_pipes) {
        if (it->IsRunning())
            it->Stop();
    }
    m_pipes.clear();
}

