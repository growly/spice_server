; First line is special

.lib /home/arya/.volare/sky130A/libs.tech/ngspice/sky130.lib.spice tt

.include inverter.sp

* Power rail.
* node 0 is vpwr
vpower vpwr 0 dc 1.8

* Input pattern.
vin in 0 pat (1.8 0 0 50p 50p 1n b010101010101010101010 0)

* Output is 2 femtofarads.
c0 out 0 2f

* Device under test.
xdut in out vpwr 0 inverter

.tran 1e-12 60e-9
.print tran format=csv v(*) i(*)
