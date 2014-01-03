DigiChron is a Pebble app that is intended to operate similarly to a Casio
digital watch. A Casio GWM5610 G-Shock was used as my model. The app has a
passing resemblence to the Casio, but the main idea is to operate as a real
digital watch does and to provide a similar feature set.

This is a watchapp, not a watchface, because it needs to have interaction in
order to set timers, operate the stopwatch, etc. As such it would normally exit
if the user clicks the back button. I have overridden that behavior to make it
less likely that the user exits by accident. To exit the app, the user must
long-click the back button.

General Features:

- Open source under the MIT license.
- Multiple functions available by clicking the DOWN button.
- All functions can be used at the same time. No blocking.
- Shows watch battery percentage and charging state on status bar.
- Shows bluetooth connectivity on status bar.
- Vibrates on bluetooth connectivity changes.
- Does not require the phone to function, other than to install.
- Supports both white-on-black and black-on-white display modes.

Main Features:

- Shows time, day or date, and seconds.
- Obeys the 12/24H setting in the Pebble settings.

Timer Features:

- Two independent countdown timers, maximum time 24 hours.
- Pressing any key turns off active alarms even if the user is not
  on the timer screen.
- Timers remember their settings on app exit and resume on restart.
- Hold UP and DOWN buttons to increment or decrement faster.

Stopwatch Features:

- Stopwatch with lap and split times, maximum time 100 hours.
- Stopwatch updates 5x per second, displays to 1/10 while running,
  but captures time to 1/100 when the stop button is clicked.
- To save battery, the stopwatch update slows to 1x per second after five
  minutes, but still maintains full accuracy in the background.
- Stopwatch will continue after exiting and restarting the app.

Button Mappings:

	All Faces:
		BACK		Clear alarms that are currently active.
		DBL-BACK	Invert display colors.
		LONG-BACK	Exit the app.
		DOWN		Next function.

	Main Face:
		SELECT		Change between date and day display.

	Timer Faces:
		SELECT		Start and stop the timer. In setting mode
				switch between hours, minutes, and seconds.
		LONG-SEL	Enter or exit setting mode.
		UP		Increment the time in setting mode.
		DOWN		Decrement the time in setting mode.

	Stopwatch Face:
		SELECT		Start, stop, and continue the timing.
		LONG-SELECT	Reset the stopwatch.
		UP		When the timing has been stopped with SELECT,
				switch between split and lap display.

Future Features:

These are some features that I would like to add. I have tried to architect
the code to make it as easy as possible to add them.

- Alarm function.
- Second time zone function.
- Multiple lap times in the stopwatch.
- Alternate look & feel, such as an analog display version.

