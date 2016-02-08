## State Machine Sequences and Wave-forms

This section provides additional explanation to particular functions of [API Functions](readme.md#functions) by presenting screenshots of state machine sequence diagrams and wave-forms. All wave-forms were taken during operation of application with DimSwitch library controlling OSRAM QUICKTRONIC - INTELLIGENT QTi DALI 2x28/54 DIM electronic ballast.


### Table of Contents

* [Toggle Relay](#toggle-relay)
  * [Sequence](#toggle-relay-sequence)
  * [Wave-forms](#toggle-relay-wave-forms)
* [Power On](#power-on)
  * [Sequence](#power-on-sequence)
  * [Wave-forms](#power-on-wave-forms)
* [Power Off](#power-off)
  * [Sequence](#power-off-sequence)
  * [Wave-forms](#power-off-wave-forms)
* [Calibrate](#calibrate)
  * [Sequence](#calibrate-sequence)
  * [Wave-forms](#calibrate-wave-forms)
* [Set Intensity](#set-intensity)
  * [Sequence](#set-intensity-sequence)
  * [Wave-forms](#set-intensity-wave-forms)


### Toggle Relay

#### Toggle Relay Sequence

![State Machine Sequence - Toggle Relay](pictures/sms-toggle-on.png)


#### Toggle Relay Wave-forms

![Wave-forms - Toggle Relay](pictures/wvf-1-power-on.png)


### Power On

#### Power On Sequence

![State Machine Sequence - Power On](pictures/sms-power-on.png)


#### Power On Wave-forms

![Wave-forms - Power On](pictures/wvf-1-power-on.png)


### Power Off

#### Power Off Sequence

![State Machine Sequence - Power Off](pictures/sms-power-off.png)


#### Power Off Wave-forms

![Wave-forms - Power Off](pictures/wvf-2-power-off-max.png)
![Wave-forms - Power Off](pictures/wvf-3-power-off-min.png)


### Calibrate

#### Calibrate Sequence

![State Machine Sequence - Calibrate](pictures/sms-calibrate.png)


#### Calibrate Wave-forms

![Wave-forms - Calibrate](pictures/wvf-8-calibrate.png)
![Wave-forms - Calibrate](pictures/wvf-9-calibrate-another.png)


### Set Intensity

#### Set Intensity Sequence

![State Machine Sequence - Set Intensity](pictures/sms-set-intensity.png)


#### Set Intensity Wave-forms

Sequence of setting light intensity depends on initial lamp state (if it is off or already on) and initial direction of intensity change. In later case if initial direction is opposite from desired, the relay is momentary toggled off to change it. See the following wave-forms that illustrate particular cases.

Set the light intensity when the lamp is initially off (below).

![Wave-forms - Set Intensity](pictures/wvf-4-set-intensity-from-off.png)

Set the light intensity when the lamp is already on (below).

![Wave-forms - Set Intensity](pictures/wvf-5-set-intensity-when-on.png)

Set the light intensity with altering direction change (below).

![Wave-forms - Set Intensity](pictures/wvf-6-set-intensity-altering.png)

Set the light intensity with altering direction change - another example (below).

![Wave-forms - Set Intensity](pictures/wvf-7-set-intensity-altering-another.png)
