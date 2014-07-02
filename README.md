clang-refactor 
==============

This is a early attempt to write a refactor for c and c++ programs based on clang-modernize and oclint rules.

in order to get refactoring for c++ i started to implement some refactorings.
i can not garantee that these will not break your code.
my aim is to improve the refactorings so that the code will stay intact.
the refactor is intended to run stand alone and can be integrated into editors by very simple 
plugins. 

i already wrote a plugin for vim 

[vim-clang-refactor]: https://github.com/realincubus/vim-clang-refactor

### Installation

in a stable llvm+clang+extra-tools build do
```sh
git clone https://github.com/realincubus/clang-refactor.git ${LLVM_ROOT}/tools/clang/tools/extra/clang-refactor
``` 

and add `add_subdirectory(clang-refactor)` to `${LLVM_ROOT}/tools/clang/tools/extra/CMakeLists.txt`
this will add the refactor to the build chain.

now go to `${LLVM_BUILD_DIR}` and run make ( to speed it up `make -j ${number_of_threads}` )
the executable will go to `${LLVM_BUILD_DIR}/bin`

i did not try to but running `sudo make install` should install this on your system.
if you did not install this remember to put `${LLVM_BUILD_DIR}/bin` into your PATH variable.

### Project Setup

just as clang-modernize the program needs a compile_commands.json file.
the easiest way to generate one is to use cmake.
simply add `SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)` to your CMakeLists.txt

### Refactorings

you can get a list of available transformations from clang-refactor with `clang-refactor -help`

  -collapse-ifstmt              - merges if statements which include other if statements
  -extract-method               - fill in
  -from-zero                    - fill in
  -local-iterator               - transforms int i; for( i = 0 ; i < 10 ; i++ ) to for( int i = 0; i < 10 ; i++) 
  -move-to-firstuse             - fill in
  -nested-index                 - finds nested indexes and renames them to avoid naming conflicts
  -pull-temporaries             - fill in
  -remove-identity-ops          - removes a (*|/) 1 a (+|-) 0
  -remove-redundant-conditional - replaces expressions like a>b ? true : false with a > b 
  -rename-variable              - renames a variable 
  -repair-broken-nullcheck      - replairs checks like if ( a->fun() && a ) 
  -unglobal-method              - fill in
  -uninvert-logic               - inverts if statements with != 
  -use-algorithms               - fill in
  -use-compound                 - translates expressions like b = b + 1 to b += 1 
  -use-const                    - will add const to the type if the variable was never written to
  -use-emplace                  - fill in
  -use-empty                    - transforms vec.size() == 0 to vec.empty()
  -use-hypot                    - translates sqrt( pow(a,2) + pow(b,2) ) to hypot( a, b ) 
  -use-math-constants           - replaces magic numbers like 3.14159... with M_PI 
  -use-pow                      - uses power operator instead of a * a
  -use-raii                     - fill in
  -use-round                    - replaces int( 0.5 + var ) with round
  -use-std-array                - converts int a[100] to std::array<int,100> a;
  -use-swap                     - finds the three line swap and replaces this with std::swap
  -use-unary-operators          - transforms b += 1 to ++b
  -use-vector                   - fill in

some of these transformations where devoloped for another project and are very specific,
others are still very buggy. the transformations do some checks but the checks are not perfect so 
they can possibly transform code and change semantics!!!

to run a refactoring on a whole file, go to the directory and run 

```sh
clang-refactor -use-pow main.cpp
```




