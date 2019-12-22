load("@local_config_python//:py.bzl", "PYTHON2_BIN")
load(
    "//bazel:repo.bzl",
    "failed_to_find_bash_bin_path",
    "get_bash_bin_path",
)

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("node-addon-api install failed: %s\n" % (msg))

def _install_node_addon_api_impl(repository_ctx):
    repository_ctx.symlink(Label("//third_party/node_addon_api:node_addon_api.BUILD"), "BUILD")
    repository_ctx.symlink(Label("//third_party/node_addon_api:package.json"), "package.json")

    bash = get_bash_bin_path(repository_ctx)
    if bash == None:
        _fail(failed_to_find_bash_bin_path(repository_ctx))

    cmd = [bash, "-c", "npm install"]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        _fail("Failed to npm install: %s." % result.stderr)

    cmd = [bash, "-c", "npx node-gyp install --devdir ."]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        _fail("Failed to node-gyp install: %s." % result.stderr)

    cmd = [bash, "-c", "node --version"]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        _fail("Failed to node --version: %s." % result.stderr)

    version = result.stdout.strip()
    if version.startswith("v"):
        version = version[1:]

    repository_ctx.template(
        "node_addon_api.bzl",
        Label("//third_party/node_addon_api:node_addon_api.bzl.tpl"),
        {
            "%{NODE_VERSION}": "\"%s\"" % (version),
        },
    )

install_node_addon_api = repository_rule(
    implementation = _install_node_addon_api_impl,
)
