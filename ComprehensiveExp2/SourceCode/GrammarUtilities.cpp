#include "GrammarUtilities.h"

bool GrammarUtil::IsSpace(char c)
{
    return (c==' ' || c=='\n' || c=='\r' || c=='\t');
}
