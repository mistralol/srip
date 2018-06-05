
#include "main.h"

OutputWav::OutputWav() :
    m_filename(""),
    m_thesrc(NULL)
{

}

OutputWav::~OutputWav() {

}

void OutputWav::OnPreStart() {
    ScopedLock lock(&m_mutex);
    std::stringstream ss;
    ss << "appsrc name=thesrc emit-signals=true";
    ss << " ! queue";
    ss << " ! audioconvert";
    ss << " ! wavenc";
    ss << " ! filesink location=\"" << m_filename << "\"";
    m_pipelinestr = ss.str();
}

void OutputWav::OnStart(GstElement *pipeline) {
    ScopedLock lock(&m_mutex);
    m_thesrc = (GstAppSrc *) gst_bin_get_by_name((GstBin *) pipeline, "thesrc");
}

void OutputWav::OnStop(GstElement *pipeline) {
    ScopedLock lock(&m_mutex);
    if (m_thesrc) {
        gst_object_unref(m_thesrc);
        m_thesrc = NULL;
    }
}

void OutputWav::PushBuffer(GstCaps *caps, GstBuffer *buffer) {
    ScopedLock lock(&m_mutex);
    if (m_thesrc) {
        GstBuffer *copy = gst_buffer_copy(buffer);
        //FIXME: Timestamps
        GST_BUFFER_PTS(copy) = 0;
        GST_BUFFER_PTS(copy) = 0;

        gst_app_src_set_caps(m_thesrc, caps);
        GstFlowReturn ret = gst_app_src_push_buffer(m_thesrc, copy);
        if (ret != GST_FLOW_OK) {
            LogError("Unknown Error: %d in %s", __PRETTY_FUNCTION__);
        }
    }
}

void OutputWav::SetFileName(const std::string filename) {
    ScopedLock lock(&m_mutex);
    m_filename = filename + ".wav";
}
