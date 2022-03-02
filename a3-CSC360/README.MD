I felt this assignment went pretty well for me.
Most of the logic within my rrsim is explained in comments but it is fundamentally just a series of loops for each
different status that a task can have. Something cool that I did make was a save system. The code will make and update a savefile each time it is ran so it can continuously keep all of its different outputs stored for your viewing pleasure.
The only bug I had in the code was the loop got stuck in like one senario that I couldn't replicate in any of the 500+ simulations I ran.


For the graphs I included the python script that I made to run it. It can be ran with:
python3 RRSim_Tester.py -seed #### -tasks #### (tasks might be task)
It uses matplotlib which isnt in vagrantshire by default
Its worth noting that this will delete the savefile and delete any outputs that may be existing. It also creates 480 output files. It runs simulations then reads the outputs of the simulator to get the data
