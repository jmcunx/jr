## jr -- Rearrange Data Columns

**Note:** Starting October 12 2023, GitHub is forcing me to use
my Cell Phone for 2FA to allow access to my repositories.  I do
not want Microsoft to have access to my phone.  So updates to
this project will no longer occur on GitHub.  I will make a note
of where source can be retrieved from on my Gemini Capsule once
everything is moved off of GitHub.
Capsule: gemini://gem.sdf.org/jmccue/ or mirror gemini://sdf.org/jmccue/

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
