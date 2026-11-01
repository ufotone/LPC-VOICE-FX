$ErrorActionPreference = 'Stop'
$platformDir = Join-Path $PSScriptRoot 'platform'
$buildCommand = 'set -e; source /app/drumlogue/environment; cd /workspace/drumlogue/lpc-voice-fx; make; make install'
docker run --rm --user 0 -v "${platformDir}:/workspace" --entrypoint /bin/bash xiashj/logue-sdk@sha256:e4d85a16c38dc4d34b0e93cabb6378df21729688dbcd88808c84e8d41aa0c9ba -lc $buildCommand
if ($LASTEXITCODE -ne 0) { throw 'drumlogue build failed.' }
Copy-Item -LiteralPath (Join-Path $platformDir 'drumlogue/lpc-voice-fx/lpc_voice_fx.drmlgunit') -Destination (Join-Path $PSScriptRoot '../lpc_voice_fx.drmlgunit') -Force
