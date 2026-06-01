# tests/CMakeLists.txt - 单元测试

livio_add_test(
    NAME    test_log
    SOURCES test_main.cpp
    LIBS    LivioArk
)

livio_add_test(
    NAME    test_comm_headers
    SOURCES test_comm_headers.cpp
    LIBS    LivioArk
)

livio_add_test(
    NAME    test_msgbus_headers
    SOURCES test_msgbus_headers.cpp
    LIBS    LivioArk
)

livio_add_test(
    NAME    test_msgbus_basic
    SOURCES test_msgbus_basic.cpp
    LIBS    LivioArk
)

livio_add_test(
    NAME    test_log_compile_time
    SOURCES test_log_compile_time.cpp
    LIBS    LivioArk
)
if(TARGET test_log_compile_time)
    target_compile_definitions(test_log_compile_time PRIVATE
        LIVIO_LOG_ACTIVE_LEVEL=LIVIO_LOG_LEVEL_INFO
    )
endif()

livio_add_test(
    NAME    test_msgbus_async_wait
    SOURCES test_msgbus_async_wait.cpp
    LIBS    LivioArk
)
