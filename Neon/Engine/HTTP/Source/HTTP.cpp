#include "pch.h"
#include "../Header/HTTP.h"

void HTTP::DispatchRequest(long long a1, long long* a2, int a3)
{
    if (Fortnite_Version < 8.01) *(int*)(__int64(a2) + (Fortnite_Version < 4.2 ? 0x60 : 0x28)) = 3;
    return DispatchRequestOriginal(a1, a2, 3);
}
