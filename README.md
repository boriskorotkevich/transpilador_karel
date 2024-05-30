# Para compilar el transpilador de manera nativa:
   g++ -std=c++20 main.cpp -o main        (o main.exe para Windows)
   
# Para compilar el transpilador con WebAssembly
   em++ -O3 -fexceptions -std=c++23 main.cpp -s EXPORTED_RUNTIME_METHODS=['cwrap'] -s MODULARIZE=1 -s EXPORT_NAME="cargador_wasm" -o main.js
   
La carpeta de "Interfaz web" ya trae el resultado de la compilación a WebAssembly. Se necesita abrir el index.html desde un servidor web (por ejemplo, http://localhost).
