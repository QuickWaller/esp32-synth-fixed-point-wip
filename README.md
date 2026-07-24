# ESP32 Synth — Fixed-Point Rework 🚧 — work in progress

A planned rework of [esp32-fm-synth](https://github.com/QuickWaller/esp32-fm-synth)
(the working instrument — start there if you want to see something that
actually plays), this time using **fixed-point arithmetic** instead of
floating point for the audio-generation path.

**Current state: early and incomplete.** This is not a working synth
yet — most of the original project (oscillators, voices, Bluetooth
output, button input) hasn't been ported over. What exists:

## What's actually built

A from-scratch **fixed-point ADSR envelope generator** (`ADSR.h`/`ADSR.cpp`)
— the amplitude envelope that shapes how a note fades in and out
(attack → decay → sustain → release). Implemented entirely in **Q16.16
and Q32.0 fixed-point** rather than floating point:

- Precomputed integer gradients for each envelope phase, so the
  per-sample update is a single integer add rather than a
  multiply/divide
- A precomputed reciprocal for the release-phase gradient (`releaseLengthReciprocal_Q1_31`),
  avoiding a division on every note-release
- A 64-bit intermediate product (`int64_t`) when combining the
  reciprocal back down to Q16.16, specifically to avoid overflow that a
  32-bit intermediate would hit

This is genuinely the interesting part of this repo — fixed-point Q-format
arithmetic is the kind of thing that matters on microcontrollers without
hardware floating-point, and doing it correctly (avoiding both overflow
and precision loss) takes more care than it looks like from the outside.

## What's not built yet

- No oscillators, no `Voice` class, no FM synthesis — `main.cpp` is
  still the default PlatformIO template
- No audio output of any kind (no Bluetooth, no DAC) — the ADSR module
  runs standalone, nothing consumes its output yet
- No button input handling

## Building

The ADSR module compiles standalone (`pio run`). There's nothing to
flash and hear yet — this repo currently proves the envelope math works
in isolation, not a playable instrument.

## Where this goes

Either: finish porting the rest of `ESP32-Project-Synth` across to use
this fixed-point ADSR (making this the "v2" of that project), or fold
this ADSR module directly into `ESP32-Project-Synth` and retire this
repo. Not yet decided — flagging honestly rather than presenting this as
further along than it is.
