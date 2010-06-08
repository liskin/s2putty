        PuTTY for Symbian OS
        --------------------

Version 1.6 Development Snapshot

Copyright 2002-2010 Petteri Kangaslampi
Copyright 2009 Risto Avila
Portions copyright Sergei Khloupnov, James Nash, Damion Yates, and
Gabor Keresztfavli.
Based on PuTTY 0.60, Copyright 1997-2007 Simon Tatham.
See license.txt for full copyright and license information.


Introduction
------------

This package contains the source code for the PuTTY SSH client for
Symbian OS. Only S60 third and fifth edition support is actively
maintained, other variants may not build successfully.

This README file contains some minimal notes about the source code to
help in exploring it. More documentation should be available later.


Building PuTTY
--------------

PuTTY uses the standard Symbian build system, and can be built like
any other application. Since it supports many platforms in a single
source code tree, the directory structure is a bit non-standard though.

To build PuTTY, select the correct directory under "build" that
matches the SDK in use, and execute "bldmake" and "abld" as usual. The
directories are:

build/s60v1     S60 first edition (Nokia 7650, 3650, N-gage, ...)
build/s60v2     S60 second edition (Nokia 6600, 6630, N70, ...)
build/s60v3     S60 third edition (Nokia E61, N80, ...)
build/s60v5     S60 fifth edition (Noki 5800, N97, ...)
build/s80v1     Series 80 v1.0 (Nokia 9200 Communicator series)
build/s80v2     Series 80 v2.0 (Nokia 9300, 9300i, 9500)
build/s90       Series 90 (Nokia 7710)

For example, to build PuTTY for Series 60 second edition, unzip the
package to the SDK directory, and execute:

        cd s2putty\build\s60v2
        bldmake bldfiles
        abld build armi urel
        (makesis putty.pkg)

Building the PuTTY engine from Visual C++ doesn't seem to work at
least in some cases. Compiling from the command line appears to work
better. If you only modify the user interface, it should be enough to
build the software once from the command line, and use the IDE
afterwards.

On S60 third edition PuTTY compiles at least with Carbide.c++ and
GCCE. RVCT and CodeWarrior have not been recently tested.


Applications and executables
----------------------------

PuTTY is a traditional Symbian OS application separated into an
application user interface and an engine DLL.

putty.app        The user interface application (putty.exe on S60v3)
putty.rsc        Application resources
putty.aif        Application information file (putty_reg.rsc on S60v3)

puttyengine.dll  The PuTTY engine.


Directories
-----------

The source code is divided into the following directories:

build            Build scripts, see above for build instructions.

configrecog      Recognizer for configuration files (recputty.mdl)

engine           PuTTY engine implementation

 engine/putty    Original PuTTY source tree, with minimal
                 modifications required to support the Symbian
                 platform. Isolated into a separate directory to help
                 merging with new PuTTY releases

 engine/puttysymbian  Symbian OS implementations of various PuTTY
                 components, such as networking code, noise
                 generation, and character set support.

include          Include files defining interfaces between the major 
                 components

test             Various test programs, may no longer work

ui               User interface implementation for different platforms
 ui/common       UI implementation files shared between platforms
 ui/s2font       A custom bitmap font rendering class and associated fonts.
                 User on S60 and S90.
 ui/s60          UI implementation for S60
 ui/s60v1        UI files specific to S60 first edition
 ui/s60v2        UI files specific to S60 second edition
 ui/s60v3        UI files specific to S60 third edition
 ui/s60v5        UI files specific to S60 fifth edition
 ui/s80          UI implementation for Series 80 communicators
 ui/s80v1        UI files specific to S80 v1.0 (Nokia 9210)
 ui/s80v2        UI files specific to S80 v2.0 (Nokia 9500 etc)
 ui/s90          UI implemenattion for Series 90 (Nokia 7710)
