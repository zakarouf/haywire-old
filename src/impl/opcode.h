#ifndef ZAKAROUF_HAYWIRE_OPCODE_H
#define ZAKAROUF_HAYWIRE_OPCODE_H

#include "../core/base.h"

#define hw_OpCode(c) zpp__CAT(hw_OpCode_, c)
enum hw_OpCode {
      hw_OpCode(RET)            = 0x00

    , hw_OpCode(CTOR_COPY_DATA)     // R(Ax) = K(BCx)
    , hw_OpCode(CTOR_COPY_FILE)     // R(Ax) = K(BCx)
    , hw_OpCode(CTOR_COPY_VAR)      // R(Ax) = R(BCx)
    , hw_OpCode(DTOR)               // R(Ax).dtor(R(Ax)) -> R(Ax) = nil

    , hw_OpCode(ACCESS)             // R(Ax) = R(BCx).access(R(BCx))
    , hw_OpCode(ACCESS_SOFT)        // R(Ax) = R(BCx).access_soft(R(BCx))

    //  R(Ax).TYPEFN_X(R(Ax), R(Bx), R(Cx))
    //                          Number    List        HashSet     String
    , hw_OpCode(TYPEFN_0)   //  Add,      Push,       Set   
    , hw_OpCode(TYPEFN_1)   //  Sub,      Pop,        Filter,     Remove Var
    , hw_OpCode(TYPEFN_2)   //  Mul,      Combine,    Combine,    Combine
    , hw_OpCode(TYPEFN_3)   //  Div,      Filter,     
    , hw_OpCode(TYPEFN_4)   //  Modulo

    , hw_OpCode(CALL)       // K(iAx) > 0?
                            //  True  -> selfmod(Ax)        ( D(Bx) .. D(Bx + 1) .. D(Cx - 1) )
                            //  False -> findfunc(D(Ax))    ( D(Bx) .. D(Bx + 1) .. D(Cx - 1) )
                            
    , hw_OpCode(CALL_C)     // 

    , hw_OpCode(EQ)         // R(Bx).eq(R(Cx)) == True? pc++;
    , hw_OpCode(LT)         // R(Bx).lt(R(Cx)) == True? pc++;
    , hw_OpCode(LE)         // R(Bx).le(R(Cx)) == True? pc++;

    , hw_OpCode(CMP)        // R(Ax) = R(Bx).cmp(R(Cx));

    , hw_OpCode(JMP)        // pc += iBCx

    , hw_OpCode(PRINT)      // call printfn

    , hw_OpCode(THREAD_NEW) // R(Ax) = thread_new(R(Bx));
    , hw_OpCode(Total)
};

#endif

