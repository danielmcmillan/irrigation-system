import subprocess
Import("env", "projenv")

def after_build(source, target, env):
    for file in target:
        path = file.get_path()
        print("Copying tool binary:", path)
        p = subprocess.run(["cp", path, "tool"])

env.AddPostAction("$BUILD_DIR/program", after_build)
