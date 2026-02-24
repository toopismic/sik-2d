#include "sid2d.h"

int main() {
    // Инициализация движка
    SiD2D_Init("windows");
    
    // Запуск главного цикла
    SiD2D_Run();
    
    // Завершение
    SiD2D_Shutdown();
    return 0;
}