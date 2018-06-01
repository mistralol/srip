
#include "main.h"

OutputManager::OutputManager() :
    m_currentfile("unknown")
{

}

OutputManager::~OutputManager() {

}

void OutputManager::PushBuffer(GstCaps *caps, GstBuffer *buffer) {
    ScopedLock lock(&m_mutex);
    for(auto it : m_pipes) {
        it->PushBuffer(caps, buffer);
    }
}

void OutputManager::SetFileName(const std::string &filename) {
    ScopedLock lock(&m_mutex);
    for(auto it : m_pipes) {
        it->Stop();
    }

    m_currentfile = filename;

    for(auto it : m_pipes) {
        it->SetFileName(filename);
        it->Start();
    }
}

void OutputManager::PipelineAdd(std::shared_ptr<IOutputPipeline> pipeline) {
    ScopedLock lock(&m_mutex);
    pipeline->SetFileName(m_currentfile);
    pipeline->Start();
    m_pipes.push_back(pipeline);
}

void OutputManager::PipelineRemoveAll() {
    ScopedLock lock(&m_mutex);
    for(auto it : m_pipes) {
        it->Stop();
    }
    m_pipes.clear();
}

