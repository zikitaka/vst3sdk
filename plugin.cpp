#include "public.sdk/source/vst/vstaudioeffect.h"
#include <cmath>

using namespace Steinberg;
using namespace Steinberg::Vst;

#define LOW_FREQ 800.0f
#define HIGH_FREQ 5000.0f

struct EQChannel {
    float lf_delay0 = 0, lf_delay1 = 0, lf_delay2 = 0, lf_delay3 = 0;
    float hf_delay0 = 0, hf_delay1 = 0, hf_delay2 = 0, hf_delay3 = 0;
    float sample_delay1 = 0, sample_delay2 = 0, sample_delay3 = 0;
};

class ThreeBandEQ : public AudioEffect {
public:
    static FUnknown* createInstance(void*) { return (IAudioProcessor*)new ThreeBandEQ(); }

    tresult PLUGIN_API initialize(FUnknown* ctx) SMTG_OVERRIDE {
        tresult res = AudioEffect::initialize(ctx);
        if (res != kResultOk) return res;

        addAudioInput(UID("In"), SpeakerArr::kStereo);
        addAudioOutput(UID("Out"), SpeakerArr::kStereo);
        return kResultOk;
    }

    tresult PLUGIN_API process(ProcessData& data) SMTG_OVERRIDE {
        if (data.numInputs == 0 || data.numOutputs == 0) return kResultOk;

        float sr = processSetup.sampleRate;
        lf = 2.0f * sinf(float(M_PI) * LOW_FREQ / sr);
        hf = 2.0f * sinf(float(M_PI) * HIGH_FREQ / sr);

        // default gains
        float lowGain = dbToMul(low_dB);
        float midGain = dbToMul(mid_dB);
        float highGain = dbToMul(high_dB);

        for (int32 i = 0; i < data.numSamples; ++i) {
            for (int c = 0; c < 2; ++c) {
                float in = data.inputs[0].channelBuffers32[c][i];
                float out = processSample(ch[c], in, lf, hf, lowGain, midGain, highGain);
                data.outputs[0].channelBuffers32[c][i] = out;
            }
        }

        return kResultOk;
    }

private:
    EQChannel ch[2];
    float lf, hf;
    float low_dB = 0.0f, mid_dB = 0.0f, high_dB = 0.0f;

    static inline float dbToMul(float db) { return powf(10.0f, db / 20.0f); }

    static inline float processSample(EQChannel& c, float sample, float lf, float hf,
                                      float lowGain, float midGain, float highGain) {
        constexpr float EPS = 1.0f / 4294967295.0f;
        float l, m, h;

        c.lf_delay0 += lf * (sample - c.lf_delay0) + EPS;
        c.lf_delay1 += lf * (c.lf_delay0 - c.lf_delay1);
        c.lf_delay2 += lf * (c.lf_delay1 - c.lf_delay2);
        c.lf_delay3 += lf * (c.lf_delay2 - c.lf_delay3);
        l = c.lf_delay3;

        c.hf_delay0 += hf * (sample - c.hf_delay0) + EPS;
        c.hf_delay1 += hf * (c.hf_delay0 - c.hf_delay1);
        c.hf_delay2 += hf * (c.hf_delay1 - c.hf_delay2);
        c.hf_delay3 += hf * (c.hf_delay2 - c.hf_delay3);
        h = c.sample_delay3 - c.hf_delay3;
        m = c.sample_delay3 - (h + l);

        l *= lowGain;
        m *= midGain;
        h *= highGain;

        c.sample_delay3 = c.sample_delay2;
        c.sample_delay2 = c.sample_delay1;
        c.sample_delay1 = sample;

        return l + m + h;
    }
};

BEGIN_FACTORY_DEF("Zikitaka",
                  "https://github.com/zikitaka",
                  "mailto:zikitaka@example.com")

DEF_CLASS2(INLINE_UID_FROM_FUID(FUID(0x12345678,0x1234,0x5678,0x9abc)),
           PClassInfo::kManyInstances,
           kVstAudioEffectClass,
           "3BandEQ",
           Vst::kDistributable,
           "Fx",
           "1.0.0",
           "VST3",
           ThreeBandEQ::createInstance)

END_FACTORY
