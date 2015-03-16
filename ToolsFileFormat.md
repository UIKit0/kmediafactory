# Tools File Format #
KMediaFactory tools file that can be downloaded using GHNS (kde-files.org)
is a targz or tar.bz2 archive with a following structure:

  * `[`name`]`.desktop - desktop file
  * `[`name`]`.png - Icon for tool
  * `[`name`]`.sh - Can be any runnable script (py, pl, etc.)

# Desktop file #
Normal KDE desktop file. It should contain at least these entries.

  * Name=`[`Name`]`
  * Exec=`[`name`]`.sh
  * Icon=`[`name`]`.png

Icon entry can be also existing kde icon (It should not have .png then).

This special entry can be used to specify if tool is placed to media button bar and menu or in tools menu.

  * `X-KMF-MediaMenu=true`

you can use [this](http://www.kde-files.org/content/show.php?content=38489) as an example.