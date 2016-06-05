#include "CustomNumericMenuItem.h"

CustomNumericMenuItem::CustomNumericMenuItem(
        uint8_t width, const char* name, float value, float minValue,
        float maxValue, float increment, ValueFormatter_t value_formatter)
: NumericMenuItem(name, NULL, value, minValue, maxValue, increment,
                  value_formatter),
  _width(width)
{
}

uint8_t CustomNumericMenuItem::get_width() const
{
    return _width;
}

void CustomNumericMenuItem::render(MenuComponentRenderer const& renderer) const
{
    // TODO: dynamic_cast not allowed due to compile option -fno-rtti;
    // static_cast does zero checks! :s
    MyRenderer const& my_renderer = static_cast<MyRenderer const&>(renderer);
    my_renderer.render_custom_numeric_menu_item(*this);
}
