/*
    File: ExprMem.c
    Auth: Brian Allen Vanderburg II
    Date: Wednesday, April 30, 2003
    Desc: Memory functions for ExprEval

    This file is part of ExprEval.
*/

/* Includes */
#include "exprincl.h"

/* Allocate memory */
void* exprAllocMem(size_t size)
    {
    return malloc(size);
    }

/* Free memory */
void exprFreeMem(void *data)
    {
    if(data)
        free(data);
    }