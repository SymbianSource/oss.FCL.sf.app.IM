/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Actively wait until observer is called, store the received values to same object
*
*
*
*/

#ifndef __CObserverWait_H__
#define __CObserverWait_H__

// INCLUDES
#include <e32base.h>
#include <e32svr.h>

#include "TStaticWrapper.h"

//##ModelId=3EBA47C203C4
template < class T1, class T2 = TInt, class T3 = TInt, class T4 = TInt, class T5 = TInt, class T6 = TInt, class T7 = TInt >
class CObserverWait : public CBase
    {
        //##ModelId=3EBA47C203DC
        CActiveSchedulerWait iWait;
        //##ModelId=3EBA47C203E4
        TBool iTimeout;
        //##ModelId=3EBA47C3000C
        TStaticWrapperVoid<CObserverWait> iWrap;
        //##ModelId=3EBA47C30011
        CAsyncCall* iCall;
    public:
        //##ModelId=3EBA47C3001A
        static CObserverWait* NewLC()
            {
            CObserverWait* _that = new( ELeave )CObserverWait;
            CleanupStack::PushL( _that );
            _that->ConstructL();
            return _that;
            }

        //##ModelId=3EBA47C3001C
        static CObserverWait* NewL()
            {
            CObserverWait* _that = NewLC();
            CleanupStack::Pop();
            return _that;
            }

        //##ModelId=3EBA47C3001E
        ~CObserverWait()
            {
            delete iCall;
            }

    protected:
        //##ModelId=3EBA47C3001F
        CObserverWait()
                : iWrap( this, &CObserverWait::Cancel )
            {
            }
        //##ModelId=3EBA47C30020
        void ConstructL()
            {
            iCall = CAsyncCall::NewL();
            }

    public:

        //##ModelId=3EBA47C30021
        void SetValue1( T1 aValue )
            {
            iT1 = aValue;
            }
        //##ModelId=3EBA47C30023
        void SetValue2( T2 aValue )
            {
            iT2 = aValue;
            }
        //##ModelId=3EBA47C3002B
        void SetValue3( T3 aValue )
            {
            iT3 = aValue;
            }
        //##ModelId=3EBA47C3002D
        void SetValue4( T4 aValue )
            {
            iT4 = aValue;
            }
        //##ModelId=3EBA47C3002F
        void SetValue5( T5 aValue )
            {
            iT5 = aValue;
            }
        //##ModelId=3EBA47C30039
        void SetValue6( T6 aValue )
            {
            iT6 = aValue;
            }
        //##ModelId=3EBA47C3003B
        void SetValue7( T7 aValue )
            {
            iT7 = aValue;
            }

        //##ModelId=3EBA47C3003D
        TBool Wait( TInt aTimeout = -1 )
            {
            iTimeout = EFalse;
            if ( aTimeout != -1 )
                {
                iCall->Start( &iWrap, aTimeout );
                }
            iWait.Start();
            iCall->Stop();
            return !Timeout();
            }
        //##ModelId=3EBA47C3003F
        void Completed( TBool aPanic = ETrue )
            {
            if ( iWait.IsStarted() )
                {
                iWait.AsyncStop();
                }
            else if ( aPanic )
                {
                User::Panic( _L( "CObserverWait" ), 1 );
                }
            }

        //##ModelId=3EBA47C30049
        void Cancel()
            {
            if ( iWait.IsStarted() )
                {
                iWait.AsyncStop();
                }
            iTimeout = ETrue;
            }

        //##ModelId=3EBA47C3004A
        TBool Timeout()
            {
            return iTimeout;
            }

        //##ModelId=3EBA47C3004B
        T1 Value1()
            {
            return iT1;
            }
        //##ModelId=3EBA47C3004C
        T2 Value2()
            {
            return iT2;
            }
        //##ModelId=3EBA47C3004D
        T3 Value3()
            {
            return iT3;
            }
        //##ModelId=3EBA47C3004E
        T4 Value4()
            {
            return iT4;
            }
        //##ModelId=3EBA47C3004F
        T5 Value5()
            {
            return iT5;
            }
        //##ModelId=3EBA47C30050
        T6 Value6()
            {
            return iT6;
            }
        //##ModelId=3EBA47C30059
        T7 Value7()
            {
            return iT7;
            }

    private:

        //##ModelId=3EBA47C3005A
        T1 iT1;
        //##ModelId=3EBA47C3005B
        T2 iT2;
        //##ModelId=3EBA47C3005C
        T3 iT3;
        //##ModelId=3EBA47C3005D
        T4 iT4;
        //##ModelId=3EBA47C30068
        T5 iT5;
        //##ModelId=3EBA47C30069
        T6 iT6;
        //##ModelId=3EBA47C3006A
        T7 iT7;
    };


#endif // __CObserverWait_H__
// End of File









