
target_sources(${library_name} PRIVATE
    anyvalue_build_adapter.cpp
    anyvalue_from_pvxs_builder.cpp
    dto_conversion_utils.cpp
    dto_scalar_conversion_utils.cpp
    dto_typecode_conversion_utils.cpp
    pvxs_type_builder.cpp
    pvxs_utils.cpp
    pvxs_value_builder.cpp
)

add_subdirectory(ca)