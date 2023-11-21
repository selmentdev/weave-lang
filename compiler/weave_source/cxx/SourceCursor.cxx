#include "weave/source/SourceCursor.hxx"
#include "weave/Unicode.hxx"

namespace weave::source
{
    void SourceCursor::Advance()
    {
        this->_current = this->_next;

        if (unicode::Decode(this->_value, this->_next, this->_last) != unicode::ConversionResult::Success)
        {
            this->_value = Invalid;
        }
    }
}
