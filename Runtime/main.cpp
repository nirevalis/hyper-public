/*
 * Created by Piotr Chudziński in 2025. All rights reserved
 */

#include <Core/Common.hpp>
#include <Engine/GameEngine.hpp>

#ifdef _WIN32
#include <Windows.h>
#include <string>
#include <Game/Game.hpp>

long __stdcall SehExceptionHandler(EXCEPTION_POINTERS *ep)
{
    std::string errorMsg("Unhandled exception: ");
    switch (ep->ExceptionRecord->ExceptionCode)
    {
#define CASE(x)                                                                                                        \
case x:                                                                                                            \
errorMsg += #x;                                                                                                \
break;
        CASE(EXCEPTION_ARRAY_BOUNDS_EXCEEDED)
        CASE(EXCEPTION_DATATYPE_MISALIGNMENT)
        CASE(EXCEPTION_FLT_DENORMAL_OPERAND)
        CASE(EXCEPTION_FLT_DIVIDE_BY_ZERO)
        CASE(EXCEPTION_FLT_INVALID_OPERATION)
        CASE(EXCEPTION_ILLEGAL_INSTRUCTION)
        CASE(EXCEPTION_INT_DIVIDE_BY_ZERO)
        CASE(EXCEPTION_PRIV_INSTRUCTION)
        CASE(EXCEPTION_STACK_OVERFLOW)
#undef CASE
        case EXCEPTION_ACCESS_VIOLATION:
        errorMsg += "EXCEPTION_ACCESS_VIOLATION ";
        if (ep->ExceptionRecord->ExceptionInformation[0] == 0)
        {
            errorMsg += "reading address ";
        } else if (ep->ExceptionRecord->ExceptionInformation[0] == 1)
        {
            errorMsg += "writing address ";
        }
        errorMsg += FORMAT("{:#x}", (uint32) ep->ExceptionRecord->ExceptionInformation[1]);
        break;
        default:
        errorMsg += FORMAT("{:#x}", (uint32) ep->ExceptionRecord->ExceptionCode);
    }

    MessageBoxA(GetConsoleWindow(), errorMsg.c_str(), "[Hyper] Unhandled Exception", MB_OK);
    return EXCEPTION_CONTINUE_SEARCH;
}

int32 RunWithSehHandler(Hyper::GameEngine& engine)
{
    __try
    {
        engine.Start();
        return 0;
    } __except (SehExceptionHandler(GetExceptionInformation()))
    {
        return -1;
    }
}

int32 main(int32 argc, const char** argv)
{
    Hyper::GameEngine engine;

    const Hyper::Game g;
    engine.GetLayerStack().PushLayer(g.GetGameLayer());

    return RunWithSehHandler(engine);
}
#endif