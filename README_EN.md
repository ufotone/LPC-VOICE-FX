# LPC VOICE FX

[日本語](README.md) | [English](README_EN.md) | [Download](https://github.com/ufotone/LPC-VOICE-FX/releases/latest)

A voice effect for drumlogue that transforms incoming speech into a robotic voice.

## Installation

1. Download and extract the ZIP from [Releases](https://github.com/ufotone/LPC-VOICE-FX/releases/latest).
2. Turn off drumlogue, then hold REC while turning it on to enter USB mass-storage mode.
3. Copy `lpc_voice_fx.drmlgunit` to `Units/MasterFXs/` on the device.
4. Safely eject the device and press PLAY/STOP to leave USB mode.
5. Select **LPC VOICE FX** as the Master FX.

To use a microphone or another external source, connect it to AUDIO IN, set `ROUTE` to `SIDECH` in the AUDIO IN settings (`SHIFT` + `BD`), and set the effect's `INPUT` to `SIDECHAIN`.

## Parameters

| Page | Name | Range | Default | Description |
|---|---|---|---|---|
| 1 | INPUT | SIDECHAIN / MAIN | SIDECHAIN | Select the source: external sidechain input or the main mix. |
| 1 | IN GAIN | 0 to +60 dB | +18 dB | Input gain. |
| 1 | PITCH | −24 to +36 semitones | 0 | Robotic voice pitch; 0 is approximately 130 Hz. |
| 1 | MIX | 0–100% | 100% | SIDECHAIN: amount of processed voice added. MAIN: balance between the dry and processed signal. |
| 2 | LEVEL | 0–100% | 80% | Level of the processed voice, including reverb. |
| 2 | LOW CUT | 20–1000 Hz | 120 Hz | Cuts low frequencies from the processed voice. Higher settings remove more low end. |
| 2 | REV MIX | 0–100% | 25% | Reverb amount on the processed voice; 0% turns it off. |
| 2 | REV TIME | 200–4000 ms | 1500 ms | Reverb decay time. |

In SIDECHAIN mode, the processed voice is added to the main mix, such as drums. LOW CUT and reverb affect the processed voice. In MAIN mode, the left and right main signals are combined for processing.
