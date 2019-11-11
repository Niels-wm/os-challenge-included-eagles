#Group 6, Included Eagles
___________________________________________________________________________
##Final Submission
The following features are what is implemented in our final submission to the OS Challenge.

- Item 1
- Item 2



##Experiments
###Optimizing 'memcmp'
######Author: Lasse Pedersen, s174253

####Experiment Motivation
Our reverse hashing "algorithm" works by calculating all values between the start and the end (received in the request), and then comparing it with the hash that are to be reversed. Whenever there is a match, we send this value back to the client as the value of which the hash was generated from. This means that we do a lot of comparisons and for that we are have just been using the inbuilt C-library function 'memcmp'. The memcmp-function will compare the strings byte by byte for the given length and tell if the bytes are not the same. I would argue that this process could be optimized in the way of casting the pointers (of the values) to integers and then subtracting these from each other. This would give a value which could quickly be checked to see if it was 0 or not, without having to compare each individual byte. There is however one special case, whereby I could think of memcmp being faster. That would be in the event of the compare length is four or less, because of the way memcmp is set up to compare values in increments of four. But as long as the values are not aligned in a 4-byte boundary then I believe that the experiment implementation would be quicker (in theory).

The overall argument would be that the time gained in the comparison would outweigh the extra time spent in casting the values, which overall would result in a better performance overhead.



####Setup
The tests stated below have all run on the same machine. Three before implementing the optimization and three after. The tests copies most of the settings from the `run-client-milestone.sh`-script.

#####Run Configuration

| Setting           | Value         |
| ------------------|:-------------:|
| SERVER            | 192.168.101.10|
| PORT              | 5003          |
| SEED              | 3435245       |
| TOTAL             | 100           |
| START             | 0             |
| DIFFICULTY        | 30000000      |
| REP\_PROB\_PERCENT| 20            |
| DELAY_US          | 75000         |
| PRIO_LAMBDA       | 0.30          |


The results of the three runs can be found in the Results-section of this document. The source code of the implementation is located on GitHub on the branch: 

#####Hardware Specification
All tests have run on the same computer (using Vagrant). The specifications of the computer is listed below:

| Specification     | Value         |
| ------------------|:-------------:|
| CPU               | Intel i7      |
| CPU clock speed   | 2.5 GHz       |
| No. of CPU cores  | 4             |
| RAM amount        | 16 GB         |
| RAM type          | 1600 MHz DDR3 |
| OS                | macOS 10.14.6 |



#####Possible Errors

Although all tests have been conducted on the same machine, and with the same alternate programs running in the background, there are still some possible errors which might have interfered with the results. The main culprit I suspect would be background tasks, such as Google Drive or OS update checks which have not been disabled before running the tests. I tried as much as possible to run the tests back-to-back, with as little time as possible between runs.


####Results
Below are the results of the three runs. Thousand separators are added for easier reading.

| Run          | Before           |After             |
| -------------|:----------------:|:----------------:|
| First run    | 60.193.301 	   | 48.385.212       |
| Second run   | 61.596.063	       | 51.931.089       |
| Third run    | 55.632.214   	   | 60.438.992       |
| **Median**   |**60.193.301**    |**51.931.089**    |




####Conclusion
Looking at the results there is some noticeable difference when comparing the medians, however I still believe that these could just be within margin of error. There might be a slight change in the speed of comparison, however, seem negligible.



