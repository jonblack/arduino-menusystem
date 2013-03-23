menusystem
==========

An arduino library for implementing a menu system.

Documentation
-------------

Other than the examples included in the library, the following pages might be
useful to you:

* [www.jonblack.org](http://www.jonblack.org/)
* [Arduino forum post](http://arduino.cc/forum/index.php/topic,105866.0.html)

Changelog
---------

The library isn't versioned in a coherent way; there are no version numbers you
can refer to. There are, however, a bunch of dates when I made some changes:

**23/03/2013**

* Merged in another minor improvement provided by thomasjfox:
 * Add the ability to loop through a menu in prev() and next()

**18/03/2013**

* Merged in minor improvements provided by thomasjfox
 * get_menu_component(): Use 'byte' instead of 16bit integer for index
 * Add return value to MenuSystem.back()
 * Prevent memory corruption by inserting too many items
 * Whitespace cleanup


**25/05/2012**

* Added support for displaying all current menu items (thanks to arcachofo)
* Added this readme

Note: The changes made for this release break backwards compatability. The
issues are relatively easy to fix.
