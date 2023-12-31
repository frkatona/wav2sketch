// forked from https://github.com/GadgetReboot/Piezo_Interface

#include <Audio.h>

#include "cpp_output\crash_75.h"            // https://freesound.org/s/506453/  closed hi-hat cymbal "chik" sound
#include "AudioSampleCrash14_choke.h"           // https://freesound.org/s/28717/   14 inch Paiste crash cymbal choked (muted by hand grip after striking)
#include "AudioSampleCrash14.h"                 // https://freesound.org/s/28715/   14 inch Paiste crash cymbal ringing out
#include "AudioSampleRide.h"                    // https://freesound.org/s/28740/   19 inch Sabian AA medium ride cymbal
#include "AudioSampleBongo.h"                   // https://freesound.org/s/28707/   bongo (used as a high pitch tom)
#include "AudioSampleTommid.h"                  // https://freesound.org/s/41437/   tom drum mid tone
#include "AudioSampleTom12.h"                   // https://freesound.org/s/428623/  12 inch vintage tom with coated emperor head
#include "AudioSampleTom16.h"                   // https://freesound.org/s/428621/  16 inch vintage tom with coated emperor head
#include "AudioSampleKick.h"                    // https://freesound.org/s/479916/  22x18 Yamaha Recording Custom bass drum
#include "AudioSampleSnare.h"                   // https://freesound.org/s/82238/   snare drum

#define adcMax                 1023             // ADC max reading at 10 bits
#define somePercentOfMax       (adcMax * 0.04)  // tweakable percentage of the full ADC range, used as a threshold level when triggering

// individual instrument playback levels - tweaked as needed to match volumes
#define AudioSampleKickLevel             1.0//0.3
#define AudioSampleSnareLevel            1.0//0.4
#define AudioSampleTom12Level            1.0//0.5
#define AudioSampleHihat_closedLevel     1.0//0.4
#define AudioSampleCrash14_chokeLevel    1.0//0.4
#define AudioSampleCrash14Level          1.0//0.4
#define AudioSampleTommidLevel           1.0//0.3
#define AudioSampleTom16Level            1.0//0.4

const byte numChannels = 8;                     // number of trigger input channels to read
const byte inputPin[numChannels] =              // assign ADC pins to input channels
{ 0, 1, 2, 3, 4, 5, 10, 11};                    // A0 thru A5, A10, A11

// audio player modules
// players can run simultaneously with a dedicated sound playing on each
// the assigned sound sample on a player can be changed
// by calling the player with a new assigned sound sample
AudioPlayMemory    sound0;
AudioPlayMemory    sound1;
AudioPlayMemory    sound2;
AudioPlayMemory    sound3;
AudioPlayMemory    sound4;
AudioPlayMemory    sound5;
AudioPlayMemory    sound6;
AudioPlayMemory    sound7;

// two 4-channel mixers take the 8 audio sample player channels
// and combine them into a single channel on a 3rd mixer for playback
// audio is treated as mono and duplicated on left and right I2S output channels
AudioMixer4        mix1;
AudioMixer4        mix2;
AudioMixer4        mix3;

// I2S DAC board interface
AudioOutputI2S     i2s1;

// audio path patch cords
// connect the output of audio sample players
// to the two 4-ch mixer inputs, then take the
// 4-ch mixer outputs into a 3rd mixer, and out
// to the I2S audio player
AudioConnection patchCord1(sound0, 0, mix1, 0);   // 4 sound samples to 4-ch mixer inputs
AudioConnection patchCord2(sound1, 0, mix1, 1);
AudioConnection patchCord3(sound2, 0, mix1, 2);
AudioConnection patchCord4(sound3, 0, mix1, 3);

AudioConnection patchCord5(sound4, 0, mix2, 0);   // 4 sound samples to 4-ch mixer inputs
AudioConnection patchCord6(sound5, 0, mix2, 1);
AudioConnection patchCord7(sound6, 0, mix2, 2);
AudioConnection patchCord8(sound7, 0, mix2, 3);

AudioConnection patchCord9(mix1, 0, mix3, 0);     // two 4-ch mixers out to final mixer inputs
AudioConnection patchCord10(mix2, 0, mix3, 1);    // to combine 8 mono channeld down to single mono

AudioConnection patchCord11(mix3, 0, i2s1, 0);    // output of mono sound into I2S DAC board left+right
AudioConnection patchCord12(mix3, 0, i2s1, 1);


