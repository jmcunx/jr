## jr -- Rearrange Data Columns

jr will rearrange Columns in either a Delimited Text File
or a Flat non-delimited Text File.

This was created for Systems that does not have awk(1) or col(1)/paste(1).
If awk(1) or col(1)/paste(1) is present you may want to use those instead.

It can also:
* Adjust Field Formatting, for example convert Date Strings.
* Convert to/from Flat Fixed Length Files from/to Delimited Text Files.

This requires [j\_lib2](https://github.com/jmcunx/j_lib2) to build.

[GNU automake](https://en.wikipedia.org/wiki/Automake)
only confuses me, but this seems to be good enough for me.

**To compile:**
* Set "DESTDIR" for where you want this to install.  Examples:
  * setenv DESTDIR /usr/local
  * export DESTDIR=/usr/local
* this assumes [j\_lib2](https://github.com/jmcunx/j_lib2)
  is also installed under "DESTDIR".
* Edit Makefile, uncomment areas associated to the OS
  you want to compile on.
  OS Sections:
  * Linux 64 bit (default)
  * Linux 32 bit
  * BSD 64 bit
  * BSD 32 bit
  * AIX

_To uninstall_, execute
"make uninstall"
from the source directory
