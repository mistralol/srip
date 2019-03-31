
#include "main.h"

OutputLameRtsp::OutputLameRtsp(const std::string &uri) :
    m_uri(uri),
    m_thesrc(NULL)
{

}

OutputLameRtsp::~OutputLameRtsp() {

}

void OutputLameRtsp::Stop() {
    if (m_thesrc) {
        gst_app_src_end_of_stream(m_thesrc);
    }
    PipelineBasic::Stop();
}

void OutputLameRtsp::OnPreStart() {
    ScopedLock lock(&m_mutex);
    Logger(LOG_DEBUG, "%s", __PRETTY_FUNCTION__);
    std::stringstream ss;
    ss << "appsrc name=thesrc emit-signals=true format=3";
    ss << " ! queue max-size-buffers=32 leaky=2";
    ss << " ! audioconvert";
    ss << " ! lamemp3enc target=quality quality=0";
    ss << " ! rtspclientsink protocols=udp location=\"" << m_uri << "\"";
    m_pipelinestr = ss.str();
}

void OutputLameRtsp::OnStart(GstElement *pipeline) {
    ScopedLock lock(&m_mutex);
    Logger(LOG_DEBUG, "%s", __PRETTY_FUNCTION__);
    m_thesrc = (GstAppSrc *) gst_bin_get_by_name((GstBin *) pipeline, "thesrc");
}

void OutputLameRtsp::OnStop(GstElement *pipeline) {
    ScopedLock lock(&m_mutex);
    Logger(LOG_DEBUG, "%s", __PRETTY_FUNCTION__);
    if (m_thesrc) {
        gst_object_unref(m_thesrc);
        m_thesrc = NULL;
    }
}

void OutputLameRtsp::PushBuffer(GstCaps *caps, GstBuffer *buffer) {
    ScopedLock lock(&m_mutex);

    if (m_thesrc) {
        GstBuffer *copy = gst_buffer_copy(buffer);

        GST_BUFFER_PTS(copy) = GST_CLOCK_TIME_NONE;
        GST_BUFFER_DTS(copy) = GST_CLOCK_TIME_NONE;

        gst_app_src_set_caps(m_thesrc, caps);
        GstFlowReturn ret = gst_app_src_push_buffer(m_thesrc, copy);
        if (ret != GST_FLOW_OK) {
            LogError("Unknown Error: %d in %s", __PRETTY_FUNCTION__);
        }
    }
}