void setup() {
  Serial.begin(9600);

  pinMode(32, INPUT_PULLUP);  // pulling low initiates demo mode

  AudioMemory(10);

  // set mixer input levels for each instrument
  mix1.gain(0, AudioSampleKickLevel);
  mix1.gain(1, AudioSampleSnareLevel);
  mix1.gain(2, AudioSampleTom12Level);
  mix1.gain(3, AudioSampleHihat_closedLevel);

  mix2.gain(0, AudioSampleCrash14_chokeLevel);
  mix2.gain(1, AudioSampleCrash14Level);
  mix2.gain(2, AudioSampleTommidLevel);
  mix2.gain(3, AudioSampleTom16Level);

  // set master output left/right levels
  // (controlled elsewhere as well)
  mix3.gain(0, 1); //0.4
  mix3.gain(1, 1); //0.4
  mix3.gain(2, 0);
  mix3.gain(3, 0);

  AudioInterrupts();

  Serial.println("Drum Trigger Demo");
}


void loop() {
  if (!digitalRead(32)) demoSamples();   // play a demo composition to test sample sounds if pin is low
  scanChannels();                        // read adc inputs and trigger sample playback if needed
}

// -----------------------------------------
// functions to play triggered sound samples
// -----------------------------------------

void (*playSample[])() =
{&playSample0, &playSample1, &playSample2, &playSample3, &playSample4, &playSample5, &playSample6, &playSample7};

void playSample0() {
  mix1.gain(0, AudioSampleKickLevel);
  sound0.play(AudioSampleKick);
}

void playSample1() {
  mix1.gain(1, AudioSampleSnareLevel);
  sound1.play(AudioSampleSnare);
}

void playSample2() {
  mix1.gain(2, AudioSampleTom12Level);
  sound2.play(AudioSampleTom12);
}
void playSample3() {
  mix1.gain(3, AudioSampleHihat_closedLevel);
  sound3.play(AudioSampleHihat_closed);
}

void playSample4() {
  mix2.gain(0, AudioSampleCrash14_chokeLevel);
  sound4.play(AudioSampleCrash14_choke);   // choke and crash play from same "sound4" sample player - same cymbal
}

void playSample5() {
  mix2.gain(1, AudioSampleCrash14Level);
  sound4.play(AudioSampleCrash14);         // choke and crash play from same "sound4" sample player - same cymbal
}

void playSample6() {
  mix2.gain(2, AudioSampleTommidLevel);
  sound6.play(AudioSampleTommid);
}

void playSample7() {
  mix2.gain(3, AudioSampleTom16Level);
  sound7.play(AudioSampleTom16);
}
// ---------------------------------------------
// end functions to play triggered sound samples
// ---------------------------------------------


// read all analog trigger inputs and monitor how the signals
// progress, triggering sample playback when appropriate
// I know, this begs to be implemented as a state machine...
// I'd also like to reach 100k subs in 2021 but...
void scanChannels() {

  enum channelState {ch_idle, ch_triggered};                       // list of possible channel states
  static unsigned int peakLevel[numChannels] = { 0 };              // store peak adc readings for each channel
  static byte chState[numChannels] = { 0 };                        // current state of each channel, start at idle

  // read and process each input channel
  // looking for changes in peaks as the input signal rises or falls
  // and when it has fallen enough from the most recent peak,
  // conditions are met for triggering a sound to be played
  for (int i = 0; i < numChannels; i++) {
    unsigned int chReading = analogRead(inputPin[i]);              // read current adc input

    // if the channel is idle (no detected input signal to monitor for triggering),
    //  if a new signal above the minimum trigger threshold is detected,
    //    begin the triggering process and the signal will be monitored as it progresses
    //    through future input readings
    // if the signal is triggered
    //  track whether it's continuing to rise or fall
    //  if rising, hold the new peaks and keep monitoring
    //  if falling far enough, trigger a sound sample playback (input trigger is settling)

    if (chState[i] == ch_idle) {
      if (chReading > (peakLevel[i] + somePercentOfMax)) {          // if input rises significantly while idle
        chState[i] = ch_triggered;                                  // set channel as triggered (signal may still fluctuate)
        peakLevel[i] = chReading;                                   // set channel peak level to new reading
      }
      else if (chReading <= (peakLevel[i] - somePercentOfMax)) {    // if input falls significantly while idle
        peakLevel[i] = chReading;                                   // trail channel peak level to new lower reading
      }
    }

    // if triggered, continue tracking the highest peak detected
    // until the input drops by some % of the peak, then consider
    // the signal settled and activate the trigger (play sample)
    if (chState[i] == ch_triggered) {                               // if channel is triggered (and the input may not have settled yet)
      if (chReading > peakLevel[i]) {                               // if there is a higher peak detected during settling time, store it as the new peak
        peakLevel[i] = chReading;
      }
      else if (chReading <= (peakLevel[i] - somePercentOfMax)) {    // if the input level has dropped some amount lower than the peak, consider trigger finalized
        float level = (peakLevel[i] / float(adcMax));               // set sample playback level based on peak level detected (touch sensitive playback)

        Serial.print("A"); Serial.print(inputPin[i]);
        Serial.print(" Triggered at peak level ");
        Serial.print (peakLevel[i]); Serial.print("/1023 Max. Playing sample at ");
        Serial.print(round(level * 100)); Serial.print("% level.");
        Serial.println();

        mix3.gain(0, level);         // set left/right output level for playback
        mix3.gain(1, level);
        playSample[i]();             // call function to play back sound sample assigned to this channel number [i]
        peakLevel[i] = chReading;    // trail the peak lower now that the signal has fallen enough to have triggered
        chState[i] = ch_idle;        // triggering is complete so channel is idle, ready to re-trigger
      }
    }
  }
}

