Import("env")

def before_build(source, target, env):
    """
    Pre-build script to ensure SPIFFS data is properly built and uploaded
    """
    env.Execute("pio run -t buildfs")

env.AddPreAction("buildprog", before_build)
