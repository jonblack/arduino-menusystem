/*
 * An example of a custom NumericMenuItem.
 * It tries to display some ASCII graphics in edit mode.
 * This can be useful if you want to give the end user an overview of the value limits.
 * 
 * Copyright (c) 2016 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#ifndef _CUSTOMNUMERICMENUITEM_H
#define _CUSTOMNUMERICMENUITEM_H

#include <MenuSystem.h>

class CustomNumericMenuItem : public NumericMenuItem
{

public:
    /**
     * @param width the width of the edit mode 'ASCII graphics', must be > 1
     * @param name The name of the menu item.
     * @param value Default value.
     * @param minValue The minimum value.
     * @param maxValue The maximum value.
     * @param increment How much the value should be incremented by.
     * @param valueFormatter The custom formatter. If NULL the String float
     *                       formatter will be used.
     */
    CustomNumericMenuItem(uint8_t width, const char* name, float value, float minValue,
                          float maxValue, float increment=1.0,
                          ValueFormatter_t value_formatter=NULL)
    : _width(width),
      NumericMenuItem(name, value, minValue, maxValue, increment, value_formatter)
    {
    }

    virtual String& get_composite_name(String& buffer) const 
    {
        if (is_editing_value()) 
        {
            // Only display the ASCII graphics in edit mode.

            // make room for a ' ' at the end and the terminating 0
            char graphics[_width+2];
            // fill the string with '-'
            for (int i=0; i<_width; i++) 
                graphics[i] = '-';
            // insert a '|' at the relative _value position
            graphics[ int( (_width-1)*(_value-_minValue)/(_maxValue-_minValue))] = '|';
            graphics[_width] = ' ';
            graphics[_width+1] = 0;   
            buffer = graphics;

            if (_value_formatter != NULL)
                buffer += _value_formatter(_value);
            else
                buffer += _value;
            return buffer;
        } else 
        {
            // Non edit mode: Let parent class handle this
            return NumericMenuItem::get_composite_name(buffer);
        }
    }
    
    const uint8_t _width;
};

#endif // _CUSTOMNUMERICMENUITEM_H
