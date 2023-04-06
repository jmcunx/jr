## jr -- Rearrange Data Columns

jr will rearrange Columns in either a Delimited Text File
or a Flat non-delimited Text File.

This was created for Systems that does not have awk(1) or col(1)/paste(1).
If awk(1) or col(1)/paste(1) is present you may want to use those instead.

It can also:
* Adjust Field Formatting, for example convert Date Strings.
* Convert to/from Flat Fixed Length Files from/to Delimited Text Files.

[j\_lib2](https://github.com/jmcunx/j_lib2) is an **optional** dependency.

[GNU automake](https://en.wikipedia.org/wiki/Automake)
only confuses me, but this seems to be good enough for me.

**To compile:**
* If "DESTDIR" is not set, will install under /usr/local
* Execute ./build.sh to create a Makefile
* Works on Linux, BSD and AIX

_To uninstall_, execute
"make uninstall"
from the source directory
