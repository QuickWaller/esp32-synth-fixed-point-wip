#include "ADSR.h"

ADSR::ADSR(uint32_t AUDIO_SAMPLE_RATE, uint32_t attackSeconds, uint32_t decaySeconds, uint32_t releaseSeconds, uint32_t sustainAmplitude_Q16_15) :
    attackLength_Q32_0((int32_t)((int32_t)AUDIO_SAMPLE_RATE * attackSeconds)),
    decayLength_Q32_0((int32_t)((int32_t)AUDIO_SAMPLE_RATE * decaySeconds)),
    releaseLength_Q32_0((int32_t)((int32_t)AUDIO_SAMPLE_RATE * releaseSeconds)),
    sustainAmplitude_Q16_16(sustainAmplitude_Q16_15)
{
    releaseGradient_Q16_16 = 0;
    previousMultiplier_Q16_16 = 0;
    currentStage = EnvelopeStage::Idle;
    samplesElapsedPhase_Q32_0 = 0;

    // 32767 is the max value for a 16-bit signed int (envelope's max amplitude)
    // bitshift left by 15 to scale it up to Q16.16 format
    attackGradient_Q16_16 = (1 << 16) / attackLength_Q32_0;
    decayGradient_Q16_16 = -((1 << 16) - sustainAmplitude_Q16_16) / decayLength_Q32_0;
    releaseLengthReciprocal_Q1_31 = ((int64_t)1 << 31) / releaseLength_Q32_0;
    defaultReleaseGradient_Q16_16 = -(sustainAmplitude_Q16_16 / releaseLength_Q32_0);


}

void ADSR::trigger_attack() {
    currentStage = EnvelopeStage::Attack;
    samplesElapsedPhase_Q32_0 = 0;
}

void ADSR::trigger_release() {
    currentStage = EnvelopeStage::Release;

    // have to use 64b intermediate to avoid overflow
    // but we still beat the 32b division by precalculating the reciprocal
    if (previousMultiplier_Q16_16 == sustainAmplitude_Q16_16) {

    }
    else {
        int64_t product = (int64_t)releaseLengthReciprocal_Q1_31 * (int64_t)releaseLength_Q32_0;
        releaseGradient_Q16_16 = -(int32_t)(product >> 16);
    }
}

EnvelopeStage ADSR::get_current_stage() const {
    return currentStage;
}

int32_t ADSR::get_multiplier() {
    int32_t currentMultiplier_Q16_16 = previousMultiplier_Q16_16;
    samplesElapsedPhase_Q32_0 ++;

    switch (currentStage) {
        case (EnvelopeStage::Idle):
            currentMultiplier_Q16_16 = 0;
            break;
        case EnvelopeStage::Attack:
            if (samplesElapsedPhase_Q32_0 == attackLength_Q32_0) {
                currentStage = EnvelopeStage::Decay;
                samplesElapsedPhase_Q32_0 = 0;
                // Set to 1 to avoid overflow in the next stage
                currentMultiplier_Q16_16 = 1 << 16; // 1.0 in Q16.16 format; 
            }
            else {
                currentMultiplier_Q16_16 += attackGradient_Q16_16;  
            }
            break;
        case EnvelopeStage::Decay:
            if (samplesElapsedPhase_Q32_0 == decayLength_Q32_0) {
                currentStage = EnvelopeStage::Sustain;
                samplesElapsedPhase_Q32_0 = 0;
                // set to sustainAmplitude_Q16_16 to avoid overflow
                currentMultiplier_Q16_16 = sustainAmplitude_Q16_16;
            }
            else {
                currentMultiplier_Q16_16 += decayGradient_Q16_16;
            }
            break;
        case EnvelopeStage::Sustain:
            currentMultiplier_Q16_16 = sustainAmplitude_Q16_16;
            break;
        case EnvelopeStage::Release:
            if (samplesElapsedPhase_Q32_0 == releaseLength_Q32_0) {
                currentStage = EnvelopeStage::Idle;
                samplesElapsedPhase_Q32_0 = 0;
                currentMultiplier_Q16_16 = 0; // return to idle state
            }
            else {
                currentMultiplier_Q16_16 += releaseGradient_Q16_16;
            }
            break;
    }
    // update previous multiplier for next call + return
    previousMultiplier_Q16_16 = currentMultiplier_Q16_16;
    return currentMultiplier_Q16_16;
}


int32_t ADSR::get_attack_length()  { return attackLength_Q32_0; };
int32_t ADSR::get_decay_length()   { return decayLength_Q32_0; };
int32_t ADSR::get_release_length() { return releaseLength_Q32_0; };
int32_t ADSR::get_sustain_level()  { return sustainAmplitude_Q16_16; };