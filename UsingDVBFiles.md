# Using DVB TS Files #

This is a guide for using TS Mpeg files recorded from DVB with KMediaFactory.
I'm using [ProjectX](http://sourceforge.net/projects/projectx) to convert / edit commercials in TS file.
Get the import dvb plugin from [kde-files.org](http://www.kde-files.org/index.php?xcontentmode=667).

## ProjectX ##

  * Add file to ProjectX: File - Add...
  * Press 'Go!' in 'work' frame to demux mpeg.
  * Edit video: Edit - open VideoCut/Specials...
  * After editing, press 'Apply & close'
  * Press 'Go!' in 'work' frame to demux mpeg again (Now with cut points).
  * Plugin multiplexes (joins) files together with [mplex](http://mjpeg.sourceforge.net/") program.