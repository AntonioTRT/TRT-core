Import("env")

env.BuildSources("$BUILD_DIR/trt_core", "$PROJECT_DIR/../trt-core/core")
env.BuildSources("$BUILD_DIR/trt_core_modules", "$PROJECT_DIR/../trt-core/modules")
