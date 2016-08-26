#ifndef TICK_H
#define TICK_H

#include <QThread>

class Tick : public QThread
{
    Q_OBJECT


public:

    //====================================================================================
    // Constructors
    //====================================================================================

    /***********************************************************
     * @brief Create a tick object which emit an event each N milliseconds.
     ***********************************************************/
    Tick(int a_iMilliseconds = 50);


signals:

    //====================================================================================
    // Signals
    //====================================================================================

    /***********************************************************
     * @brief Called when time elapsed.
     ***********************************************************/
    void ticked();


protected:

    //====================================================================================
    // Private Operations
    //====================================================================================

    /***********************************************************
     * @brief Run this thread.
     ***********************************************************/
    void run();


    //====================================================================================
    // Fields
    //====================================================================================

    int m_iMilliseconds;
};

#endif // TICK_H
