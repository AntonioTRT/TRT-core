#include "app_selector.h"
#include "app.h"

int main(void)
{
    app_init();

    while (1)
    {
        app_loop();
    }
}
