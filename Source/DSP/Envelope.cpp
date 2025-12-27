#include "Envelope.h"
#include "AudioUtils.h"
#include <algorithm>

Envelope::Envelope()
{
    calculateRates();
}

void Envelope::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
    calculateRates();
}

void Envelope::setParameters(float attack, float decay, float sustain, float release)
{
    attackTime = AudioUtils::clamp(attack, 0.001f, 2.0f);
    decayTime = AudioUtils::clamp(decay, 0.001f, 2.0f);
    sustainLevel = AudioUtils::clamp(sustain, 0.0f, 1.0f);
    releaseTime = AudioUtils::clamp(release, 0.001f, 5.0f);

    calculateRates();
}

void Envelope::calculateRates()
{
    // Minimum attack time to prevent clicks
    const float minAttackTime = 0.005f;  // 5ms
    float safeAttackTime = std::max(attackTime, minAttackTime);

    // Calculate per-sample increments
    // Rate = how much the level changes per sample
    attackRate = 1.0f / (safeAttackTime * static_cast<float>(sampleRate));
    decayRate = (1.0f - sustainLevel) / (decayTime * static_cast<float>(sampleRate));
    releaseRate = sustainLevel / (releaseTime * static_cast<float>(sampleRate));
}

void Envelope::noteOn(float vel)
{
    velocity = AudioUtils::clamp(vel, 0.0f, 1.0f);
    enterPhase(Phase::Attack);
}

void Envelope::noteOff()
{
    enterPhase(Phase::Release);
}

void Envelope::reset()
{
    currentPhase = Phase::Idle;
    currentLevel = 0.0f;
}

void Envelope::enterPhase(Phase newPhase)
{
    currentPhase = newPhase;

    // Phase-specific initialization
    switch (newPhase)
    {
        case Phase::Attack:
            // Start attack from current level (for retriggering)
            // Don't reset to 0 - smoother retriggering
            break;

        case Phase::Decay:
            currentLevel = 1.0f;  // Peak level
            break;

        case Phase::Sustain:
            currentLevel = sustainLevel;
            break;

        case Phase::Release:
            // Release from current level
            // Recalculate release rate based on current level
            if (sustainLevel > 0.0f)
                releaseRate = currentLevel / (releaseTime * static_cast<float>(sampleRate));
            break;

        case Phase::Idle:
            currentLevel = 0.0f;
            break;
    }
}

float Envelope::processSample()
{
    switch (currentPhase)
    {
        case Phase::Idle:
            return 0.0f;

        case Phase::Attack:
            // Ramp up to peak
            currentLevel += attackRate;

            if (currentLevel >= 1.0f)
            {
                currentLevel = 1.0f;
                enterPhase(Phase::Decay);
            }
            break;

        case Phase::Decay:
            // Ramp down to sustain level
            currentLevel -= decayRate;

            if (currentLevel <= sustainLevel)
            {
                currentLevel = sustainLevel;
                enterPhase(Phase::Sustain);
            }
            break;

        case Phase::Sustain:
            // Hold at sustain level
            currentLevel = sustainLevel;
            break;

        case Phase::Release:
            // Ramp down to zero
            currentLevel -= releaseRate;

            if (currentLevel <= 0.0f)
            {
                currentLevel = 0.0f;
                enterPhase(Phase::Idle);
            }
            break;
    }

    // Apply velocity to final output
    return currentLevel * velocity;
}
