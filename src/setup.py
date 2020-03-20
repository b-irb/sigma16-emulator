from distutils.core import setup, Extension
setup(name="sigma16", version="1.0",
        ext_modules=[Extension("sigma16", ["emulator/sigma16module.c", "emulator/tracing.c", "emulator/vm.c"],
            extra_compile_args = ["-O2", "-DPYTHON_COMPAT", "-DENABLE_TRACE", "-flto"])])
