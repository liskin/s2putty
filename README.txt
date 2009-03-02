        PuTTY for Symbian OS
        --------------------

Version 1.5.0, 25 January 2009

Copyright 2002-2009 Petteri Kangaslampi
Portions copyright Sergei Khloupnov, James Nash, Damion Yates, and
Gabor Keresztfavli.
Based on PuTTY 0.60, Copyright 1997-2007 Simon Tatham.
See license.txt for full copyright and license information.


Introduction
------------

This package is the full final release for PuTTY SSH client for
Symbian OS version 1.5.0. This is the first non-beta non-snapshot
release since 1.3.2 was released in January 2005, and is a significant
milestone in the development.

The only change compared to 1.5RC1 is a small palette resource change,
but the code itself has not been modified.

Compared to earlier releases, 1.5 brings significant enhancements:
PuTTY for Symbian OS is based on an up-to-date release of the PuTTY
core from the Windows version, it has a new friendlier settings system
based on profiles, more settings including character set and color
palette selection, and fixes and enhancements on the S60 platform.

This release support all S60 third edition phones and the Series 80 v2
Communicator products. Both versions have essentially identical
features and will be fully supported through bug fixes. However, given
that the user base for S60 is now orders of magnitude larger, new
features may only be availble on S60 in the future. Earlier S60
versions are not supported, and while this version contains some
changes to enable very basic functionality on S60 5th edition, it is
still considered unsupported too.

If you are upgrading from releases earlier than 1.5 beta 2, note that
PuTTY can now use full 256-bit keys with the AES encryption algorithm,
which can slow it down considerably. New profiles will use a faster
128-bit Blowfish algorithm, but existing profiles will need to ben
changed from the SSH settings page to take advantage of this.

PuTTY is distributed in two different packages, one for S60 third
edition, and one for Series 80 phones. Make sure you use the correct
version for your phone model. The packages are:

putty_s60v3_*   S60 third edition, supporting all current S60
                smartphones. Includes Nokia E61, N80, N95 etc
putty_s80v2_*   Series 80 v2.0. Nokia 9300, 9300i, 9500

A separate UIQ v3.x port is available at
        http://coredump.fi/putty

PuTTY is free software, and available with full source code under a
very liberal license agreement.

The user's guide contains further documentation, and has been recently
updated.


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

Main changes since 1.5 beta 2:
- Some fixes for S60 5th edition devices, the terminal is now visible
  after one orientation change.
- Added support for user-selectable color palettes

Main changes since 1.5 beta 1:
- Updated to PuTTY 0.60 core
- Default cipher is now 128-bit Blowfish, and the cipher can be
  changed from the SSH settings page.
- Fixed the send grid to work properly on S60 3.1, and to handle the
  E71 keyboard
- S60 settings view can now clear the private key, fixing bug 1930543.
- Copy/paste support for S60, thanks to a patch from Thomas Grenman
- A proper icon for S60 3rd ed, from James Nash

Main changes since 1.4 beta 1:
- Support for different character encodings, most notably UTF-8
- S80 settings improvements: Settings are stored as profiles, and the
  application presents a profile list at startup.
- S60 third edition bug fixes and additional fonts. PuTTY now works
  properly on an E90
- Refactored and largely rewritten S60 UI, with a separate profile
  list view, proper settings views, no need to exit the application
  after each connection etc
- S60 settings brought up to date with S80
- Replaced the "Send" menu with a new grid control. This avoids problems
  with multiply nested menus that have appeared on recent S60 3.1
  devices, and should also improve usability on non-QWERTY devices
- Lots of bug fixes, UI improvements, and general cleanup

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
