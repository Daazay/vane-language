#include <utest/utest.h>

#include <vane/utils/path.h>

struct TestPath {
    String path1;
    String path2;
};

UTEST_F_SETUP(TestPath) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = STRING_EMPTY;
}

UTEST_F_TEARDOWN(TestPath) {
    string_destroy(&utest_fixture->path1);
    string_destroy(&utest_fixture->path2);
}

// get absolute path

UTEST_F(TestPath, get_absolute_path_empty_path) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = get_absolute_path(&utest_fixture->path1);

    ASSERT_TRUE(string_is_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, get_absolute_path_not_exist) {
    utest_fixture->path1 = string_from_cstr("./file_1234");
    utest_fixture->path2 = get_absolute_path(&utest_fixture->path1);

    ASSERT_FALSE(string_is_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, get_absolute_path_exist) {
    utest_fixture->path1 = string_from_cstr("./projects");
    utest_fixture->path2 = get_absolute_path(&utest_fixture->path1);

    ASSERT_FALSE(string_is_empty(&utest_fixture->path2));
}

// get normalized path

UTEST_F(TestPath, get_normalized_path_empty_path) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = get_normalized_path(&utest_fixture->path1);

    ASSERT_TRUE(string_is_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, get_normalized_path) {
#if defined(PLATFORM_WINDOWS)
    const char* expected = "..\\..\\directory\\dir2";
#else
    const char* expected = "/../../directory/dir2";
#endif
    utest_fixture->path1 = string_from_cstr("//\\\\./\\//../../\\//./directory/inner/..\\dir2");
    utest_fixture->path2 = get_normalized_path(&utest_fixture->path1);

    ASSERT_TRUE(string_eq_cstr(&utest_fixture->path2, expected));
}

// join str

UTEST_F(TestPath, join_str) {
#if defined(PLATFORM_WINDOWS)
    const char* expected = "dir\\inner\\file.txt";
#else
    const char* expected = "dir/inner/file.txt";
#endif

    String comp1 = (String){ .text = "dir", .len = 3 };
    String comp2 = (String){ .text = "inner", .len = 5 };
    String comp3 = (String){ .text = "file.txt", .len = 8 };

    utest_fixture->path1 = path_join_str(3, (const String * []) { &comp1, & comp2, & comp3 });

    ASSERT_TRUE(string_eq_cstr(&utest_fixture->path1, expected));
}

// join cstr

UTEST_F(TestPath, join_cstr) {
#if defined(PLATFORM_WINDOWS)
    const char* expected = "dir\\inner\\file.txt";
#else
    const char* expected = "dir/inner/file.txt";
#endif

    const char* comp1 = "dir";
    const char* comp2 = "inner";
    const char* comp3 = "file.txt";

    utest_fixture->path1 = path_join_cstr(3, (const char* []) { comp1, comp2, comp3 });

    ASSERT_TRUE(string_eq_cstr(&utest_fixture->path1, expected));
}

// is path absolute

bool is_path_absolute(const String* path);

// is path exists

UTEST_F(TestPath, is_path_exists_empty_path) {
    utest_fixture->path1 = STRING_EMPTY;

    ASSERT_FALSE(is_path_exists(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_exists_not_exist) {
    utest_fixture->path1 = string_from_cstr("./not_existing_file1234");

    ASSERT_FALSE(is_path_exists(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_exists_dir) {
    utest_fixture->path1 = string_from_cstr("./projects");

    ASSERT_TRUE(is_path_exists(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_exists_file) {
    utest_fixture->path1 = string_from_cstr("./projects/premake5.lua");

    ASSERT_TRUE(is_path_exists(&utest_fixture->path1));
}

// is path a dir

UTEST_F(TestPath, is_path_a_dir_empty_path) {
    utest_fixture->path1 = STRING_EMPTY;

    ASSERT_FALSE(is_path_a_dir(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_a_dir_not_exist) {
    utest_fixture->path1 = string_from_cstr("./not_existing_dir/");

    ASSERT_FALSE(is_path_a_dir(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_a_dir_dir) {
    utest_fixture->path1 = string_from_cstr("./projects");

    ASSERT_TRUE(is_path_a_dir(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_a_dir_file) {
    utest_fixture->path1 = string_from_cstr("./projects/premake5.lua");

    ASSERT_FALSE(is_path_a_dir(&utest_fixture->path1));
}

// is path a file

UTEST_F(TestPath, is_path_a_file_empty_path) {
    utest_fixture->path1 = STRING_EMPTY;

    ASSERT_FALSE(is_path_a_file(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_a_file_not_exist) {
    utest_fixture->path1 = string_from_cstr("./not_existing_dir/");

    ASSERT_FALSE(is_path_a_file(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_a_file_dir) {
    utest_fixture->path1 = string_from_cstr("./projects");

    ASSERT_FALSE(is_path_a_file(&utest_fixture->path1));
}

UTEST_F(TestPath, is_path_a_file_file) {
    utest_fixture->path1 = string_from_cstr("./projects/premake5.lua");

    ASSERT_TRUE(is_path_a_file(&utest_fixture->path1));
}