REM Uncomment if you want to use a custom ostdecoder in OSTViewer
REM set DECODER=%~dp0bin\ostdecoder.exe

REM Uncomment if you want to use a custom TraceServer in OSTViewer
REM set TRACESERVER=%~dp0bin\TraceServer.exe

REM start "Trace Viewer" "%~dp0trcviewer\trcviewer.exe" -clean --clear-workspace-preferences -data @noDefault %*
start "Trace Viewer" "%~dp0trcviewer\trcviewer.exe" -clean -data @noDefault %*
