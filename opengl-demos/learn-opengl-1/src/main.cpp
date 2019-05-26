/*

main notes for this application:
- will be building for x86 only (DEBUG and RELEASE), can leave x64 build configs untouched
- will be targeting the /MT & /MTd runtimes, and thus preferring static-linking
- middleware folder will contain all middleware (then each project can choose which versions to use)
- will be targetting OpenGL 3.3 Core Context (can increase version for more advanced demos)

- MY CONVENTIONS:
- m_MemberVar
- s_StaticVar
- g_GlobalVar
- int *ptr not int* ptr, due to int* a, b being misleading
- use #pragma once instead of include-guards (simpler, would only break in specific situations (e.g. same file in multiple places), but avoids typos and is supported by the big compilers)
- never "using namespace std;"
- use .cpp/.h
- headers shouldn't depend on others being included first, but anyway, include headers from local scope to global scope (e.g. my headers before system headers)

*/

#include <GLFW/glfw3.h>
#include <iostream>


int main(int argc, const char *argv[]) {


	std::cout << "IT BEGINS!" << std::endl;

	return 0;
}