        PuTTY for Symbian OS
        --------------------

Version 1.5.1, 19 July 2009

Copyright 2002-2009 Petteri Kangaslampi
Portions copyright Sergei Khloupnov, James Nash, Damion Yates, and
Gabor Keresztfavli.
Based on PuTTY 0.60, Copyright 1997-2007 Simon Tatham.
See license.txt for full copyright and license information.


Introduction
------------

This package is a maintenance release for PuTTY SSH client for Symbian
OS version 1.5. It contains a number of bug fixes over 1.5.0 and minor
new features, see "Changes" below for details.

PuTTY is distributed in two different packages, one for S60 third
edition, and one for Series 80 phones. Make sure you use the correct
version for your phone model. The packages are:

putty_s60v3_*   S60 third edition, supporting all current S60
                smartphones. Includes Nokia E61, N80, N95 etc
putty_s80v2_*   Series 80 v2.0. Nokia 9300, 9300i, 9500

A separate UIQ v3.x port is available at
        http://coredump.fi/putty

PuTTY 1.5 only officially supports S60 3rd edition and Series 80 v2
phones; users with S60 5th edition touchscreen phones such as the
Nokia 5800 XPressMusic or the Nokia N97 should try Risto Avila's Touch
UI available at
        http://bd.kicks-ass.net/koodaus/putty/
Risto's touch UI will be included in the next major PuTTY release, so
any feedback is more than welcome!

PuTTY is free software, and available with full source code under a
very liberal license agreement.

See the user's guide for further documentation.


Installing PuTTY on Series 80
-----------------------------

PuTTY installation packages for Series 80 are signed with a
self-signed certificate. To be able to verify the packages, you'll
need to install the certificate to the device. The steps needed are:

1. Fetch the certificate from
   http://www.s2.org/~pekangas/petteri_s80_2009_der.zip and unzip it.

2. Verify the certificate. Its MD5 sum is
   9559ec393f3fecb0c34ababfc0f9727f. A PGP signature is available at
   http://www.s2.org/~pekangas/petteri_s80_2009_der.cer.asc, the key is
   http://www.s2.org/~pekangas/petteri_pgp_2009.asc. The key is also
   available on OpenPGP key servers, ID E393AD7C.

3. Copy the certificate to a file in the communicator.

4. Open Control panel, select the "Security" group, and from there
   open "Certificate manager".

5. Change to the "Other" tab and find the new certificate, named
   "Petteri Kangaslampi" from the list.

6. Select "View details", select "Trust settings" and enable
   "Application installation"

After installing the certificate, install the .SIS package normally.


Installing PuTTY on S60
-----------------------

PuTTY S60 3rd edition installation packages are self-signed. Many S60
devices, including all Nokia E-series phones, refuse to install
self-signed applications by default.

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
putty_s60v3_version.zip. Download the version you want, verify its PGP
signature, unzip the file, and transfer the .SISX package from the
archive to your phone. You can then install the package by opening it
from the e.g. the Messages Inbox or using the file manager.


Changes
-------

Main changes since 1.5.0:
- Added profile file import/export to make it easier to manually
  configure advanced features
- Incorporated a port forwarding fix from Shai Ayal. This should fix
  long-standing bug #918200 and forwarding local ports to the remote
  server seems to work. Note that port forwarding is still not
  available in the UI and is not officially supported.
- Patches from Damion Yates:
    - Backspace setting (Ctrl-H vs Delete)
    - Force a 80x24 terminal on an E90 when using a 10x14 font
    - Rudimentary LockSize support (not in the UI)
  Thanks Damion!
- Potential fix for "Bad Name" errors seen on S60 in some situations.
  This should fix bug #1878884.
- Increased the maximum password length from 32 to 64 characters on S60.

See Changelog in the source distribution for more details.


Future plans
------------

As a rule, PuTTY 1.5 is now considered stable, and should only receive
bug fixes. Most new development should go into a new 1.6 branch which
will open in the future, but 1.5 may receive minor enhancements such
as new settings in addition to bug fixes.


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

The original S60 port was written by Sergei Khloupnov.

S60 3rd ed icon by James Nash <http://cirrus.twiddles.com/>

Obviously this program wouldn't exist without the original PuTTY SSH
client by Simon Tatham and others. Many thanks for writing such an
excellent application and releasing it as free software!
