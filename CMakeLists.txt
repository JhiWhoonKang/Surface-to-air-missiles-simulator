cmake_minimum_required(VERSION 3.10)
project(SAM_System_Total)

add_subdirectory(LC)
add_subdirectory(LS)
add_subdirectory(Simulator)
add_subdirectory(MFR/NewMFR)

install(FILES "target_list copy.ini" DESTINATION config/global RENAME target_list.ini)
