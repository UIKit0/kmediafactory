# Template File format #
Templates that come with KMediaFactory should provide good examples how to write theme files. KMediaFactory template file is a zip archive with a following structure:

  * mimetype - File that has text "application/x-kmediafactory-template"
  * icon.png - Small (64x64) preview of the template.
  * preview.jpg - Large (720x576) preview of the template.
  * template.xml - Page layouts
  * locale - Directory for template translations

# Page layouts #

Page layout has the following structure:
```
<template>
  <page>
    widgets...
  </page>
  other pages...
</template>
```

## template ##
Parameters:

  * first\_page - Name of first page
  * id - name/id used internally in KMediaFactory
  * name - name that is shown to user

## page ##
Definition of each page.
Parameters:

  * name - Page name
  * titles - Number of titles (Videos from media page) this page can contain. 0 if showing only chapters
  * chapters - Number of chapters this page can contain. 0 if showing only titles.

## Widgets ##

### All widgets ###
Parameters common to all widgets:

  * name - Name of the widget
  * margin - Margin between outer frame and things inside widget.
  * x - Widget left (see coordinates)
  * y - Widget top (see coordinates)
  * w - Widget width (see coordinates)
  * h - Widget height (see coordinates)
  * color - Widget color #RRGGBBAA (Red, Green, Blue, Alpha 00 = Transparent)
  * halign - left|center|right
  * valign - top|middle|bottom
  * layer - widget layer (sub|select|highlight)
  * shadow - shadow for the widget
    * radius - Blur radius
    * offset.x - Shadow x offset
    * offset.y - Shadow y offset
    * type - None = 0, Hard = 1, Blur = 2
    * color - Color of the shadow
    * sigma - Standard deviation

#### coordinates ####
Coordinates can be four different types:

  * Minimum `[`min`]` - Tries to find minimum size. example: w="min"
  * Relative `[`+/-0`]` - Relative to parent widget. example: x="+10"
  * Absolute `[`0`]` - Absolute coordinate. example: w="150"
  * Percentage `[`0%`]` - Relative to parent widget. example: w="50%"

Coordinates can also hold custom margin [0:margin]. example: x="+0:15"

### image ###
Bitmap image widget.
Parameters:

  * url - url of the image (See url)
  * scale - 1 = scale image to size
  * proportional - 1 = scale proportionally

#### url ####
Additional to all kde kioslave protocols there are couple of more.

  * template:///path/image - get image from template. example: template:///images/background.jpg
  * kde://resource/image - find image from kde dir. example: kde://wallpaper/blue-bend.jpg
  * project:///[$]title.[$]chapter - get image from project. $ locks the number (It don't grow on multiple pages) example: project:///1.1

### frame ###
Frame/box widget.
Parameters:
  * line\_width - Frame line width (Default: 0)
  * fill\_color - Color which frame is filled (Default: transparent)
  * rounded - Frame corner radius (Default: 0)

### label ###
Text label widget.
Parameters:
  * font - Label font
    * size - Font size
    * weight - Font weight
    * name - Font family
  * text - Label text
    * %[$]title.[$]chapter% - Can contain markers to include chapter texts. title = 0 -> main title.

### button ###
Button (invisible widget that groups other widgets as button)
Parameters:
  * jump - `[`page name`]`:title.chapter
  * up - name of the button to go when up is pressed.
  * left - name of the button to go when left is pressed.
  * right - name of the button to go when right is pressed.
  * down - name of the button to go when down is pressed.


### group ###
Widget to group other widgets.

### vbox ###
Widget to group other widgets vertically.

### hbox ###
Widget to group other widgets horizontally.

### spacer ###
Widget that takes space in vbox or hbox.