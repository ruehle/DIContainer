#include <stdio.h>

#include "gmock/gmock.h"

#pragma warning ( disable : 4245 )

GTEST_API_ int main(int argc, char **argv) {
    printf("Running DIContainer unit tests\n");

#ifdef _WIN32
    unsigned int oldflags;
    _controlfp_s(&oldflags, ~(_SW_OVERFLOW | _SW_ZERODIVIDE | _SW_INVALID), _MCW_EM);
#endif

    ::testing::InitGoogleMock( &argc, argv );
    return RUN_ALL_TESTS();
}
