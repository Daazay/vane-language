#include <utest/utest.h>

#include <vane/utils/file_utils.h>

struct TestFileUtils {
    String path;
    struct {
        byte* data;
        u64 size;
    } content;
};

UTEST_F_SETUP(TestFileUtils) {
    utest_fixture->path = STRING_EMPTY;

    utest_fixture->content.data = NULL;
    utest_fixture->content.size = 0;
}

UTEST_F_TEARDOWN(TestFileUtils) {
    string_destroy(&utest_fixture->path);

    free(utest_fixture->content.data);
}

// load file

UTEST_F(TestFileUtils, load_file_empty_path) {
    utest_fixture->path = STRING_EMPTY;

    IOStatus status = file_load(&utest_fixture->path, &utest_fixture->content.data, &utest_fixture->content.size);
    ASSERT_EQ(IO_STATUS_ERR_INVALID_PATH, status);
}

UTEST_F(TestFileUtils, load_file_not_exist) {
    utest_fixture->path = string_from_cstr("./not_exist");

    IOStatus status = file_load(&utest_fixture->path, &utest_fixture->content.data, &utest_fixture->content.size);
    ASSERT_EQ(IO_STATUS_ERR_FILE_NOT_FOUND, status);
}

UTEST_F(TestFileUtils, load_file_dir) {
    utest_fixture->path = string_from_cstr("./projects");

    IOStatus status = file_load(&utest_fixture->path, &utest_fixture->content.data, &utest_fixture->content.size);
    ASSERT_EQ(IO_STATUS_ERR_FILE_NOT_FOUND, status);
}

UTEST_F(TestFileUtils, load_file_empty) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/empty_file");

    IOStatus status = file_load(&utest_fixture->path, &utest_fixture->content.data, &utest_fixture->content.size);
    ASSERT_EQ(IO_STATUS_ERR_FILE_EMPTY, status);
}

UTEST_F(TestFileUtils, load_file) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/file");

    IOStatus status = file_load(&utest_fixture->path, &utest_fixture->content.data, &utest_fixture->content.size);
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

UTEST_F(TestFileUtils, iterate_directory_empty_dir) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/empty_dir");

    u64 count = 0;
    IOStatus status = iterate_directory(&utest_fixture->path, (iterate_directory_fn)&iterate_count, &count);

    ASSERT_EQ(count, 0);
}

UTEST_F(TestFileUtils, iterate_directory_file) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/file");

    u64 count = 0;
    IOStatus status = iterate_directory(&utest_fixture->path, (iterate_directory_fn)&iterate_count, &count);

    ASSERT_EQ(status, IO_STATUS_ERR_DIR_NOT_FOUND);
    ASSERT_EQ(count, 0);
}

UTEST_F(TestFileUtils, iterate_directory) {
    utest_fixture->path = string_from_cstr("./projects/tests/resources/utils/");

    u64 count = 0;
    IOStatus status = iterate_directory(&utest_fixture->path, (iterate_directory_fn)&iterate_count, &count);

    ASSERT_EQ(status, IO_STATUS_OK);
    ASSERT_EQ(count, 4);
}