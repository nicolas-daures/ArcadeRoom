#include "tick.h"


//====================================================================================
// Constructors
//====================================================================================

Tick::Tick(int a_iMilliseconds)
: m_iMilliseconds(a_iMilliseconds)
{
    setParent(0);
    moveToThread(this);
}


//====================================================================================
// Private Operations
//====================================================================================

void Tick::run()
{
    while(true)
    {
        msleep(m_iMilliseconds);
        emit ticked();
    }
}
