file(GLOB source_files "*.cpp")

target_sources(${library_name}
    PRIVATE ${source_files}
)
