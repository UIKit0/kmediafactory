# Import DV in 0.6.x #

  * Get the import dvdauthor xml plugin from [kde-files.org](http://www.kde-files.org/index.php?xcontentmode=667).
  * Grab dv:
```
   $ dvgrab --format raw --opendml --size 0 --autosplit --timestamp video-
```
  * Edit in kino:
```
   $ kino video-*.dv
```
  * Export to mpeg/8-dvd.
  * Import dvdauthor xml to kmediafactory.