ROOT_PATH = (_WORKING_DIR .. "/")

DEPENDENCIES_PATH  = path.join(ROOT_PATH, "dependencies")
PROJECTS_PATH      = path.join(ROOT_PATH, "projects")
BUILD_PATH         = path.join(ROOT_PATH, "build")

BUILD_BIN_PATH = path.join(BUILD_PATH, "bin/%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}")
BUILD_OBJ_PATH = path.join(BUILD_PATH, "obj/%{cfg.buildcfg}-%{cfg.platform}/%{prj.name}")