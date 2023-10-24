#include "Compiler.Core/SourceCursor.hxx"
#include "Compiler.Core/Unicode.hxx"

namespace Weave
{
    void SourceCursor::Advance()
    {
        this->m_Current = this->m_Next;

        if (UTF8Decode(this->m_Value, this->m_Next, this->m_Last) != UnicodeConversionResult::Success)
        {
            this->m_Value = Invalid;
        }
    }
}
