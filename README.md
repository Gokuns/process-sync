# traffic-simulation
Operating Systems class project
In our implementation, all the specified requirements are met. We have 6 threads running in total and below are the explainations for each:

main thread: responsible for the creation of all others and initializations of pointers/global variables/attributes etc.

thread_PO: the thread representing the police officer. During the simulation time, it coordinates the cars through the road and keeps the logs for its actions and the cars. It is also responsible for checking if a car have been waiting for more than 20 seconds.

road threads (thread_N, thread_E, thread_S, thread_W): there is 4 of them and they are responsible for creating and adding a car every 1 second with probability p (or for the case of north, 1-p).

We have one mutex lock to prevent any race conditions regarding the 4 car queues. We also have a condition variable for the honk-cell phone situation.
