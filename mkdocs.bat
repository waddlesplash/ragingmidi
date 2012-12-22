if "%robodocexe%"=="" set robodocexe=robodoc
%robodocexe% --src src/Gui --doc ./docs --multidoc --index --html --rc robodoc_cpp.rc --nopre --cmode --nodesc
