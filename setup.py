from distutils.core import setup, Extension

sigma16 = Extension(
    "sigma16",
    ["src/sigma16module.c", "src/tracing.c", "src/vm.c"],
    extra_compile_args=["-O2", "-DPYTHON_COMPAT", "-flto"],
)

setup(
    name="sigma16",
    version="1.0",
    ext_modules=[sigma16],
    description="Python bindings for Sigma16 emulator.",
)
