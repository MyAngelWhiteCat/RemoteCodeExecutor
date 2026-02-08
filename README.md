Инструмент для создания инъекций и исполнения произвольного кода в процессы на ОС Windows предназначен для **тестирования** [лабаратории процессов](https://github.com/MyAngelWhiteCat/ProcessesLaboratory) - утилиты для динамического исследования активных процессов и выявления активнности вредоносов 


Building:

```
git clone git@github.com:MyAngelWhiteCat/RemoteCodeExecutor.git
cd RemoteCodeExecutor
mkdir build
cd build 
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Usage:

DLL injecting
```
./RemoteCodeExecutor any.dll anyprocess
```

Shellcode injecting
```
work in progress
```

