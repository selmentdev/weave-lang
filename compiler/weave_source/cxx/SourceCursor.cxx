#include "weave/source/SourceCursor.hxx"
#include "weave/Unicode.hxx"

namespace weave::source
{
    void SourceCursor::advance()
    {
        this->_current = this->_next;

        if (unicode::utf8_decode(this->_value, this->_next, this->_last) != unicode::UnicodeConversionResult::Success)
        {
            this->_value = Invalid;
        }
    }
}
