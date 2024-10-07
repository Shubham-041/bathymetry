cmake_minimum_required(VERSION 3.4)
project(Tesing)

find_package(Ceres REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(PCL 1.8 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(g2o REQUIRED)
find_package(yaml-cpp)
# find_package(Boost 1.74 REQUIRED)
find_package(Boost COMPONENTS iostreams date_time math_c99 regex REQUIRED)

find_package(LibXTF REQUIRED)
find_package(LibGSF REQUIRED)
find_package(EigenCereal REQUIRED)
find_package(DataTools REQUIRED)

include(GNUInstallDirs)

include_directories(submaps_tools/include)
include_directories(graph_optimization/include)
include_directories(registration/include)
include_directories(meas_models/include)
include_directories(bathy_slam/include)

include_directories(
	${CERES_INCLUDE_DIRS}
	${EIGEN_INCLUDE_DIRS} 
    ${G2O_INCLUDE_DIRS} 
	${PCL_INCLUDE_DIRS}
    ${OPENCV_INCLUDE_DIRS}
    ${yaml_cpp_INCLUDE_DIRS}
    ${BOOST_INCLUDE_DIRS}
)



add_library(submaps_tools  submaps_tools/src/submaps.cpp)
target_link_libraries(submaps_tools PUBLIC 
                      std_data 
                      navi_data 
                      data_transforms 
                      Boost::date_time
                      ${PCL_LIBRARIES})


add_library(utils_g2o  graph_optimization/src/utils_g2o.cpp)
target_link_libraries(utils_g2o PUBLIC
                      submaps_tools 
                      ceres 
                      g2o_core g2o_stuff g2o_types_sba g2o_types_slam3d
                      g2o_csparse_extension g2o_solver_csparse cholmod g2o_solver_cholmod
                      ${G2O_LIBS})


add_library(graph_construction graph_optimization/src/graph_construction.cpp)
target_link_libraries(graph_construction PUBLIC
                     submaps_tools 
                     ceres 
                     fmt
                     ${G2O_LIBS})

add_library(read_g2o  graph_optimization/src/read_g2o.cpp)
target_link_libraries(read_g2o PUBLIC
                      ceres 
                      ${GFLAGS_LIBRARIES})

add_library(ceres_optimizer 
            graph_optimization/src/ceres_optimizer.cpp
            graph_optimization/src/types.cpp)
target_link_libraries(ceres_optimizer PUBLIC 
                      ceres 
                      submaps_tools 
                      ${GFLAGS_LIBRARIES})


add_library(utils_visualization 
            registration/src/utils_visualization.cpp)           

target_link_libraries( utils_visualization PUBLIC
    submaps_tools
    graph_construction
    ceres_optimizer
    fmt
    ${PCL_LIBRARIES}
)

add_library(gicp_registration 
            registration/src/gicp_reg.cpp)         

target_link_libraries(gicp_registration PUBLIC
utils_visualization
submaps_tools
benchmark
fmt
${PCL_LIBRARIES}
${YAML_CPP_LIBRARIES}
)


add_library(multibeam_simple meas_models/src/multibeam_simple.cpp)
target_link_libraries(multibeam_simple PUBLIC
                      submaps_tools ${PCL_LIBRARIES})


add_library(slam_solver bathy_slam/src/bathy_slam.cpp)
target_link_libraries(slam_solver PUBLIC 
                      submaps_tools 
                      graph_construction 
                      gicp_registration
                      utils_g2o)



## APP
add_executable(bathy_slam_real
               apps/src/test_slam_real.cpp)           
target_link_libraries(bathy_slam_real
                      gicp_registration
                      utils_g2o
                      submaps_tools
                      graph_construction
                      ceres_optimizer
                      read_g2o
                      data_transforms
                      benchmark
                      std_data
                      slam_solver
                      fmt
                      ${YAML_CPP_LIBRARIES})           


add_executable(read_auv_data
              apps/src/read_auv_data.cpp)

target_link_libraries(read_auv_data
                      std_data 
                      navi_data 
                      gsf_data 
                      all_data 
                      xtf_data 
                      csv_data 
                      data_transforms 
                      benchmark 
                      submaps_tools 
                      utils_visualization)


# add_executable(a test.cpp
# )
# target_link_libraries(a 
#                     ceres
#                     gicp_registration
#                     utils_g2o
#                     submaps_tools
#                     graph_construction
#                     ceres_optimizer
#                     read_g2o
#                     data_transforms
#                     benchmark
#                     std_data
#                     slam_solver
#                     fmt
#                     ${YAML_CPP_LIBRARIES}
#                     ${G2O_LIBS}
#                     )