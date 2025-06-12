#include <utest/utest.h>

#include <vane/utils/file_utils.h>

struct TestFileUtils {
    String path;

    FileContent fc;
};

UTEST_F_SETUP(TestFileUtils) {
    utest_fixture->path = STRING_EMPTY;

    utest_fixture->fc = (FileContent){ 0 };
}

UTEST_F_TEARDOWN(TestFileUtils) {
    string_destroy(&utest_fixture->path);

    file_content_destroy(&utest_fixture->fc);
}

// load file content

UTEST_F(TestFileUtils, file_content_load_empty_path) {
    utest_fixture->path = STRING_EMPTY;

    IOStatus status = file_content_load(&utest_fixture->fc, &utest_fixture->path);
    ASSERT_EQ(IO_STATUS_ERR_INVALID_PATH, status);
}

UTEST_F(TestFileUtils, file_content_load_not_exist) {
    utest_fixture->path = string_from_cstr("./not_exist");

    IOStatus status = file_content_load(&utest_fixture->fc, &utest_fixture->path);
    ASSERT_EQ(IO_STATUS_ERR_FILE_NOT_FOUND, status);
}

UTEST_F(TestFileUtils, file_content_load_dir) {
    utest_fixture->path = string_from_cstr("./projects");

    IOStatus status = file_content_load(&utest_fixture->fc, &utest_fixture->path);
    ASSERT_EQ(IO_STATUS_ERR_FILE_NOT_FOUND, status);
}

UTEST_F(TestFileUtils, file_content_load_empty) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/empty_file");

    IOStatus status = file_content_load(&utest_fixture->fc, &utest_fixture->path);
    ASSERT_EQ(IO_STATUS_ERR_FILE_EMPTY, status);
}

UTEST_F(TestFileUtils, file_content_load) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/file");

    IOStatus status = file_content_load(&utest_fixture->fc, &utest_fixture->path);
    ASSERT_EQ(IO_STATUS_OK, status);
}

// iterate directory

static bool iterate_count(const String* name, bool is_dir, u64 size, void* data) {
    (*(u64*)data)++;
    return true;
}

UTEST_F(TestFileUtils, iterate_directory_empty_path) {
    utest_fixture->path = STRING_EMPTY;

    u64 count = 0;
    IOStatus status = iterate_directory(&utest_fixture->path, (iterate_directory_fn)&iterate_count, &count);

    ASSERT_EQ(status, IO_STATUS_ERR_INVALID_PATH);
    ASSERT_EQ(count, 0);
}

UTEST_F(TestFileUtils, iterate_directory_not_exist) {
    utest_fixture->path = string_from_cstr("./not_exist");

    u64 count = 0;
    IOStatus status = iterate_directory(&utest_fixture->path, (iterate_directory_fn)&iterate_count, &count);

    ASSERT_EQ(status, IO_STATUS_ERR_DIR_NOT_FOUND);
    ASSERT_EQ(count, 0);
}

UTEST_F(TestFileUtils, iterate_directory_file) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/file");

    u64 count = 0;
    IOStatus status = iterate_directory(&utest_fixture->path, (iterate_directory_fn)&iterate_count, &count);

    ASSERT_EQ(status, IO_STATUS_ERR_DIR_NOT_FOUND);
    ASSERT_GE(count, 0);
}

UTEST_F(TestFileUtils, iterate_directory) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/");

    u64 count = 0;
    IOStatus status = iterate_directory(&utest_fixture->path, (iterate_directory_fn)&iterate_count, &count);

    ASSERT_EQ(status, IO_STATUS_OK);
    ASSERT_GE(count, 0);
}