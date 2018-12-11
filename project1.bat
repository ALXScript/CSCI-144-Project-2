set arg1=%1
set arg2=%2
g++ -std=c++11 main.cpp -lpthread
sleep 2
a.exe %arg1% %arg2%