The buff designer philosophy is as follows:

The buff designer is handed a list of requirements. It then creates a design and converts it into a buff blueprint.

The design is something which starts as an abstract thing and becomes more and more concrete further down the line.

BuffStrength is generally a number between 0 and 1000, generally how much percent better they make the character
0 = No effect
1 = Not noticeable effect
10 = Small effect "boring buff". Think 10% extra damage for 10 seconds
100 = Big effect. ~100% extra damage for 10 seconds or 10% extra damage for 100 seconds
500 = Enormous effect
1000 = Game-winning effect

BuffIntensity decides the ratio between strength and duration; an intense buff is a relatively strong buff over a short duration

0 = no intensity, infinite duration
0.01 = super low intensity, ~1000 second duration
0.1 = low intensity, ~100 second duration
1 = normal intensity, ~10 second duration
10 = extreme intensity, ~1 second duration
NOTE: duration does not necessarily correlate to actual time, it is just the expected lifetime of the buff.


Todo: figure out a way to handle diminishing returns; no big difference between 5x movement speed and 10x movement speed