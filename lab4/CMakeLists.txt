add_executable(
    hamarc
    main.cpp
    utils.cpp
    hamming.cpp
    cli.cpp
    archive.cpp
)

target_compile_features(hamarc PRIVATE cxx_std_20)
