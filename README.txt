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

This package contains a development snapshot for PuTTY SSH client for
Symbian OS version 1.6.

PuTTY 1.6 only officially supports S60 3rd and 5th edition phones. The
Series 80 version may still work but is not supported -- Series 80
Communicator users are recommended to continue using version 1.5. S60
3rd and 5th edition builds are distributed separately, make sure you
use the correct version for your phone model:

putty_s60v3_*   S60 third edition, supporting all current S60
                smartphones without a touchscreen. 
                Includes Nokia E61, E71, N80, N95 etc
putty_s60v5_*   S60 fifth edition, supporting all current S60 
                smartphones with a touchscreen.
                Includes Nokie 5800, N97 etc

A separate UIQ v3.x port is available at
        http://coredump.fi/putty

PuTTY is free software, and available with full source code under a
very liberal license agreement.

See the user's guide for further documentation.


Installing PuTTY
----------------

PuTTY S60 installation packages are self-signed. Many S60 devices,
including all Nokia E-series phones, refuse to install self-signed
applications by default.

To enable this, go to the device main menu, select Tools and start
Application Manager. From the application manager press Options,
select Settings, and set Software installation to All. The names will
be different in devices using a different language but the same
setting should be present. This is a mandatory operation, otherwise
PuTTY will not install, and the installer will complain about a
certificate error!

Unfortunately there is no way to add new trusted application signing
keys to an S60 device, so you will need to use PGP signatures to
verify their authentity. All installation packages have a separate PGP
signature. The key is available at
http://www.s2.org/~pekangas/petteri_pgp_2008.asc and on OpenPGP key
servers, ID E393AD7C.

S60 releases are distributed in files named
putty_s60v?_version.zip. Download the version you want, verify its PGP
signature, unzip the file, and transfer the .SISX package from the
archive to your phone. You can then install the package by opening it
from the e.g. the Messages Inbox or using the file manager.


Changes
-------

Main changes since 1.5:
- Included Risto Avila's S60 5th edition touch UI in the releases.

See Changelog in the source distribution for more details.


Future plans
------------

The main new feature in PuTTY 1.6 is S60 5th edition touch screen
support, which is fairly complete. Some further enhancements and new
features are likely before the final release.


Contact information
-------------------

The project home page is at
        http://s2putty.sourceforge.net/

The site also contains the source code, news, and other useful
information.

Feedback, bug reports, comments, and questions can be e-mailed to
        pekangas@s2.org
        risto.avila@utanet.fi (S60 5th ed touch UI)

We can't promise to answer all e-mail, but will definitely read it all.

Mailing lists, downloads, and other information can be found at
        http://www.sourceforge.net/projects/s2putty/


Acknowledgements
----------------

This Symbian OS port is partially based on the SyOSsh by Gabor
Keresztfavli. Especially the network code, noise generation, and
storage implementation borrow heavily from SyOSsh.

The original S60 port was written by Sergei Khloupnov.

S60 3rd ed icon by James Nash <http://cirrus.twiddles.com/>

S60 5th edition touch UI by Risto Avila.

Obviously this program wouldn't exist without the original PuTTY SSH
client by Simon Tatham and others. Many thanks for writing such an
excellent application and releasing it as free software!
