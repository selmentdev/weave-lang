#include "weave/threading/Runnable.hxx"
#include "weave/BugCheck.hxx"

namespace weave::threading
{
    bool Runnable::Start()
    {
        return true;
    }

    void Runnable::Execute()
    {
        WEAVE_BUGCHECK("Runnable::Execute() not implemented");
    }

    void Runnable::Finish()
    {
    }

    void Runnable::Run()
    {
        if (this->Start())
        {
            this->Execute();
            this->Finish();
        }
    }
}
