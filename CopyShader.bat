for /r ".\DX11\HLSL" %%f in (*.fx) do (
    copy /y "%%f" ".\Game\Content\Shader\"
)
