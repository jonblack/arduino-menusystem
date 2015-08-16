menusystem
==========

An arduino library for implementing a menu system.

Documentation
-------------

Other than the examples included in the library, the following pages might be
useful to you:

* [Arduino forum post](http://arduino.cc/forum/index.php/topic,105866.0.html)
* [Humble Coder](http://www.humblecoder.com/)

Contribution
------------

If you'd like to contribute to `arduino-menusystem`, please submit a
pull-request.

Changelog
---------

**2.0.1 - 16/08/2015**

* Add Sure Electronics LED Matrix example

**2.0.0 - 14/08/2015**

* Add LCD Nav Keypad example from (thanks to @SauleVire)
* Remove AUTHORS file: too much hassle to maintain
* Add library.properties

**1.0.1 - 20/13/2013**

* Renamed project to `arduino-menusystem`;
* Fix buffer overflow when adding a menu;
* Menu components are now dynamically allocated (previously it was fixed at
  five items).

**1.0.0 - 15/12/2013**

* Selected menu item can be remembered (thanks to @KipK)
* Examples for serial and LCDs (thanks to @niesteszeck)
* Add AUTHORS file

**0.3.0 - 23/03/2013**

* Merged in another minor improvement provided by @thomasjfox:
 * Add the ability to loop through a menu in `prev()` and `next()`

**0.2.0 - 18/03/2013**

* Merged in minor improvements provided by thomasjfox
 * `get_menu_component()`: Use 'byte' instead of 16bit integer for index
 * Add return value to `MenuSystem.back()`
 * Prevent memory corruption by inserting too many items
 * Whitespace cleanup

**0.1.0 - 25/05/2012**

* Added support for displaying all current menu items (thanks to arcachofo)
* Added this readme

Note: The changes made for this release break backwards compatability. The
issues are relatively easy to fix.
