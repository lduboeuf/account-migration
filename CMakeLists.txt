cmake_minimum_required(VERSION 3.14)

project(AccountMigration VERSION 1.0 LANGUAGES CXX)

find_package(PkgConfig)
find_package(Qt5 REQUIRED COMPONENTS Core)
pkg_check_modules(ACCOUNTSQT accounts-qt5 REQUIRED)

# include_directories(
#     ${CMAKE_BINARY_DIR}
#     ${ACCOUNTSQT_INCLUDE_DIRS}
# )



add_executable(account_migration account_migration.cpp)

target_link_libraries(account_migration PRIVATE
    Qt5::Core
    ${ACCOUNTSQT_LIBRARIES}
)

target_include_directories(account_migration PRIVATE ${ACCOUNTSQT_INCLUDE_DIRS})

target_compile_definitions(account_migration PRIVATE QT_NO_KEYWORDS)

set_target_properties(account_migration PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
