#include <utest/utest.h>

#include <vane/utils/path.h>

struct TestPath {
    union {
        String path;
        String path1;
    };
    union {
        String path2;
        String tmp1;
    };

    union {
        String path3;
        String tmp2;
    };
};

UTEST_F_SETUP(TestPath) {
    utest_fixture->path1 = (String){ 0 };
    utest_fixture->path2 = (String){ 0 };
    utest_fixture->path3 = (String){ 0 };
}

UTEST_F_TEARDOWN(TestPath) {
    string_destroy(&utest_fixture->path1);
    string_destroy(&utest_fixture->path2);
    string_destroy(&utest_fixture->path3);
}

UTEST_F(TestPath, path_to_absolute1) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = path_to_absolute(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_to_absolute2) {
    utest_fixture->path1 = string_from_cstr(".");
    utest_fixture->path2 = path_to_absolute(&utest_fixture->path1);

    ASSERT_TRUE(is_path_absolute(&utest_fixture->path2));
}

UTEST_F(TestPath, path_to_absolute3) {
    utest_fixture->path1 = string_from_cstr("file.txt");
    utest_fixture->path2 = path_to_absolute(&utest_fixture->path1);

    ASSERT_TRUE(is_path_absolute(&utest_fixture->path2));
}

UTEST_F(TestPath, path_to_absolute4) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("C:\\file.txt");
#else
    utest_fixture->path1 = string_from_cstr("/file.txt");
#endif
    utest_fixture->path2 = path_to_absolute(&utest_fixture->path1);
    ASSERT_TRUE(is_path_absolute(&utest_fixture->path2));
}

UTEST_F(TestPath, path_normalize1) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = path_normalize(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_normalize2) {
    utest_fixture->path1 = string_from_cstr(".");
    utest_fixture->path2 = path_normalize(&utest_fixture->path1);

    ASSERT_STREQ(".", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_normalize3) {
    utest_fixture->path1 = string_from_cstr("..");
    utest_fixture->path2 = path_normalize(&utest_fixture->path1);

    ASSERT_STREQ("..", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_normalize4) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\b\\..\\c");
#else
    utest_fixture->path1 = string_from_cstr("a/b/../c");
#endif
    utest_fixture->path2 = path_normalize(&utest_fixture->path1);

#if defined(PLATFORM_WINDOWS)
    ASSERT_STREQ("a\\c", utest_fixture->path2.text);
#else
    ASSERT_STREQ("a/c", utest_fixture->path2.text);
#endif
}

UTEST_F(TestPath, path_normalize5) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr(".\\.\\a\\\\b");
#else
    utest_fixture->path1 = string_from_cstr("././a//b");
#endif
    utest_fixture->path2 = path_normalize(&utest_fixture->path1);

#if defined(PLATFORM_WINDOWS)
    ASSERT_STREQ("a\\b", utest_fixture->path2.text);
#else
    ASSERT_STREQ("a/b", utest_fixture->path2.text);
#endif
}

UTEST_F(TestPath, get_current_working_dir1) {
    utest_fixture->path = get_current_working_dir();
    ASSERT_TRUE(is_path_absolute(&utest_fixture->path));
}

UTEST_F(TestPath, path_get_dir1) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_dir2) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("\\");
#else
    utest_fixture->path1 = string_from_cstr("/");
#endif
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

#if defined(PLATFORM_WINDOWS)
    ASSERT_STREQ("\\", utest_fixture->path2.text);
#else
    ASSERT_STREQ("/", utest_fixture->path2.text);
#endif
}

UTEST_F(TestPath, path_get_dir3) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\b\\");
#else
    utest_fixture->path1 = string_from_cstr("a/b/");
#endif
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

#if defined(PLATFORM_WINDOWS)
    ASSERT_STREQ("a\\b", utest_fixture->path2.text);
#else
    ASSERT_STREQ("a/b", utest_fixture->path2.text);
#endif
}

