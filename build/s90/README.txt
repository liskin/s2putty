        PuTTY for Series 90
        -------------------

Version 1.4 Development Snapshot

Copyright 2002-2005 Petteri Kangaslampi
Based on PuTTY 0.56 beta, Copyright 1997-2004 Simon Tatham.
See license.txt for full copyright and license information.


Introduction
------------

This package contains a development snapshot of PuTTY SSH
Client. Development snapshot releases are work in progress, and may
contain more defects or missing features than official releases. They
are mainly intended for developers who wish to track PuTTY for Symbian
OS development and for users who need features not in standard
releases.

The Series 90 (Nokia 7710) is very new, so expect more initial
problems than usual. See 

PuTTY is free software, and available with full source code under a
very liberal license agreement.

See the user's guide document in this package for more documentation.


Series 90 specific issues
-------------------------

- Tap on the terminal window to activate on-screen keyboard or
  handwriting recognition.

- The "A" device key (upper right) works as a Ctrl prefix. Press it
  once, and follow it by a letter on the on-screen keyboard or
  handwriting recognition window.

- The directional pad works as cursor keys, and the Esc device key
  (lower right) works as Esc.

- There is currently only one font

For the most part PuTTY for S90 should work similarly to the S80
version, so most of the documentation for the 9210 release should be
valid.


Changes
-------

See Changelog in the source distribution.


Future plans
------------

Changes planned for 1.4:
- Add compression and keepalive to options
- Rewrite S60 UI with custom text rendering routines and remove
  separate fixed font. Adding new fonts seems to confuse a lot of
  applications and the installation procedure isn't too reliable.
- Possibly copy/paste support for Communicators
- More fonts for S90
- Any good ideas? Contact me, preferably with patches


Contact information
-------------------

The project home page is at
        http://s2putty.sourceforge.net/

The site also contains the source code, news, and other useful
information.

Feedback, bug reports, comments, and questions can be e-mailed to
        pekangas@s2.org

I can't promise to answer all e-mail, but will definitely read it all.

Mailing lists, downloads, and other information can be found at
        http://www.sourceforge.net/projects/s2putty/


Acknowledgements
----------------

This Symbian OS port is partially based on the SyOSsh by Gabor
Keresztfavli. Especially the network code, noise generation, and
storage implementation borrow heavily from SyOSsh.

Obviously this program wouldn't exist without the original PuTTY SSH
client by Simon Tatham and others. Many thanks for writing such an
excellent application and releasing it as free software!
