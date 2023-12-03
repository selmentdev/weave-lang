#pragma once

namespace weave::threading
{
    class Runnable
    {
    public:
        Runnable() = default;
        virtual ~Runnable() = default;

        Runnable(Runnable const&) = default;
        Runnable(Runnable&&) = default;
        Runnable& operator=(Runnable const&) = default;
        Runnable& operator=(Runnable&&) = default;

    protected:
        virtual bool Start();
        virtual void Execute();
        virtual void Finish();

    public:
        void Run();
    };
}
