"""Repository rule for Python autoconfiguration.

`python_configure` depends on the following environment variables:

  * `PYTHON2_BIN_PATH`: location of python2 binary.
"""

load(
    "//bazel:repo.bzl",
    "failed_to_find_bash_bin_path",
    "get_bash_bin_path",
    "get_bin_path",
)

_PYTHON2_BIN_PATH = "PYTHON2_BIN_PATH"

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("python configuration failed: %s\n" % (msg))

def _get_python2_bin(repository_ctx):
    """Gets the python2 bin path."""
    python2_bin = get_bin_path(repository_ctx, "python2", _PYTHON2_BIN_PATH)
    if python2_bin == None:
        _fail("Failed to find python2 path.")
    return python2_bin

def _check_python_bin(repository_ctx, python_bin):
    """Checks the python bin path."""
    cmd = '[[ -x "%s" ]] && [[ ! -d "%s" ]]' % (python_bin, python_bin)
    result = repository_ctx.execute([get_bash_bin_path(repository_ctx), "-c", cmd])
    if result.return_code == 1:
        _fail("Is it the python binary(%s)?" % python_bin)

def _python_configure_impl(repository_ctx):
    """Creates the repository containing files set up to build with Python."""
    python2_bin = _get_python2_bin(repository_ctx)
    _check_python_bin(repository_ctx, python2_bin)

    repository_ctx.template("BUILD", Label("//third_party/py:BUILD"))
    repository_ctx.template(
        "py.bzl",
        Label("//third_party/py:py.bzl.tpl"),
        {
            "%{PYTHON2_BIN}": "\"%s\"" % (python2_bin),
        },
    )

python_configure = repository_rule(
    implementation = _python_configure_impl,
    environ = [
        _PYTHON2_BIN_PATH,
    ],
)
"""Detects and configures the local Python.

Add the following to your WORKSPACE FILE:

```python
python_configure(name = "local_config_python")
```

Args:
  name: A unique name for this workspace rule.
"""
