 
#include <gui/desktop.h>

using namespace myos;
using namespace myos::common;
using namespace myos::gui;


Desktop::Desktop(common::int32_t w, common::int32_t h,
                common::uint8_t r, common::uint8_t g, common::uint8_t b)
:   CompositeWidget(0,0,0, w,h,r,g,b),
    MouseEventHandler()
{
    MouseX = w/2;
    MouseY = h/2;
}

Desktop::~Desktop()
{
}

void Desktop::Draw(common::GraphicsContext* gc)
{
    CompositeWidget::Draw(gc);
    
    for(int i = 0; i < 4; i++)
    {
        gc -> PutPixel(MouseX-i, MouseY, 0xFF, 0xFF, 0xFF);
        gc -> PutPixel(MouseX+i, MouseY, 0xFF, 0xFF, 0xFF);
        gc -> PutPixel(MouseX, MouseY-i, 0xFF, 0xFF, 0xFF);
        gc -> PutPixel(MouseX, MouseY+i, 0xFF, 0xFF, 0xFF);
    }
}
            
void Desktop::OnMouseDown(myos::common::uint8_t button)
{
    CompositeWidget::OnMouseDown(MouseX, MouseY, button);
}

void Desktop::OnMouseUp(myos::common::uint8_t button)
{
    CompositeWidget::OnMouseUp(MouseX, MouseY, button);
}

void Desktop::OnMouseMove(int x, int y)
{
    x /= 4;
    y /= 4;
    
    int32_t newMouseX = MouseX + x;
    if(newMouseX < 0) newMouseX = 0;
    if(newMouseX >= w) newMouseX = w - 1;
    
    int32_t newMouseY = MouseY + y;
    if(newMouseY < 0) newMouseY = 0;
    if(newMouseY >= h) newMouseY = h - 1;
    
    CompositeWidget::OnMouseMove(MouseX, MouseY, newMouseX, newMouseY);
    
    MouseX = newMouseX;
    MouseY = newMouseY;
}
