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
* Description:  Templated notify mediator implementation.
*
*/

#ifndef GENOBSERVERNOTIFYMEDIATORS_H__
#define GENOBSERVERNOTIFYMEDIATORS_H__

//  INCLUDES
#include <E32Std.h>
#include "RGenericObserverArray.h"


// CLASS DECLARATION
/**
 * Templated notify mediator implementation.
 *
 * Suitable for observers which have event and error
 * functions type:
 *   - void ObserverClass::EventFuncL( Type1 aArg1,
 *                                     Type2 aArg2 )
 *   - void ObserverClass::ErrorFunc( TInt aLeaveError,
 *                                    Type1 aArg1 )
 *
 * Inline functions left to header for clarity.
 *
 * @since 3.0
 */
template < class _ObsClass,
class _Arg1,
class _Arg2 >
class TGenNotifyMediator2
            : public MGenObserverNotifyMediator< _ObsClass >
    {
    public: //Constructor

        explicit TGenNotifyMediator2(
            void ( _ObsClass::* aEventFuncL )( _Arg1, _Arg2 ),
            void ( _ObsClass::* aErrorFunc )( TInt, _Arg1 ),
            _Arg1 aArg1,
            _Arg2 aArg2 )
                : iEventFuncL( aEventFuncL ),
                iErrorFunc( aErrorFunc ),
                iArg1( aArg1 ),
                iArg2( aArg2 )
            {
            }


    private: //From MGenObserverNotifyMediator

        void MediateNotifyL( _ObsClass& aObserverToNotify )
            {
            ( aObserverToNotify.*iEventFuncL )( iArg1, iArg2 );
            }

        void MediateNotifyError( _ObsClass& aObserverToNotify,
                                 TInt aLeaveError )
            {
            ( aObserverToNotify.*iErrorFunc )( aLeaveError, iArg1 );
            }


    private: //Data
        //OWN: Event handler function
        void ( _ObsClass::* iEventFuncL )( _Arg1, _Arg2 );

        //OWN: Error handler function
        void ( _ObsClass::* iErrorFunc )( TInt, _Arg1 );

        //OWN: Templated argument
        _Arg1 iArg1;

        //OWN: Templated argument
        _Arg2 iArg2;
    };



#endif      //GENOBSERVERNOTIFYMEDIATORS_H__


//  End of File

