/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Command absorber to eat all key presses.
*
*/

#ifndef __CCOMMANDABSORBINGCONTROL_H
#define __CCOMMANDABSORBINGCONTROL_H

//  INCLUDES
#include <coecntrl.h>


//FORWARD DECLARATION
class CEikAppUi;
class CEikButtonGroupContainer;



// CLASS DECLARATION
/**
 * Command absorber to eat all key presses.
 *
 * Puts empty CBA on the screen and absorbs
 * all the key events.
 *
 * @lib ConnectionUi.lib
 * @since 2.1
 */
NONSHARABLE_CLASS( CCommandAbsorbingControl ) : public CCoeControl
    {
    /**
     * UI event & command flusher.
     * @since 2.1
     */
    NONSHARABLE_CLASS( CCommandFlush ) : protected CActive
        {
public:  // constructors and destructor

        /**
         * C++ default constructor.
         */
        CCommandFlush();

        /**
         * Destructor.
         */
        virtual ~CCommandFlush();


public: // New functions

        /**
         * Executes the flush.
         * @since 2.1
         */
        void Flush();


protected:  // From CActive


        /**
         * RunL(), RunError() and DoCancel()
         * implement the normal CActive usage protocol.
         */
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();



private: // New private helper methods

        /**
         * Waits the started asynchronous steps
         * to complete using the CActiveSchedulerWait.
         * @since 2.1
         */
        void WaitCompletion();


        /**
         * Stops the previously started
         * CActiveSchedulerWait wait loop.
         * @since 2.1
         */
        void Completed();

private:    // Data

        ///<Active Scheduler Wait, owned
        CActiveSchedulerWait            iWait;
        };


public:
    /**
     * Two-phased constructor.
     */
    static CCommandAbsorbingControl* NewL();

    /**
     * Destructor.
     */
    virtual ~CCommandAbsorbingControl();


public:

    /**
     * Starts absorbing commands.
     * Puts absorber to CleanUpstack.
     * Absorber must be released with
     * CleanupStack::PopAndDestroy();
     * @since 2.1
     */
    void AbsorbCommandsLC();

    /**
     * Flushes possibly buffered commands
     * from control stack. Flushing is
     * effective only if the command
     * absorber is active.
     *
     * @since 2.1
     */
    void FlushCommands();



protected:

    /**
     * Symbian OS constructor.
     */
    void ConstructL();


    /**
     * C++ default constructor.
     */
    CCommandAbsorbingControl();


public:  // From CCoeControl

    /**
     * Absorbing OfferkeyEvent() method.
     * Absorbs all offered key events (returns
     * always EKeyWasConsumed).
     *
     * @since 2.1
     * @param aKeyEvent The key event to check
     * @param aType The event type.
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                 TEventCode aType );

private:  // own helpers


    /**
     * Starts the command absorbing.
     *
     * @since 2.1
     */
    void StartAbsorbingL();


    /**
     * Stops command absorbing if needed.
     *
     * @since 2.1
     */
    void StopAbsorbingIfNeeded();


    /**
     * Alters CBA visibility.
     *
     * @since 2.1
     * @param aVisible If ETrue makes buttons visible. Else
     * makes them invisible.
     */
    void MakeCbaVisible( TBool aVisible ) const;


    /**
     * Helper to clear owned command absorber.
     *
     * @since 2.1
     * @param aCommandAbsorber The self pointer.
     */
    static void ClearCommandAbsorb( TAny* aCommandAbsorber );



private: //data

    ///<Application under what this is running, not owned
    CEikAppUi*                  iAppUi;

    ///<Used CBA button container, not owned
    CEikButtonGroupContainer*   iCba;

    ///<Command flusher, owned
    CCommandFlush               iCommandFlusher;

    ///<Is command absorbion on, owned
    TBool                       iAbsorbOn;

    };

#endif      // __CCOMMANDABSORBINGCONTROL_H
// End of File

