
#include "main.h"

static void Noop(GstCaps *, GstBuffer *) {
    LogDebug("%s", __PRETTY_FUNCTION__);
}


PulseSource::PulseSource() {
    SetRestart(false);
    SetName("PulseSource");
    SetFunction(std::bind(&Noop, std::placeholders::_1, std::placeholders::_2));

    std::stringstream ss;
    ss << "pulsesrc";
    ss << " ! queue name=thequeue max-size-buffers=5000 leaky=1";
    ss << " ! appsink name=thesink emit-signals=true sync=false";

    m_pipelinestr = ss.str();
}

PulseSource::~PulseSource() {

}

void PulseSource::SetFunction(std::function<void(GstCaps *, GstBuffer *)> func) {
    ScopedLock lock(&m_mutex);
    m_callback = func;
}

void PulseSource::OnStart(GstElement *pipeline) {
    GstElement *queue = gst_bin_get_by_name((GstBin *) pipeline, "thequeue");
    GstAppSink *sink = (GstAppSink *) gst_bin_get_by_name((GstBin *) pipeline, "thesink");
    Logger(LOG_DEBUG, "%s", __PRETTY_FUNCTION__);

    if (queue == NULL) {
        LogCritical("Cannot find 'thequeue' gstreamer element");
        abort();
    } else {
        g_signal_connect(queue, "overrun", G_CALLBACK(PulseSource::OnOverRun), this);
        gst_object_unref(queue);
    }

    if (sink == NULL) {
        LogCritical("Cannot find 'thesink' gstreamer element");
        abort();
    } else {
        g_signal_connect(sink, "new-sample", G_CALLBACK(PulseSource::OnNewSample), this);
        gst_object_unref(sink);
    }
}


void PulseSource::OnOverRun(GstElement* object, gpointer user_data) {
    LogCritical("Queue overrun in %s", __PRETTY_FUNCTION__);
}

GstFlowReturn PulseSource::OnNewSample(GstElement* object, gpointer user_data) {
    GstAppSink *sink = (GstAppSink *) object;
    PulseSource *self = (PulseSource *) user_data;

    //Note: When the sample is unref'ed so are the buffers and caps
    GstSample *sample = gst_app_sink_pull_sample(sink);
    GstCaps *caps = gst_sample_get_caps(sample);
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    if (caps && buffer) {
        ScopedLock lock(&self->m_mutex);
        self->m_callback(caps, buffer);
    } else {
        LogError("CAPS: %p BUFFER: %p", caps, buffer);
    }

    gst_sample_unref(sample);

    return GST_FLOW_OK;
}

