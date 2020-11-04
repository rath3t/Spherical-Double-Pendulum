# Pendulum

## Prerequisites for running in the browser

- Have [emscripten
  installed](https://emscripten.org/docs/getting_started/downloads.html) and
  its SDK linked in your shell `source <emsdk_dir>/emsdk_env.sh`
- Allow the build script to be executable `chmod +x ./scripts/build.sh`

## Running in the browser

```bash
$ ./scripts/build.sh && python -m SimpleHTTPServer 1234 dist
```

and then open http://localhost:1234 in your browser.
