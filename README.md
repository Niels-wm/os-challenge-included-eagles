### Group 6, Included Eagles
___________________________________________________________________________
# Final Submission
The following features are what is implemented in our final submission to the OS Challenge.

- Item 1
- Item 2



# Experiments
## Thread Prioritization
###### Author: Lasse Pedersen, s174253

#### Experiment Motivation
I found out about the possibility to prioritize each individual thread in Linux using the Pthread-library by changing the threads attributes. This however requires you to explicitly set the scheduler policy, which can also be done through the Pthread library. The reason for the experiment was that I was interested in knowing if the inbuilt scheduler could be manipulated into prioritizing those specific threads who are specifically assigned to running the reverse hashing algorithm. The mindset was that if these threads could be given priority over system tasks, an improvement in performance would hopefully be noticeable. But before being able to set the thread-priority you first have to assign the scheduler policy. The default Linux policy is 'SCHED\_OTHER', which is a time-sharing round-robin scheduler who will assign each task a certain amount of time pending on what other tasks that are running in the system. There are a number of alternative policies, but I will mainly be focusing on SCHED\_FIFO and SCHED\_RR, who both are real-time policies that will pre-empt (interrupt) every other task. These will allow me to set their individual thread priority (SCHED\OTHER does not allow for setting priorities). For proof of concept the thread priority will be loosely based on the task priority. 



#### Setup
The tests stated below have all run on the same machine. Three before implementing thread prioritization, three after with the Round-Robin scheduler and another three after with the FIFO scheduler. The tests copies most of the settings from the `run-client-milestone.sh`-script, but the *PRIO\_LAMBDA* have been changed. *PRIO\_LAMBDA* have changed to create an increase in more different package priorities. Which is needed in order for the thread prioritization to be effective.

##### Run Configuration
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


The results of the nine runs can be found in the Results-section.

##### Hardware Specification
All tests have run on the same computer (using Vagrant). The specifications of the computer is listed below:

| Specification     | Value         |
| ------------------|:-------------:|
| CPU               | Intel i7      |
| CPU clock speed   | 2.5 GHz       |
| No. of CPU cores  | 4             |
| RAM amount        | 16 GB         |
| RAM type          | 1600 MHz DDR3 |
| OS                | macOS 10.14.6 |


##### Possible Errors
Although all tests have been conducted on the same machine, with WIFI and Bluetooth disabled. There is still the possibility for errors coming from background OS tasks. These I have tried to keep at a minimum by disabling as much as possible, such as anti-virus programs and etc. The tests have been conducted on a laptop with the battery being at 100% and plugged in to power and on a cooling pad in order to try and manage thermal conditions. The tests were run back-to-back.


#### Results
Below are the results of the nine runs. Thousand separators are added for easier reading.

| Run          | Before           |With Round-Robin  |With FIFO        |
| -------------|:----------------:|:----------------:|:---------------:|
| First run    | 85.753.653 	   | 86.106.845       | 86.699.428      |
| Second run   | 85.787.545	       | 85.518.831       | 86.446.192      |
| Third run    | 87.488.100   	   | 85.497.092       | 86.571.903      |
| **Median**   |**85.787.545**    |**85.518.831**    |**86.571.903**   |

The results also indicated that nearly all prioritization levels were used in the tests. Ranging between level 1 and level 16.


#### Conclusion
The results indicated that there is no real performance benefit from manipulating the OS scheduler. The numbers fluctuate between being either slightly faster or slightly slower with the scheduler policy and thread prioritization set. So, in conclusion, I think the OS scheduler is perfectly alright at self-managing individual tasks and in an environment were all background tasks were kept at a minimum then the OS scheduler had really no other tasks to down prioritize (with or without the experiment implementation).


## Optimizing 'memcmp'
###### Author: Lasse Pedersen, s174253

#### Experiment Motivation
Our reverse hashing "algorithm" works by calculating all values between the start and the end (received in the request), and then comparing it with the hash that are to be reversed. Whenever there is a match, we send this value back to the client as the value of which the hash was generated from. This means that we do a lot of comparisons and for that we are have just been using the inbuilt C-library function 'memcmp'. The memcmp-function will compare the strings byte by byte for the given length and tell if the bytes are not the same. I would argue that this process could be optimized in the way of casting the pointers (of the values) to integers and then subtracting these from each other. This would give a value which could quickly be checked to see if it was 0 or not, without having to compare each individual byte. There is however one special case, whereby I could think of memcmp being faster. That would be in the event of the compare length is four or less, because of the way memcmp is set up to compare values in increments of four. But as long as the values are not aligned in a 4-byte boundary then I believe that the experiment implementation would be quicker (in theory).

