# drumlin
common cpp items

These developed as a meta-protocol against multi-threaded QT; I have a plan to introduce a slot&signal mechanism for reading out metadata whilst several threads do something or other.

There is obviously more to the development than this, which I have deposited here mainly as
a recruitment stooge, although I have plans to go further with the concept. (See cephalicmarble/pleg)

N.B. I was permitted to use any of the prototype that I developed with this so long
as it did not expose any more pertinent information from the specification.

# Tests

## thread-rig

socket        - ThreadType::SocketTest (client and server)
mutexcall     - ThreadType::MockTest
thread_worker - ThreadType::WorkerMock
terminator    - ThreadType::Terminator
thread       

### unitary

regex
string_list
byte_array

## app-rig (dependencies on thread-rig)

drumlin
cursor
application

### unitary

jsonconfig
exception

#### Discussion

At a first glance, I can develop app-rig and associated shell script (run-tests.sh)
against the present thread code. Once those and the unitary app-rig code is tested,
I will take a step back and work on auto_ptr in the string_list and byte_array.
(This is code that lost its way a while back, and it is exposed now the development
is being separated.) Then after regextest.cpp, I will be confident with the app-rig
and may develop the mock threads. Overall, test development will be from the base
of the list above, progressing in an upward direction.
