#include <iostream>

#include <client/example.h>
#include <shared/example.h>

int main(int argc, char* argv[])
{
    shared::hello_from_shared();
    shared::core_types::Type1 a_type;
    return 0;
}