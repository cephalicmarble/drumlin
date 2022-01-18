# days of yore

Drumlin and Pleg were all that remained when I had separated concerns in a brief, all-encompassing edit prior to supervised deletion of the code I was NOT allowed to possess. The refactoring has hung around waiting for a serious attempt since 2017, being made of code which I had been permitted to keep.

# drumlin

I have a plan to introduce a slot&signal mechanism for reading out metadata whilst several threads do something or other.

## note (namespace drumlin::work)

The plan would be to allow event-bubbling from per-thread meta-trap configs
during work definition and later stages of work lifecycles.
NB: thread may see event prior to trap

This is intended to allow push-notifications and to simplify debug from SPA during development of working units.

Possible HTTP(S) communications...
* GET status:|config:|report:|work:|descriptor:
* HEAD status:|config:|report:|work:|descriptor:
* POST workTo:  (appends or restarts 'thread' NB:ACL)
* PATCH config: (does not restart,            NB:ACL)
* DELETE config:|work:

# Approach to testing

Obvs I would like 100% test coverage, and the refactoring began with none.

As units are moved from pleg (essentially a repository of pre-refactoring code), the code is incorporated in a feature branch until it builds, prior to development in test branches which should achieve the desired coverage.

Currently this is underway with the first inclusion, which is the buffer allocator and work storage/access system, which I am developing with TDD.

Once the work system has its grounding, I will write the unitary tests for drumlin. I freely admit that the work is just too interesting and the refactoring too young to approach in any other manner. There is a long way before the imagined stride mechanism is load-balancing against a stream of HTTP(S):POSTed work items, but there is always some place to begin.

## pleg

socket        - ThreadType::SocketTest (client and server)

## drumlin

thread       
thread_worker - ThreadType::WorkerMock
terminator    - ThreadType::Terminator
application
drumlin::work

### unitary
mutexcall
regex
string_list
byte_array
jsonconfig
exception
cursor

#### Discussion

Since more complete assessment was done, I added much code from pleg into drumlin. Now attempting to use TDD for later aspects of development, I still want unit tests prior to use of the unit. String_list has turned up in a few places prior to that. drumlin/CMakeLists.txt allows unit tests to be developed in parallel, work is being done in 'drumlin/test.cpp' branch.

The note above (under drumlin), is intended to apply to later stage development, covering data-handling and metadata-response. Work is being done in 'drumlin/threads' branch.

Projected SPA development will take place in branches of pleg, to cover at least the units and verbs shown above.