// Demo mode - playback samples in a composition
void demoSamples()
{
  // sample playback tests
  // doesn't matter what "sound" player they use
  // just need to assign a sample to a player and let it go

  Serial.println("Demo mode");
  // set output level
  mix3.gain(0, 0.4);
  mix3.gain(1, 0.4);

  // ride cymbal test
  sound3.play(AudioSampleRide);
  delay(500);
  sound3.play(AudioSampleRide);
  delay(350);
  sound3.play(AudioSampleRide);
  delay(150);
  sound3.play(AudioSampleRide);
  delay(500);

  // choked crash cymbal and bass drum
  sound0.play(AudioSampleKick);
  sound1.play(AudioSampleCrash14_choke);
  delay(500);
  sound2.play(AudioSampleKick);
  sound3.play(AudioSampleCrash14);
  delay(500);

  // hi-hat chik chik
  sound0.play(AudioSampleHihat_closed);
  delay(100);
  sound0.play(AudioSampleHihat_closed);
  delay(200);

  // tom fill
  sound0.play(AudioSampleTommid);
  delay(100);
  sound0.play(AudioSampleTommid);
  delay(100);
  sound1.play(AudioSampleTom12);
  delay(100);
  sound1.play(AudioSampleTom12);
  delay(100);
  sound2.play(AudioSampleBongo);
  delay(90);
  sound2.play(AudioSampleBongo);
  delay(90);
  sound2.play(AudioSampleBongo);
  delay(90);
  sound3.play(AudioSampleTom12);
  delay(100);
  sound3.play(AudioSampleTom12);
  delay(100);
  sound0.play(AudioSampleTommid);
  delay(100);
  sound0.play(AudioSampleTommid);
  delay(100);
  sound1.play(AudioSampleTom16);
  delay(100);
  sound1.play(AudioSampleTom16);
  delay(100);
  sound2.play(AudioSampleKick);
  delay(200);
  sound3.play(AudioSampleSnare);
  delay(100);
  sound0.play(AudioSampleKick);
  delay(200);
  delay(200);

  // bass/choked crash
  sound0.play(AudioSampleKick);
  sound1.play(AudioSampleCrash14_choke);
  delay(200);

  sound2.play(AudioSampleKick);
  sound3.play(AudioSampleCrash14_choke);
  delay(200);
  delay(200);

  // tom fill
  sound1.play(AudioSampleBongo);
  delay(30);
  sound1.play(AudioSampleBongo);
  delay(100);
  sound3.play(AudioSampleTom12);
  delay(30);
  sound3.play(AudioSampleTom12);
  delay(100);
  sound5.play(AudioSampleTom12);
  delay(30);
  sound5.play(AudioSampleTom12);
  delay(100);
  sound7.play(AudioSampleTommid);
  delay(30);
  sound7.play(AudioSampleTommid);
  delay(100);
  sound1.play(AudioSampleTommid);
  delay(30);
  sound1.play(AudioSampleTommid);
  delay(100);
  sound3.play(AudioSampleTom16);
  delay(30);
  sound3.play(AudioSampleTom16);
  delay(100);
  sound5.play(AudioSampleTom16);
  delay(30);
  sound5.play(AudioSampleTom16);
  delay(100);

  // bass/choked crash
  sound0.play(AudioSampleKick);
  delay(200);
  sound1.play(AudioSampleKick);
  sound2.play(AudioSampleCrash14_choke);
  delay(200);
  delay(200);

  sound3.play(AudioSampleKick);
  delay(200);
  sound4.play(AudioSampleKick);
  sound5.play(AudioSampleCrash14);
  delay(200);
  delay(500);

  // bass/snare
  sound0.play(AudioSampleKick);
  delay(200);
  sound1.play(AudioSampleSnare);
  delay(30);
  sound2.play(AudioSampleSnare);
  delay(120);
  sound3.play(AudioSampleKick);
  delay(200);

}
