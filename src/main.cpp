// main.cpp : Defines the entry point for the application.
//

#include "main.h"

int main(int argc, char* argv[]) {
    //std::cout << "Hello World\n";

    Log::StartLine();
    Log::Write("Hello World!");
    Log::EndLine();

    Log::Save();

    std::cout << "Press enter to exit...\n";
    std::cin.ignore();
    return 0;
}