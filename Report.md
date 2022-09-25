Max Bakalos
U96572173
9/25/2022
EC 463

Hardware Miniproject Report

In this mini project I altered to pwm_led_fade Raspberry Pi Pico code in order to create a new light fading pattern. In my new code, the light fades up and down slowly twice, then quickly twice, then repeats. I accomplished this by creating a counter variable and increasing/decreasing fade faster after 4 counts (2 counts for up & down) and then resetting after a total of 8 counts (up 4 times and down 4 times). I also decreased the clock to half speed by dividing "sysclock" by 8 instead of 4. This gave a slower visual which is what I wanted.

This project was a success because I learned how to better use GitHub as well as how to program the Raspberry Pi Pico. My vison of alternating between two fast and two slow blinks worked out as well.