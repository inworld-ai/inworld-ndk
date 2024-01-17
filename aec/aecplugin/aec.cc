/**
 * Copyright 2023-2024 Theai, Inc. dba Inworld AI
 *
 * Use of this source code is governed by the Inworld.ai Software Development Kit License Agreement
 * that can be found in the LICENSE.md file or at https://www.inworld.ai/sdk-license
 */

#include "aec.h"

#define WEBRTC_APM_DEBUG_DUMP 0
#include "modules/audio_processing/include/audio_processing.h"
using namespace webrtc;


typedef struct {
    webrtc::StreamConfig stream_config;
    rtc::scoped_refptr<AudioProcessing> apm;
} AECHandle;

void* WebRtcAec3_Create(int32_t sample_rate_hz) {

    auto apm = AudioProcessingBuilder().Create();
    AudioProcessing::Config config;
    config.echo_canceller.enabled = true;
    config.echo_canceller.mobile_mode = false;
    config.gain_controller1.enabled = true;
    config.gain_controller2.enabled = true;
    config.high_pass_filter.enabled = true;
    config.noise_suppression.enabled = true;
    apm->ApplyConfig(config);
    apm->Initialize();
    
    auto handle = new AECHandle();
    handle->stream_config = webrtc::StreamConfig(sample_rate_hz, 1);
    handle->apm = apm;
    return handle;
}

void WebRtcAec3_Free(void* handle)
{
    auto aec = (AECHandle *)handle;
    aec->apm = NULL;
    delete aec;
}

int32_t WebRtcAec3_BufferFarend(void* handle, const int16_t* farend)
{
    auto aec = (AECHandle*)handle;
    return aec->apm->ProcessReverseStream(farend, aec->stream_config, aec->stream_config, (int16_t *)farend);
}

int32_t WebRtcAec3_Process(
    void* handle,
    const int16_t* nearend,
    int16_t* out) {

    auto aec = (AECHandle*)handle;
    return aec->apm->ProcessStream(nearend, aec->stream_config, aec->stream_config, out);
}

void WebRtcAec3_GetMetrics(void* handle, Metrics* metrics)
{
    auto aec = (AECHandle*)handle;
    auto result = aec->apm->GetStatistics();
    metrics->delay_ms = result.delay_ms.value_or(0);
    metrics->echo_return_loss = result.echo_return_loss.value_or(0.);
    metrics->echo_return_loss_enhancement = result.echo_return_loss_enhancement.value_or(0.);
}