UTEST_F(TestPath, path_get_dir4) {
    utest_fixture->path1 = string_from_cstr("a");
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

    ASSERT_STREQ(".", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_dir5) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\");
#else
    utest_fixture->path1 = string_from_cstr("a/");
#endif
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

    ASSERT_STREQ("a", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_dir6) {
    utest_fixture->path1 = string_from_cstr(".");
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

    ASSERT_STREQ(".", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_dir7) {
    utest_fixture->path1 = string_from_cstr("..");
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

    ASSERT_STREQ(".", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_dir8) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\b\\c.txt");
#else
    utest_fixture->path1 = string_from_cstr("a/b/c.txt");
#endif
    utest_fixture->path2 = path_get_dir(&utest_fixture->path1);

#if defined(PLATFORM_WINDOWS)
    ASSERT_STREQ("a\\b", utest_fixture->path2.text);
#else
    ASSERT_STREQ("a/b", utest_fixture->path2.text);
#endif
}

UTEST_F(TestPath, path_get_name1) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = path_get_name(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_name2) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("\\");
#else
    utest_fixture->path1 = string_from_cstr("/");
#endif
    utest_fixture->path2 = path_get_name(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_name3) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\b\\");
#else
    utest_fixture->path1 = string_from_cstr("a/b/");
#endif
    utest_fixture->path2 = path_get_name(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_name4) {
    utest_fixture->path1 = string_from_cstr("a");
    utest_fixture->path2 = path_get_name(&utest_fixture->path1);

    ASSERT_STREQ("a", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_name5) {
    utest_fixture->path1 = string_from_cstr(".");
    utest_fixture->path2 = path_get_name(&utest_fixture->path1);

    ASSERT_STREQ(".", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_name6) {
    utest_fixture->path1 = string_from_cstr("..");
    utest_fixture->path2 = path_get_name(&utest_fixture->path1);

    ASSERT_STREQ("..", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_name7) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\b\\c.txt");
#else
    utest_fixture->path1 = string_from_cstr("a/b/c.txt");
#endif
    utest_fixture->path2 = path_get_name(&utest_fixture->path1);

    ASSERT_STREQ("c.txt", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_get_ext1) {
    utest_fixture->path1 = STRING_EMPTY;
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_ext2) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("\\");
#else
    utest_fixture->path1 = string_from_cstr("/");
#endif
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_ext3) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\b\\");
#else
    utest_fixture->path1 = string_from_cstr("a/b/");
#endif
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_ext4) {
    utest_fixture->path1 = string_from_cstr("a");
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_ext5) {
    utest_fixture->path1 = string_from_cstr(".");
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_ext6) {
    utest_fixture->path1 = string_from_cstr(".git");
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_ext7) {
    utest_fixture->path1 = string_from_cstr("..");
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_TRUE(is_string_empty(&utest_fixture->path2));
}

UTEST_F(TestPath, path_get_ext8) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path1 = string_from_cstr("a\\b\\c.txt");
#else
    utest_fixture->path1 = string_from_cstr("a/b/c.txt");
#endif
    utest_fixture->path2 = path_get_ext(&utest_fixture->path1);

    ASSERT_STREQ("txt", utest_fixture->path2.text);
}

UTEST_F(TestPath, path_join_cstr1) {
    const char* p1 = "a";
    const char* p2 = "b";
    const char* p3 = "c";

    utest_fixture->path = path_join_cstr(3, (const char* []) { p1, p2, p3 });

#if defined(PLATFORM_WINDOWS)
    ASSERT_STREQ("a\\b\\c", utest_fixture->path.text);
#else
    ASSERT_STREQ("a/b/c", utest_fixture->path.text);
#endif
}

UTEST_F(TestPath, path_join_str1) {
    String p1 = { .text = "a", .len = 1 };
    String p2 = { .text = "b", .len = 1 };
    String p3 = { .text = "c", .len = 1 };

    utest_fixture->path = path_join_str(3, (const String* []) { &p1, &p2, &p3 });

#if defined(PLATFORM_WINDOWS)
    ASSERT_STREQ("a\\b\\c", utest_fixture->path.text);
#else
    ASSERT_STREQ("a/b/c", utest_fixture->path.text);
#endif
}

UTEST_F(TestPath, is_path_absolute1) {
    utest_fixture->path = STRING_EMPTY;

    ASSERT_FALSE(is_path_absolute(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_absolute2) {
    utest_fixture->path = string_from_cstr(".");

    ASSERT_FALSE(is_path_absolute(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_absolute3) {
    utest_fixture->path = string_from_cstr("..");

    ASSERT_FALSE(is_path_absolute(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_absolute4) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path = string_from_cstr("a\\b\\c.txt");
#else
    utest_fixture->path = string_from_cstr("a/b/c.txt");
#endif

    ASSERT_FALSE(is_path_absolute(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_absolute5) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path = string_from_cstr("C:\\");
#else
    utest_fixture->path = string_from_cstr("/");
#endif

    ASSERT_TRUE(is_path_absolute(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_absolute6) {
#if defined(PLATFORM_WINDOWS)
    utest_fixture->path = string_from_cstr("C:\\a\\b\\c.txt");
#else
    utest_fixture->path = string_from_cstr("/a/b/c.txt");
#endif

    ASSERT_TRUE(is_path_absolute(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_exist1) {
    utest_fixture->path = STRING_EMPTY;

    ASSERT_FALSE(is_path_exist(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_exist2) {
    utest_fixture->path = string_from_cstr("not_existing_path");

    ASSERT_FALSE(is_path_exist(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_exist3) {
    utest_fixture->path = string_from_cstr(".");

    ASSERT_TRUE(is_path_exist(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_exist4) {
    utest_fixture->path = string_from_cstr("..");

    ASSERT_TRUE(is_path_exist(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_exist5) {
    utest_fixture->path = string_from_cstr(__FILE__);

    ASSERT_TRUE(is_path_exist(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_dir1) {
    utest_fixture->path = STRING_EMPTY;

    ASSERT_FALSE(is_path_dir(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_dir2) {
    utest_fixture->path = string_from_cstr("not_existing_path");

    ASSERT_FALSE(is_path_dir(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_dir3) {
    utest_fixture->path = string_from_cstr(".");

    ASSERT_TRUE(is_path_dir(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_dir4) {
    utest_fixture->path = string_from_cstr("..");

    ASSERT_TRUE(is_path_dir(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_dir5) {
    utest_fixture->path = string_from_cstr(__FILE__);

    ASSERT_FALSE(is_path_dir(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_file1) {
    utest_fixture->path = STRING_EMPTY;

    ASSERT_FALSE(is_path_file(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_file2) {
    utest_fixture->path = string_from_cstr("not_existing_path");

    ASSERT_FALSE(is_path_file(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_file3) {
    utest_fixture->path = string_from_cstr(".");

    ASSERT_FALSE(is_path_file(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_file4) {
    utest_fixture->path = string_from_cstr("..");

    ASSERT_FALSE(is_path_file(&utest_fixture->path));
}

UTEST_F(TestPath, is_path_file5) {
    utest_fixture->path = string_from_cstr(__FILE__);

    ASSERT_TRUE(is_path_file(&utest_fixture->path));
}