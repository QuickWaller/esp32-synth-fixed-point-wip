#pragma once
#include <cstdint>

#ifdef ARDUINO
#include <Arduino.h>
#endif


enum class EnvelopeStage { Idle, Attack, Decay, Sustain, Release };

class ADSR {
    public:
        ADSR(uint32_t AUDIO_SAMPLE_RATE, uint32_t attackSeconds, uint32_t decaySeconds, uint32_t releaseSeconds, uint32_t sustainLevel);
        
        // =======| Main interface for Voice to use |==========
        int32_t get_multiplier();
        void trigger_attack();
        void trigger_release();
        EnvelopeStage get_current_stage() const;

        // =======| Getters and Setters |===================
        // phase variables
        int32_t get_attack_length();
        int32_t get_decay_length();
        int32_t get_release_length();

        int32_t get_sustain_level();



    private:
        // horizontal phase variables
        int32_t attackLength_Q32_0;
        int32_t decayLength_Q32_0;
        int32_t releaseLength_Q32_0;

        // vertical phase variables
        int32_t sustainAmplitude_Q16_16;
        
        // gradients
        int32_t attackGradient_Q16_16;
        int32_t decayGradient_Q16_16;
        int32_t releaseLengthReciprocal_Q1_31;
        int32_t defaultReleaseGradient_Q16_16;

        // temporary variables
        int32_t releaseGradient_Q16_16;
        int32_t previousMultiplier_Q16_16;
        int32_t samplesElapsedPhase_Q32_0;
        

        // State tracking
        EnvelopeStage currentStage;


        
};