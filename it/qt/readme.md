# Qt integration tests

## Remarks

Currently, no integration tests for Qt are implemented.
 - First, the Qt interfaces has LOTS of functions and testing them all would be a HUGE amount of work I can't afford right now.
 - Second, Testing the interface using QtTest may be difficult to integrate in current CI setup (CI is run on headless servers only for now)
 - Third, both Qt and the command line uses the same library as a backend (Core::xx functions). Yes, this is not enough to show the Qt interface is working well, but will be enough for now.

For those reasons, this directory is empty for now.
Of course, any help with the developement of CI for Qt interface would be appreciated.