The overall argument would be that the time gained in the comparison would outweigh the extra time spent in casting the values, which overall would result in a better performance overhead.



#### Setup
The tests stated below have all run on the same machine. Three before implementing the optimization and three after. The tests copies most of the settings from the `run-client-milestone.sh`-script. Please see the implementation on the `memcmp-optimization`-branch.


##### Run Configuration

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
| PRIO_LAMBDA       | 1.5           |


The results of the three runs can be found in the Results-section of this document. The source code of the implementation is located on GitHub on the branch: 

##### Hardware Specification
All tests have run on the same computer (using Vagrant). The specifications of the computer is listed below:

| Specification     | Value         |
| ------------------|:-------------:|
| CPU               | Intel i7      |
| CPU clock speed   | 2.5 GHz       |
| No. of CPU cores  | 4             |
| RAM amount        | 16 GB         |
| RAM type          | 1600 MHz DDR3 |
| OS                | macOS 10.14.6 |



##### Possible Errors
Although all tests have been conducted on the same machine, with WIFI and Bluetooth disabled. There is still the possibility for errors coming from background OS tasks. These I have tried to keep at a minimum by disabling as much as possible, such as anti-virus programs and etc. The tests have been conducted on a laptop with the battery being at 100% and plugged in to power and on a cooling pad in order to try and manage thermal conditions. The tests were run back-to-back.


#### Results
Below are the results of the three runs. Thousand separators are added for easier reading.

| Run          | Before           |After             |
| -------------|:----------------:|:----------------:|
| First run    | 16.534.411 	   | 15.647.855       |
| Second run   | 16.470.303       | 15.530.894       |
| Third run    | 16.580.817   	   | 15.636.911       |
| **Median**   |**16.534.411**    |**15.636.911**    |

#### Conclusion
Looking at the results there are some small noticeable differences when comparing the two. The after-implementation appears to be just slightly faster, and hopefully this is due to the fact that the alternate compare function is indeed faster than the inbuilt C function. But since the difference isn't greater, I think that there is a real chance of the results lying within the margin of error.













## 1. Parallelization
###### Author: Niels With Mikkelsen, s174290

### Overall Motivation

This overall experiment seeks to figure out how much (if any) we can benefit from parallelization, i.e. handling multiple client request simultaneously. When it comes to parallelization there are several diffrent ways to go about it, in the next 3 sub experiments we will try to find the best one. 

The reason we at all bother to do these experiments is because the computer which are running the test have a multi-core CPU with 4 CPU's.


### 1.1 Sequential Model vs Process Model
In this experiment we want to compare a implementation using multiple processes (concurrent) and a implementation using only one process (sequential). 

The sequential model is probably the simplest working implementation of the challenge. The server accepts one client request at a time, calculate the reversed hash and sends back the answer to the client. I believe that this implementation will be the slowest one, because it is the minimum viable product (MVP) for accomplishing 100% reliability and has no optimizations. It is mainly used for see the effect of the process model. 


The process model is a little more interesting. Every time the server receives a client request a new process is created using the **fork** system call. The **fork** system call copies its address space to the new child process, this way their are no shared memory (/critical sections) and thus no need for synchronisation. I believe that this implementation will be faster than the sequential model because we make use of the multi-core CPU i.e. we can have multiple processes working on diffrent requests.

#### 1.1.1 Setup
All test regarding this experiment has been executed on the same computer, we have tried to keep the workload constant during the test session, however this is nearly impossible and is a possible error. The configuration parameters of the client is the following: 

##### Run Configuration

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
| PRIO_LAMBDA       | 1.50          |


#### 1.1.2 Results
Below are the results of the tests.

| Run          | Sequential       | Concurrent       |
| -------------|:----------------:|:----------------:|
| First run    | 160.738.991	  | 93.545.260       |
| Second run   | 160.278.732	  | 93.458.995       |
| Third run    | 160.193.208	  | 92.979.926       |
| **Average**  | **160.403.644**  | **93.328.060**   |


#### 1.1.3 Discussion and Conclusion
Looking at the results there is a noticeable difference when comparing the averages, the process model is clearly faster that the sequential model. However, the process model implementation still has 2 major weakness when it comes to speed. 1) It has to copy the entire address space every time it creates a child process. 2) It starts more processes than we have CPU’s, this means that the scheduler has to do context switches quite often. Let’s first try to solve the first weakness by using threads.


### 1.2 Process Model vs Thread Model
In this experiment we want to compare a implementation using multiple processes and a implementation using multiple threads. 

The process model is described in the "Sequential Model vs Process Model" experiment

...



### 1.3 Optimizing Thread Model
![ThreadsPicture](Threads.png)

...












